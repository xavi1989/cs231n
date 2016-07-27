#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <Windows.h>
#include "Group.h"
#include "SimpleImage.h"
#include "Sphere.h"
#include "Utility.h"
#include "Wall.h"
//#define __ADAPTIVE_OPTIMIZATION__

using namespace std;

float getSceneX(int w, int imgWidth, float planeMinX, float planeMaxX) {
    return planeMinX + (float)w / imgWidth * (planeMaxX - planeMinX);
}

float getSceneY(int h, int imgHeight, float planeMinY, float planeMaxY) {
    return planeMaxY - (float)h / imgHeight * (planeMaxY - planeMinY);
}

void usage_message() {
    std::cout << "Usage: ./KX_RayTracer <output_file> <x_res> <y_res> <tracing scene> <effort> <fast_diffuse>" << std::endl;
    std::cout << "{x_res, y_res} resolutions should be given in pixels." << std::endl;
    std::cout << "effort is how many rays to shoot for each pixel." << std::endl;
    std::cout << "fast_diffuse: 1 for fast Lambertian shading. 0 for slow diffuse reflections." << std::endl;
    std::cout << "tracing scences: " << std::endl;
    std::cout << "    1 - basic" << std::endl;
}

std::shared_ptr<Surface> GetScene01() {
    std::shared_ptr<Group> pScene(new Group());

#ifndef DEBUG_DIFFUSE
    // Add a reflective sphere into the scene.
    std::shared_ptr<Sphere> pSphere(new Sphere(Point3f(-3.5f, -5, 10), 3.5f /*radius*/));
    std::shared_ptr<Material> pSphereMaterial(new Material(RGBColor(0.9999f, 0.9999f, 0.9999f)));
    pSphereMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
    pSphereMaterial->SetReflectionType(Type::SPEC);
    pSphere->SetMaterial(pSphereMaterial);
    pScene->AddObject(pSphere);

    // Add a glass sphere into the scene.
    std::shared_ptr<Sphere> pGlassSphere(new Sphere(Point3f(6, -6, 7), 3.0f /*radius*/));
    std::shared_ptr<Material> pGlassSphereMaterial(new Material(RGBColor(0.95f, 0.95f, 0.95f)));
    pGlassSphereMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
    pGlassSphereMaterial->SetReflectionType(Type::REFR);
    pGlassSphereMaterial->SetRefrIndex(1.5f, 1.0f);
    pGlassSphere->SetMaterial(pGlassSphereMaterial);
    pScene->AddObject(pGlassSphere);
#endif

    // Add a light at the top wall.
    std::shared_ptr<Wall> pTopLight(new Wall(Point3f(-4, 9.9f, 14), Point3f(4, 9.9f, 14), Point3f(4, 9.9f, 6), Point3f(-4, 9.9f, 6)));
    std::shared_ptr<Material> pTopLightMaterial(new Material(RGBColor(0.0f, 0.0f, 0.0f)));
    pTopLightMaterial->SetEmissionColor(RGBColor(1.0f, 1.0f, 1.0f));
    pTopLightMaterial->SetReflectionType(Type::LIGHT);
    pTopLight->SetMaterial(pTopLightMaterial);
    pScene->AddObject(pTopLight);

    // Add the back wall.
    std::shared_ptr<Wall> pFrontWall(new Wall(Point3f(-10, 10, -20), Point3f(10, 10, -20), Point3f(10, -10, -20), Point3f(-10, -10, -20)));
    std::shared_ptr<Material> pFrontWallMaterial(new Material(RGBColor(0.5f, 0.5f, 0.5f)));
    pFrontWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
    pFrontWallMaterial->SetReflectionType(Type::DIFF);
    pFrontWall->SetMaterial(pFrontWallMaterial);
    pScene->AddObject(pFrontWall);

    // Add the front wall.
    std::shared_ptr<Wall> pBackWall(new Wall(Point3f(10, 10, 20), Point3f(-10, 10, 20), Point3f(-10, -10, 20), Point3f(10, -10, 20)));
    std::shared_ptr<Material> pBackWallMaterial(new Material(RGBColor(0.2f, 0.8f, 0.2f)));
    pBackWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
    pBackWallMaterial->SetReflectionType(Type::DIFF);
    pBackWall->SetMaterial(pBackWallMaterial);
    pScene->AddObject(pBackWall);

    // Add the top wall.
    std::shared_ptr<Wall> pTopWall(new Wall(Point3f(-10, 10, 20), Point3f(10, 10, 20), Point3f(10, 10, -20), Point3f(-10, 10, -20)));
    std::shared_ptr<Material> pTopWallMaterial(new Material(RGBColor(0.9f, 0.9f, 0.9f)));
    pTopWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
    pTopWallMaterial->SetReflectionType(Type::DIFF);
    pTopWall->SetMaterial(pTopWallMaterial);
    pScene->AddObject(pTopWall);

    // Add the bottom wall.
    std::shared_ptr<Wall> pBottomWall(new Wall(Point3f(10, -10, 20), Point3f(-10, -10, 20), Point3f(-10, -10, -20), Point3f(10, -10, -20)));
    std::shared_ptr<Material> pBottomWallMaterial(new Material(RGBColor(0.9f, 0.9f, 0.9f)));
    pBottomWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
    pBottomWallMaterial->SetReflectionType(Type::DIFF);
    pBottomWall->SetMaterial(pBottomWallMaterial);
    pScene->AddObject(pBottomWall);

    // Add the left wall.
    std::shared_ptr<Wall> pLeftWall(new Wall(Point3f(-10, -10, 20), Point3f(-10, 10, 20), Point3f(-10, 10, -20), Point3f(-10, -10, -20)));
    // std::shared_ptr<Material> pLeftWallMaterial(new Material(RGBColor(0.9f, 0.9f, 0.9f)));
    std::shared_ptr<Material> pLeftWallMaterial(new Material(RGBColor(0.8f, 0.2f, 0.2f)));
    pLeftWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
    pLeftWallMaterial->SetReflectionType(Type::DIFF);
    pLeftWall->SetMaterial(pLeftWallMaterial);
    pScene->AddObject(pLeftWall);

    // Add the right wall.
    std::shared_ptr<Wall> pRightWall(new Wall(Point3f(10, 10, 20), Point3f(10, -10, 20), Point3f(10, -10, -20), Point3f(10, 10, -20)));
    // std::shared_ptr<Material> pRightWallMaterial(new Material(RGBColor(0.9f, 0.9f, 0.9f)));
    std::shared_ptr<Material> pRightWallMaterial(new Material(RGBColor(0.2f, 0.2f, 0.8f)));
    pRightWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
    pRightWallMaterial->SetReflectionType(Type::DIFF);
    pRightWall->SetMaterial(pRightWallMaterial);
    pScene->AddObject(pRightWall);

    return pScene;
}

