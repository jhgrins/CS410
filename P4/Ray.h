//
// Created by jhgri on 10/28/2019.
//
#ifndef GRAPHICS_RAY_H
#define GRAPHICS_RAY_H

#include "Eigen/Eigen/Eigen"
#include "Material.h"
#include <float.h>


class Ray {
public:
    Eigen::Vector3d point;
    Eigen::Vector3d direct;
    double best_dist;
    Eigen::Vector3d surf_norm;
    Eigen::Vector3d best_point;
    Material mat;

    Ray(const Eigen::Vector3d &,const Eigen::Vector3d &, double best_d=DBL_MAX);
    Ray() = default;
    Ray(const Ray &) = default;
    Ray &operator=(const Ray &) = default;
    ~Ray() = default;

};


#endif //GRAPHICS_RAY_H
