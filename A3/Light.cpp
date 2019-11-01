//
// Created by jhgri on 10/28/2019.
//
#include "Eigen/Eigen/Eigen"
#include "Light.h"

using namespace std;

Light::Light(Eigen::Vector3d &loc, Eigen::Vector3d &col) : location(loc), color(col){}

