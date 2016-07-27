#include "Scene.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace Raytracer148;
using namespace std;
using namespace Eigen;

#define MAX_RAY_DEPTH 5
#define DEBUG 0
#define VDB 0

#if VDB
#include <vdb.h>
#endif

bool Scene::CheckBlock(Eigen::Vector3d &lightPos, Eigen::Vector3d &hitPos) {
    Ray ray;
    ray.origin = hitPos;
    ray.direction = (lightPos - hitPos).normalized();
    HitRecord r;
    Vector3d hitTolight = lightPos - hitPos;

    for(int i=0; i<shapes.size(); i++) {

        r = shapes[i]->intersect(ray);

        if (r.t <= std::numeric_limits<double>::epsilon()) {
            // find one shape blocking the light source.
            return false;
        }

        Vector3d hitTohit = ray.origin + r.t * ray.direction;

        if(hitTohit.norm() < hitTolight.norm())
            return true;
    }

    return false;
}

HitRecord Scene::closestHit(const Ray &ray) {
    HitRecord result;
    result.t = -1;
    bool foundSomething = false;

    for (unsigned int i = 0; i < shapes.size(); i++) {
        HitRecord r = shapes[i]->intersect(ray);
        if (r.t > std::numeric_limits<double>::epsilon() && (!foundSomething || r.t < result.t)) {
            result = r;
            foundSomething = true;
        }
    }

    return result;
}

// Returns an RGB color, where R/G/B are each in the range [0,1]
Vector3d Scene::trace(const Ray &ray, const int &depth) {
    // For now we'll have diffuse shading with no shadows, so it's easy!
    HitRecord r = closestHit(ray);

    Vector3d result;
    result[0] = result[1] = result[2] = 0;

    if (r.t < numeric_limits<double>::epsilon()) return result;

    float bias = 1e-4;
    bool inside = false;

    // make sure normal and ray direction are on the same side
    if((ray.direction).dot(r.normal) > 0) {
        inside = true;
        r.normal = -r.normal;
    }

#if DEBUG
    printf("begin of the trace function depth=%d \n", depth);
    printf("transparency %f reflection %f \n", r.m.transparency, r.m.reflection);
#endif

    if((r.m.transparency > 0 || r.m.reflection > 0) && depth < MAX_RAY_DEPTH) {
        // handle transparency and reflection
        float costheta = (-ray.direction).dot(r.normal);
        float F0 = 0.1;
        float fresneleffect = F0 + (1 - F0) * pow(1 - costheta, 5);
        if(r.m.transparency == 0)
            fresneleffect = 1;

        Vector3d reflectDir = (ray.direction + r.normal * 2 * costheta).normalized();

        Ray reflectRay;
        reflectRay.origin = r.position + r.normal * bias;
        reflectRay.direction = reflectDir;
        Vector3d reflectColor = trace(reflectRay, depth + 1);

#if VDB
        vdb_line(reflectRay.origin[0], reflectRay.origin[1], reflectRay.origin[2], 
                 reflectRay.origin[0] + reflectRay.direction[0], reflectRay.origin[1] + reflectRay.direction[1], reflectRay.origin[2] + reflectRay.direction[2]);

        vdb_line(ray.origin[0], ray.origin[1], ray.origin[2], 
                 reflectRay.origin[0], reflectRay.origin[1], reflectRay.origin[2]);

        vdb_line(reflectRay.origin[0], reflectRay.origin[1], reflectRay.origin[2],
                 reflectRay.origin[0] + r.normal[0], reflectRay.origin[1] + r.normal[1], reflectRay.origin[2] + r.normal[2]);

#endif
        // the result is a mix of reflection and refraction (if the sphere is transparent)
        // refraction
        Vector3d refractionColor;
        refractionColor[0] = refractionColor[1] = refractionColor[2] = 0;

        if (r.m.transparency) {
            float ior = 1.1;
            float eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
            float cosi = -(r.normal).dot(ray.direction);
            float k = 1 - eta * eta * (1 - cosi * cosi);

            if(k >= 0) {
                Vector3d refractDir = (ray.direction) * eta - eta * (r.normal) * cosi + (r.normal) * (eta *  cosi - sqrt(k));
                refractDir.normalized();

                Ray refractRay;
                refractRay.origin = r.position - r.normal * bias;
                refractRay.direction = refractDir;

                refractionColor = trace(refractRay, depth + 1);
            }
        }

        Vector3d combinedColor = (
            reflectColor * fresneleffect +
            refractionColor * (1 - fresneleffect) * r.m.transparency);

#if DEBUG
        printf("fresnel %f reflectColor %f %f %f \n", fresneleffect, refractionColor[0], refractionColor[1], refractionColor[2]);
#endif
        result[0] = combinedColor[0] * r.m.surfaceColor[0];
        result[1] = combinedColor[1] * r.m.surfaceColor[1];
        result[2] = combinedColor[2] * r.m.surfaceColor[2];

    } else {
        Vector3d rayToLight = (lightPos - r.position).normalized();
        double dot = rayToLight.dot(r.normal);
        if (dot < 0) {
          dot = 0;
          //printf("dot %f \n", dot);
          //printf("r.position %f %f %f lightPos %f %f %f \n", r.position[0], r.position[1], r.position[2], lightPos[0], lightPos[1], lightPos[2]);
          //printf("normal %f %f %f \n", r.normal[0], r.normal[1], r.normal[2]);
        }

        Vector3d mordifiedHitPos = r.normal * bias + r.position;
        bool iblock = CheckBlock(lightPos, mordifiedHitPos);
        if(iblock) {
            dot = 0;
        }     

        // diffuse
        float diffuse = dot ? r.m.kd : 0;

        result[0] = (r.m.ka + diffuse) * r.m.surfaceColor[0];
        result[1] = (r.m.ka + diffuse) * r.m.surfaceColor[1];
        result[2] = (r.m.ka + diffuse) * r.m.surfaceColor[2];
#if 0
        printf("diffuse %f  color %f %f %f\n", diffuse, result[0], result[1], result[2]);
#endif
    }

    return result;
}

#define N 4

void Scene::render(Image &image) {
    // We make the assumption that the camera is located at (0,0,0)
    // and that the image plane happens in the square from (-1,-1,1)
    // to (1,1,1).

    assert(image.getWidth() == image.getHeight());

    int size = image.getWidth();
    double pixelSize = 2. / size;
    for (int x = 0; x < size; x++)
        for (int y = 0; y < size; y++) {
            Vector3d color = Vector3d(0, 0, 0);
            for(int z = 0; z < N * N; z++) {
                Ray curRay;
                curRay.origin[0] = curRay.origin[1] = curRay.origin[2] = 0;
                int j = z / N;
                int i = z % N;
                float delta = 1.0 / N;
                float rx = ((float)rand() / RAND_MAX) - 0.5; // -0.5 to 0.5
                rx = rx * delta;
                float ry = ((float)rand() / RAND_MAX) - 0.5; // -0.5 to 0.5
                ry = ry * delta;

                float xoffset = delta/2.0 + i * delta + rx;
                float yoffset = delta/2.0 + j * delta + ry;


                curRay.direction[0] = (x + xoffset) * pixelSize - 1;
                curRay.direction[1] = 1 - (y + yoffset) * pixelSize;
                curRay.direction[2] = 1;

                color += trace(curRay, 0);              
            }

            image.setColor(x, y, color[0] / (N * N), color[1] / (N * N), color[2]/(N * N));
        }
}
