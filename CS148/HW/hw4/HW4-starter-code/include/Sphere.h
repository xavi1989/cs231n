#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include "Materials.h"

namespace Raytracer148 {
class Sphere : public Shape {
public:
    Sphere(Eigen::Vector3d &center, double radius, Materials&m) : c(center), r(radius), material(m){}

    virtual HitRecord intersect(const Ray &ray);

private:
    Eigen::Vector3d c;
    double r;
    Materials material;
};
}

#endif
