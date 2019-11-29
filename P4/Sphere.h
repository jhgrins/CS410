//
// Created by jhgri on 10/28/2019.
//

#ifndef GRAPHICS_SPHERE_H
#define GRAPHICS_SPHERE_H

#include "Eigen/Eigen/Eigen"
#include "Material.h"
#include "Ray.h"

class Sphere {
public:
    Eigen::Vector3d position;
    double radius;

    Material mat;

    Sphere(const Eigen::Vector3d &, double, const Material &);
    Sphere() = default;
    Sphere(const Sphere &) = default;
    Sphere &operator=(const Sphere &) = default;
    ~Sphere() = default;

    bool checkIntersect(Ray &) const;
};

#endif //GRAPHICS_SPHERE_H
