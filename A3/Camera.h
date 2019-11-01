//
// Created by jhgri on 10/28/2019.
//

#ifndef GRAPHICS_CAMERA_H
#define GRAPHICS_CAMERA_H
#include "Ray.h"
#include "Eigen/Eigen/Eigen"
#include <vector>

class Camera {
public:
    Eigen::Vector3d eye, look, up;
    double left, right, bottom, top;
    double near, width, height;
    Eigen::Vector3d cwv, cuv, cvv;

    void makeCamVec();

    Camera(const Eigen::Vector3d &, const Eigen::Vector3d &, const Eigen::Vector3d &, const std::vector<double> &,double ,double ,double);
    Ray pixelRay(int, int);


    Camera() = default;
    Camera(const Camera &) = default;
    Camera &operator=(const Camera &) = default;
    ~Camera() = default;
};


#endif //GRAPHICS_CAMERA_H
