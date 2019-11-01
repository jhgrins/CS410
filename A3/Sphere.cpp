//
// Created by jhgri on 10/28/2019.
//

#include "Sphere.h"


using namespace std;

Sphere::Sphere(const Eigen::Vector3d &pos, double rad, const Material &mat) : position(pos), radius(rad),
    mat(mat){}
/*
bool Sphere::checkIntersect(const Ray &ray){
    return true;

}
*/