std::shared_ptr<Surface> GetScene02() {
	std::shared_ptr<Group> pScene(new Group());

	// Add a reflective Pikachu into the scene.
	std::shared_ptr<Surface> pPikuchu = LoadMesh("../KX_RayTracer/Meshes/P2_Pikachu.obj", 1.0f, Vector3f(0.f, -3.f, 10.f));
	std::shared_ptr<Material> pPikachuMaterial(new Material(RGBColor(1.0f, 1.0f, 0.0f)));
	pPikachuMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pPikachuMaterial->SetReflectionType(Type::DIFF);
	pPikuchu->SetMaterial(pPikachuMaterial);
	pScene->AddObject(pPikuchu);

	// Add a light at the top wall.
	std::shared_ptr<Wall> pTopLight(new Wall(Point3f(-4, 9.9f, 14), Point3f(4, 9.9f, 14), Point3f(4, 9.9f, 6), Point3f(-4, 9.9f, 6)));
	std::shared_ptr<Material> pTopLightMaterial(new Material(RGBColor(0.0f, 0.0f, 0.0f)));
	pTopLightMaterial->SetEmissionColor(RGBColor(1.0f, 1.0f, 1.0f));
	pTopLightMaterial->SetReflectionType(Type::LIGHT);
	pTopLight->SetMaterial(pTopLightMaterial);
	pScene->AddObject(pTopLight);

	// Add the front wall.
	std::shared_ptr<Wall> pFrontWall(new Wall(Point3f(-10, 10, -20), Point3f(10, 10, -20), Point3f(10, -10, -20), Point3f(-10, -10, -20)));
	std::shared_ptr<Material> pFrontWallMaterial(new Material(RGBColor(0.5f, 0.5f, 0.5f)));
	pFrontWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pFrontWallMaterial->SetReflectionType(Type::DIFF);
	pFrontWall->SetMaterial(pFrontWallMaterial);
	pScene->AddObject(pFrontWall);

	// Add the back wall.
	std::shared_ptr<Wall> pBackWall(new Wall(Point3f(10, 10, 20), Point3f(-10, 10, 20), Point3f(-10, -10, 20), Point3f(10, -10, 20)));
	std::shared_ptr<Material> pBackWallMaterial(new Material(RGBColor(0.2f, 0.8f, 0.2f)));
	pBackWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pBackWallMaterial->SetReflectionType(Type::DIFF);
	pBackWall->SetMaterial(pBackWallMaterial);
	pScene->AddObject(pBackWall);

	// Add the top wall.
	std::shared_ptr<Wall> pTopWall(new Wall(Point3f(-10, 10, 20), Point3f(10, 10, 20), Point3f(10, 10, -20), Point3f(-10, 10, -20)));
	std::shared_ptr<Material> pTopWallMaterial(new Material(RGBColor(0.95f, 0.95f, 0.95f)));
	pTopWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pTopWallMaterial->SetReflectionType(Type::DIFF);
	pTopWall->SetMaterial(pTopWallMaterial);
	pScene->AddObject(pTopWall);

	// Add the bottom wall.
	std::shared_ptr<Wall> pBottomWall(new Wall(Point3f(10, -10, 20), Point3f(-10, -10, 20), Point3f(-10, -10, -20), Point3f(10, -10, -20)));
	std::shared_ptr<Material> pBottomWallMaterial(new Material(RGBColor(0.95f, 0.95f, 0.95f)));
	pBottomWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pBottomWallMaterial->SetReflectionType(Type::DIFF);
	pBottomWall->SetMaterial(pBottomWallMaterial);
	pScene->AddObject(pBottomWall);

	// Add the left wall.
	std::shared_ptr<Wall> pLeftWall(new Wall(Point3f(-10, -10, 20), Point3f(-10, 10, 20), Point3f(-10, 10, -20), Point3f(-10, -10, -20)));
	std::shared_ptr<Material> pLeftWallMaterial(new Material(RGBColor(0.8f, 0.2f, 0.2f)));
	pLeftWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pLeftWallMaterial->SetReflectionType(Type::DIFF);
	pLeftWall->SetMaterial(pLeftWallMaterial);
	pScene->AddObject(pLeftWall);

	// Add the right wall.
	std::shared_ptr<Wall> pRightWall(new Wall(Point3f(10, 10, 20), Point3f(10, -10, 20), Point3f(10, -10, -20), Point3f(10, 10, -20)));
	std::shared_ptr<Material> pRightWallMaterial(new Material(RGBColor(0.2f, 0.2f, 0.8f)));
	pRightWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pRightWallMaterial->SetReflectionType(Type::DIFF);
	pRightWall->SetMaterial(pRightWallMaterial);
	pScene->AddObject(pRightWall);

	return pScene;
}

