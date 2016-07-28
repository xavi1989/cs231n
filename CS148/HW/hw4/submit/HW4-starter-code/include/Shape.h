#ifndef SHAPE_H
#define SHAPE_H

#include <Eigen/Dense>
#include <Materials.h>

namespace Raytracer148 {
struct Ray {
  Eigen::Vector3d origin, direction;
};

class Shape;

struct HitRecord {
  Eigen::Vector3d position, normal;
  double t;
  Materials m;
};

class Shape {
public:
  virtual ~Shape(){}
  virtual HitRecord intersect(const Ray &ray) = 0;
};

}

#endif
