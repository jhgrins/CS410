//
// Created by jhgri on 10/28/2019.
//

#include "Sphere.h"


using namespace std;

Sphere::Sphere(const Eigen::Vector3d &pos, double rad, const Material &mat) : position(pos), radius(rad),
    mat(mat){}

bool Sphere::checkIntersect(Ray &ray) const {
    double rad = radius;

    Eigen::Vector3d c = position - ray.point;

    double csq = c.dot(c); // num

    auto v = c.dot(ray.direct);

    auto d_dist = rad * rad - (csq - v * v);

    if(d_dist > 0){
        double distHit = v - sqrt(d_dist);
        if((distHit < ray.best_dist) && (distHit > 0)){
            ray.best_dist = distHit;
            ray.best_point = ray.point + distHit * ray.direct;
            ray.surf_norm = (ray.best_point - position).normalized();
            ray.mat = mat;
            return true; // wrong in notebooks
        }
    }
    return false;
}