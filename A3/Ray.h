//
// Created by jhgri on 10/28/2019.
//
#ifndef GRAPHICS_RAY_H
#define GRAPHICS_RAY_H

#include "Eigen/Eigen/Eigen"
#include "Sphere.h"
#include <float.h>


class Ray {
public:
    Eigen::Vector3d point;
    Eigen::Vector3d direct;
    double best_dist = DBL_MAX;
    Sphere best_sphere;
    Eigen::Vector3d best_point;

    Ray(const Eigen::Vector3d &,const Eigen::Vector3d &);
    Ray() = default;
    Ray(const Ray &) = default;
    Ray &operator=(const Ray &) = default;
    ~Ray() = default;

    bool isBest(const Sphere &);

    bool checkIntersect(const Sphere &);

};


#endif //GRAPHICS_RAY_H
