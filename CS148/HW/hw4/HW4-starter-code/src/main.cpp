#include <iostream>
#include "Image.h"
#include "Scene.h"
#include "Sphere.h"
#include "Materials.h"
#include "Wall.h"

using namespace std;
using namespace Raytracer148;
using namespace Eigen;

int main() {
    Image im(400, 400);

    Scene scene;

    Materials m;
    m.reflection = 0.0;
    m.surfaceColor = Eigen::Vector3d(1.0, 0, 0);

    Vector3d center;
    center[0] = 0;
    center[1] = 0;
    center[2] = 4;
    scene.addShape(new Sphere(center, 2, m));

    center[0] = -.5;
    center[1] = 1;
    center[2] = 2.5;
    m.surfaceColor = Eigen::Vector3d(0.0, 1.0, 0);
    m.reflection = 0.0;
    scene.addShape(new Sphere(center, .5, m));

    center[0] = .5;
    center[1] = 1.25;
    center[2] = 2.75;
    m.surfaceColor = Eigen::Vector3d(0.0, 0.0, 1.0);
    m.reflection = 0.0;
    scene.addShape(new Sphere(center, .5, m));

    // Setup Wall
    Materials wallM;
    float Value = 10;
    // Top Wall
    wallM.surfaceColor = Eigen::Vector3d(0.9f, 0.9f, 0.9f);
    scene.addShape(new Wall(Value/2, 0, wallM));

    // Bottom Wall
    wallM.surfaceColor = Eigen::Vector3d(0.9f, 0.9f, 0.9f);
    scene.addShape(new Wall(-Value/2, 1, wallM));

    // Left Wall
    wallM.surfaceColor = Eigen::Vector3d(0.8f, 0.2f, 0.2f);
    scene.addShape(new Wall(-Value/2, 2, wallM));

    // Right Wall
    wallM.surfaceColor = Eigen::Vector3d(0.2f, 0.2f, 0.8f);
    scene.addShape(new Wall(Value/2, 3, wallM));

    // Back Wall
    wallM.surfaceColor = Eigen::Vector3d(0.2f, 0.8f, 0.2f);
    scene.addShape(new Wall(Value, 5, wallM));

    scene.render(im);

    im.writePNG("test.png");

    return 0;
}
