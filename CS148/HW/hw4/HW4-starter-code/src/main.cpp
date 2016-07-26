#include <iostream>
#include "Image.h"
#include "Scene.h"
#include "Sphere.h"
#include "Materials.h"
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

    scene.render(im);

    im.writePNG("test.png");

    return 0;
}