std::shared_ptr<Surface> GetScene03() {
	std::shared_ptr<Group> pScene(new Group());

	// Add a diamond sphere into the scene.
	std::shared_ptr<Sphere> pSphere(new Sphere(Point3f(-5.f, -4.f, 7.f), 1.5f /*radius*/));
	std::shared_ptr<Material> pSphereMaterial(new Material(RGBColor(0.9999f, 0.9999f, 0.9999f)));
	pSphereMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pSphereMaterial->SetReflectionType(Type::REFR);
	pSphereMaterial->SetRefrIndex(2.419f, 1.5f);
	pSphere->SetMaterial(pSphereMaterial);
	pScene->AddObject(pSphere);

	// Add a glass sphere into the scene encapsulating the diamond.
	std::shared_ptr<Sphere> pGlassSphere(new Sphere(Point3f(-5.f, -4.f, 7.f), 3.5f /*radius*/));
	std::shared_ptr<Material> pGlassSphereMaterial(new Material(RGBColor(0.95f, 0.95f, 0.95f)));
	pGlassSphereMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pGlassSphereMaterial->SetReflectionType(Type::REFR);
	pGlassSphereMaterial->SetRefrIndex(1.5f, 1.0f);
	pGlassSphere->SetMaterial(pGlassSphereMaterial);
	pScene->AddObject(pGlassSphere);

	// Add a diamond sphere into the scene.
	std::shared_ptr<Sphere> pSphere2(new Sphere(Point3f(5.f, -6.f, 9.f), 3.f /*radius*/));
	std::shared_ptr<Material> pSphere2Material(new Material(RGBColor(0.9999f, 0.9999f, 0.9999f)));
	pSphere2Material->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pSphere2Material->SetReflectionType(Type::REFR);
	pSphere2Material->SetRefrIndex(2.419f, 1.0f);
	pSphere2->SetMaterial(pSphere2Material);
	pScene->AddObject(pSphere2);

	// Add a reflective sphere into the scene inside diamond above.
	std::shared_ptr<Sphere> pGlassSphere2(new Sphere(Point3f(5.f, -6.f, 9.f), 1.f /*radius*/));
	std::shared_ptr<Material> pGlassSphere2Material(new Material(RGBColor(0.95f, 0.95f, 0.95f)));
	pGlassSphere2Material->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pGlassSphere2Material->SetReflectionType(Type::SPEC);
	pGlassSphere2->SetMaterial(pGlassSphere2Material);
	pScene->AddObject(pGlassSphere2);

	// Add a light at the top wall.
	std::shared_ptr<Wall> pTopLight(new Wall(Point3f(-4, 9.9f, 14), Point3f(4, 9.9f, 14), Point3f(4, 9.9f, 6), Point3f(-4, 9.9f, 6)));
	std::shared_ptr<Material> pTopLightMaterial(new Material(RGBColor(0.0f, 0.0f, 0.0f)));
	pTopLightMaterial->SetEmissionColor(RGBColor(1.0f, 1.0f, 1.0f));
	pTopLightMaterial->SetReflectionType(Type::LIGHT);
	pTopLight->SetMaterial(pTopLightMaterial);
	pScene->AddObject(pTopLight);

	// Add the back wall.
	std::shared_ptr<Wall> pFrontWall(new Wall(Point3f(-10, 10, -20), Point3f(10, 10, -20), Point3f(10, -10, -20), Point3f(-10, -10, -20)));
	std::shared_ptr<Material> pFrontWallMaterial(new Material(RGBColor(0.5f, 0.5f, 0.5f)));
	pFrontWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pFrontWallMaterial->SetReflectionType(Type::DIFF);
	pFrontWall->SetMaterial(pFrontWallMaterial);
	pScene->AddObject(pFrontWall);

	// Add the front wall.
	std::shared_ptr<Wall> pBackWall(new Wall(Point3f(10, 10, 20), Point3f(-10, 10, 20), Point3f(-10, -10, 20), Point3f(10, -10, 20)));
	std::shared_ptr<Material> pBackWallMaterial(new Material(RGBColor(0.2f, 0.8f, 0.2f)));
	pBackWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pBackWallMaterial->SetReflectionType(Type::DIFF);
	pBackWall->SetMaterial(pBackWallMaterial);
	pScene->AddObject(pBackWall);

	// Add the top wall.
	std::shared_ptr<Wall> pTopWall(new Wall(Point3f(-10, 10, 20), Point3f(10, 10, 20), Point3f(10, 10, -20), Point3f(-10, 10, -20)));
	std::shared_ptr<Material> pTopWallMaterial(new Material(RGBColor(0.9f, 0.9f, 0.9f)));
	pTopWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pTopWallMaterial->SetReflectionType(Type::DIFF);
	pTopWall->SetMaterial(pTopWallMaterial);
	pScene->AddObject(pTopWall);

	// Add the bottom wall.
	std::shared_ptr<Wall> pBottomWall(new Wall(Point3f(10, -10, 20), Point3f(-10, -10, 20), Point3f(-10, -10, -20), Point3f(10, -10, -20)));
	std::shared_ptr<Material> pBottomWallMaterial(new Material(RGBColor(0.9f, 0.9f, 0.9f)));
	pBottomWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pBottomWallMaterial->SetReflectionType(Type::DIFF);
	pBottomWall->SetMaterial(pBottomWallMaterial);
	pScene->AddObject(pBottomWall);

	// Add the left wall.
	std::shared_ptr<Wall> pLeftWall(new Wall(Point3f(-10, -10, 20), Point3f(-10, 10, 20), Point3f(-10, 10, -20), Point3f(-10, -10, -20)));
	// std::shared_ptr<Material> pLeftWallMaterial(new Material(RGBColor(0.9f, 0.9f, 0.9f)));
	std::shared_ptr<Material> pLeftWallMaterial(new Material(RGBColor(0.8f, 0.2f, 0.2f)));
	pLeftWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pLeftWallMaterial->SetReflectionType(Type::DIFF);
	pLeftWall->SetMaterial(pLeftWallMaterial);
	pScene->AddObject(pLeftWall);

	// Add the right wall.
	std::shared_ptr<Wall> pRightWall(new Wall(Point3f(10, 10, 20), Point3f(10, -10, 20), Point3f(10, -10, -20), Point3f(10, 10, -20)));
	// std::shared_ptr<Material> pRightWallMaterial(new Material(RGBColor(0.9f, 0.9f, 0.9f)));
	std::shared_ptr<Material> pRightWallMaterial(new Material(RGBColor(0.2f, 0.2f, 0.8f)));
	pRightWallMaterial->SetEmissionColor(RGBColor(0.0f, 0.0f, 0.0f));
	pRightWallMaterial->SetReflectionType(Type::DIFF);
	pRightWall->SetMaterial(pRightWallMaterial);
	pScene->AddObject(pRightWall);

	return pScene;
}

