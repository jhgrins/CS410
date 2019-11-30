//
// Created by jhgri on 10/28/2019.
//
#include <iostream>
#include "Ray.h"

using namespace std;

Ray::Ray(const Eigen::Vector3d &lv, const Eigen::Vector3d &dv, double best_d) : point(lv), direct(dv), best_dist(best_d){}
