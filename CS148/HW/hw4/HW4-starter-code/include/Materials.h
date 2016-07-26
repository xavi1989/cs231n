#ifndef MATERIALS_H
#define MATERIALS_H

#include <Eigen/Dense>

namespace Raytracer148 {

class Materials {
public:
  Materials(float Ka = 0.1, float Kd = 0.5, float Ks = 0.8, int P = 16, Eigen::Vector3d color = Eigen::Vector3d(1, 1, 1), float trans = 0, float ref = 0) {
    ka = Ka;
    kd = Kd;
    ks = Ks;
    p = P;
    surfaceColor = color;
    transparency = trans;
    reflection = ref;
  }

  Materials(const Materials &tmp) {
    ka = tmp.ka;
    kd = tmp.kd;
    ks = tmp.ks;
    p = tmp.p;
    surfaceColor = tmp.surfaceColor;
    transparency = tmp.transparency;
    reflection = tmp.reflection;
  }

  ~Materials () {}

  float ka;
  float kd;
  float ks;
  int p;
  float transparency;
  float reflection;
  Eigen::Vector3d surfaceColor;
};

}

#endif