void monteCarlo(const std::string& output_name, const ::shared_ptr<Surface>& pScene, int img_w, int img_h, int tracing_scene, int effort) {

    float planeMinX = -10.0f;
    float planeMaxX = 10.0f;
    float planeMinY = -10.0f;
    float planeMaxY = 10.0f;

    float view_radius = (planeMaxX - planeMinX) / img_w / 2.0f;
    float d = 20.0;                // Distance between the viewpoint and the image plane
    __declspec(align(16)) Point3f e(0, 0, -20.0);        // Viewpoint

    // Allocate intermediate image, 16-byte memory aligned
    RGBColor *i_image = (RGBColor *)_aligned_malloc(img_w * img_h * sizeof(RGBColor), 16);
    for (int h = 0; h < img_h; h++) {
        for (int w = 0; w < img_w; w++) {
            *(i_image + h * img_w + w) = RGBColor(0, 0, 0);
        }
    }

    #pragma omp parallel
    {
        int i = omp_get_thread_num();
        printf_s("Hello from thread %d\n", i);
    }

    int count = 0;

    double wall0 = get_wall_time();
    double cpu0 = get_cpu_time();

    // Generate ray based on effort for each pixel and trace for the pixel's color.
#pragma omp parallel for
    for (int h = 0; h < img_h; h++) {
        for (int w = 0; w < img_w; w++) {

            // w and h are in image coordinate, need to convert them into the scene coordinate.
            float x_anch = getSceneX(w, img_w, planeMinX, planeMaxX);
            float y_anch = getSceneY(h, img_h, planeMinY, planeMaxY);

#ifndef __ADAPTIVE_OPTIMIZATION__
            for (int iter = 0; iter < effort; iter++) {
                float x = frand_radius(view_radius) + x_anch;
                float y = frand_radius(view_radius) + y_anch;

                __declspec(align(16)) Vector3f rayDir = Vector3f(x - e.x, y - e.y, d);
                rayDir.Normalize();
                Ray ray(e, rayDir);
                __declspec(align(16)) RGBColor traceR = ray.traceForColor(*pScene, 0 /*depth*/, 0 /*diffuse_depth*/);
                *(i_image + h * img_w + w) = *(i_image + h * img_w + w) + traceR;
            }
            *(i_image + h * img_w + w) = *(i_image + h * img_w + w) * (1.0f / effort);
#else
            int base_effort = effort / 5, inc_effort = effort / 5;
            __declspec(align(16)) RGBColor base_color(0.f, 0.f, 0.f);
            __declspec(align(16)) RGBColor cur_color(0.f, 0.f, 0.f);
            int cur_effort = 0;
            for (int iter = 0; iter < effort; iter++) {
                cur_effort++;
                float x = frand_radius(view_radius) + x_anch;
                float y = frand_radius(view_radius) + y_anch;

                __declspec(align(16)) Vector3f rayDir = Vector3f(x - e.x, y - e.y, d);
                rayDir.Normalize();
                Ray ray(e, rayDir);
                __declspec(align(16)) RGBColor traceR = ray.traceForColor(*pScene, 0 /*depth*/, 0 /*diffuse_depth*/);
                *(i_image + h * img_w + w) = *(i_image + h * img_w + w) + traceR;

                if (cur_effort == base_effort){
                    base_color = *(i_image + h * img_w + w) * (1.0f / cur_effort);
                    inc_effort += base_effort;
                }
                else if (cur_effort == inc_effort){
                    cur_color = *(i_image + h * img_w + w) * (1.0f / cur_effort);
                    if ((fabs(cur_color.r - base_color.r) < 0.001f) && (fabs(cur_color.g - base_color.g) < 0.001f) && (fabs(cur_color.b - base_color.b) < 0.001f)){
                        break;
                    }
                    //if color did not converge at this point, update base_color and run more
                    inc_effort += base_effort;
                    base_color = cur_color;
                }
            }
            *(i_image + h * img_w + w) = *(i_image + h * img_w + w) * (1.0f / cur_effort);
#endif
        }

        #pragma omp atomic
        count++;
        std::cout << "Progress: " << count << "/" << img_h << " completed." << std::endl;
    }

    double wall1 = get_wall_time();
    double cpu1 = get_cpu_time();
    cout << "Wall Time = " << wall1 - wall0 << endl;
    cout << "CPU Time  = " << cpu1 - cpu0 << endl;

    SimpleImage result(img_w, img_h, RGBColor(0, 0, 0));
    for (int h = 0; h < img_h; h++) {
        for (int w = 0; w < img_w; w++) {
            result.set(w, h, *(i_image + h * img_w + w));
        }
    }

    _aligned_free(i_image);
    result.save(output_name);
}

