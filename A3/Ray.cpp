//
// Created by jhgri on 10/28/2019.
//
#include <iostream>
#include "Ray.h"

using namespace std;

Ray::Ray(const Eigen::Vector3d &lv, const Eigen::Vector3d &dv) : point(lv), direct(dv){}

bool Ray::isBest(const Sphere &sphere) {
    double rad = sphere.radius;

    Eigen::Vector3d s_pos = sphere.position;

    Eigen::Vector3d c = s_pos - point;

    //auto v = c.dot(direct); // notebook says to do this first

    double csq = c.dot(c); // num

    auto v = c.dot(direct); //was working before not with recurs

    auto d_dist = rad * rad - (csq - v * v);

    if(d_dist > 0){
        double distHit = v - sqrt(d_dist);
        if((distHit < best_dist) && (distHit > 0)){
            best_dist = distHit;
            best_sphere = sphere;
            best_point = point + distHit * direct;
            return true; // wrong in notebooks
        }
    }
    return false;
}

bool Ray::checkIntersect(const Sphere &sphere){
    double rad = sphere.radius;

    Eigen::Vector3d s_pos = sphere.position;

    Eigen::Vector3d c = s_pos - point;

    //auto v = c.dot(direct); // notebook says to do this first

    double csq = c.dot(c); // num

    auto v = c.dot(direct); //was working before not with recurs

    auto d_dist = rad * rad - (csq - v * v);

    if(d_dist > 0){
        double distHit = v - sqrt(d_dist);
        if((distHit < best_dist) && (distHit > 0)){
            best_dist = distHit;
            best_sphere = sphere;
            best_point = point + distHit * direct;
            return true; // wrong in notebooks
        }
    }
    return false;
}
