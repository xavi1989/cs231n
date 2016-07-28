#ifndef WALL_H
#define WALL_H

#include "Shape.h"
#include "Materials.h"

#define DEBUG_WALL 0

namespace Raytracer148 {
class Wall : public Shape {
public:
    Wall(float v, int type, Materials &m) : Value(v), Type(type), material(m) {
        switch(type) {
            case 0: // TOP Wall
                normal = Eigen::Vector3d(0, -1, 0);
                break;
            case 1: // Bottom Wall
                normal = Eigen::Vector3d(0, 1, 0);
                break;
            case 2: // Left Wall
                normal = Eigen::Vector3d(1, 0, 0);
                break;
            case 3: // Right Wall
                normal = Eigen::Vector3d(-1, 0, 0);
                break;
            case 4: // Front Wall
                normal = Eigen::Vector3d(0, 0, 1);
                break;
            case 5: // Back Wall
                normal = Eigen::Vector3d(0, 0, -1);
                break;
        }
    }

    Eigen::Vector3d GetNormal() const {
        return this->normal;
    }

    virtual HitRecord intersect(const Ray &ray);

private:
    float Value;
    int Type;
    Eigen::Vector3d normal;
    Materials material;
};

HitRecord Wall::intersect(const Ray &ray) {
    HitRecord result;
    result.t = -1;

    double t0 = 0;
    int index = -1;
    if(this->Type == 0 || this->Type == 1) {
        // TOP Bottom
        index = 1;
#if DEBUG_WALL
        printf("this is top/bottom wall \n");
#endif

    } else if(this->Type == 2 || this->Type == 3) {
        // Left Right
        index = 0;

#if DEBUG_WALL
        printf("this is left/right wall \n");
#endif

    } else {
        // Front back
        index = 2;

#if DEBUG_WALL
        printf("this is front/back wall \n");
#endif

    }

    // origin[i] + t * direction[i] = Value
    if(ray.direction[index] == 0) { // no hit
        return result;
    }

    t0 = (this->Value - ray.origin[index]) / (ray.direction[index]);
#if DEBUG_WALL
    printf("wall value %f ray origin %f direction %f\n", this->Value, ray.origin[index], ray.direction[index]);
    printf("t is %f \n", t0);
#endif

    if(t0 <= 0) return result; // no hit

    result.t = t0;
    result.position = ray.origin + result.t * ray.direction;
    result.normal = this->normal;
    result.m = this->material;

    return result;
}

}

#endif