int main(int argc, char **argv) {

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    omp_set_num_threads(sysinfo.dwNumberOfProcessors);

    std::string output_file = "default.png";
    int imgWidth = 128;
    int imgHeight = 128;
    int tracing_scene = 1;
    int effort = 10;
    fUseFastShading = false;

    if (argc != 1) {
        if (argc == 7) {
            output_file = argv[1];
            imgWidth = atoi(argv[2]);
            imgHeight = atoi(argv[3]);
            tracing_scene = atoi(argv[4]);
            effort = atoi(argv[5]);
            fUseFastShading = atoi(argv[6]) == 1;
        }
        else {
            usage_message();
            return 1;
        }
    }

    srand(static_cast<unsigned>(time(NULL)));

    // Get the scene based on the scene number.
    std::shared_ptr<Surface> pScene;
    if (tracing_scene == 1) {
        pScene = GetScene01();
        monteCarlo(output_file, pScene, imgWidth, imgHeight, tracing_scene, effort);
    }
	else if (tracing_scene == 2) {
		pScene = GetScene02();
		monteCarlo(output_file, pScene, imgWidth, imgHeight, tracing_scene, effort);
	}
	else if (tracing_scene == 3) {
		pScene = GetScene03();
		monteCarlo(output_file, pScene, imgWidth, imgHeight, tracing_scene, effort);
	}
    else if (tracing_scene == 100) {
        sse_test(imgWidth, imgHeight, tracing_scene, effort);
    }
    else {
        pScene = GetScene01();
        monteCarlo(output_file, pScene, imgWidth, imgHeight, tracing_scene, effort);
    }

    return 0;
}
