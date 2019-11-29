//
// Created by jhgri on 10/28/2019.
//

#include "Camera.h"
#include <iostream>

using namespace std;

Camera::Camera(const Eigen::Vector3d &eye, const Eigen::Vector3d &look, const Eigen::Vector3d &up, const vector<double> &bnds,double near,double width,double height) :
    eye(eye),look(look),up(up),left(bnds[0]),right(bnds[1]),bottom(bnds[2]),top(bnds[3]),near(near),width(width),height(height) {
    makeCamVec();
}

void Camera::makeCamVec(){
    cwv = eye - look;
    cwv = cwv.normalized();
    cuv = up.cross(cwv); cuv.normalize();
    cvv = cwv.cross(cuv);
}

Ray Camera::pixelRay(int i, int j){
    vector<Eigen::Vector3d> vecsLU;

    double px = static_cast<double>(i)/(width-1)*(right-left)+left;
    double py = static_cast<double>(j)/(height-1)*(bottom-top)+top;

    Eigen::Vector3d lv = eye + (near * cwv) + (px * cuv) + (py * cvv);
    Eigen::Vector3d uv = lv - eye; uv.normalize();
    Ray ray(lv, uv);
    return ray;
}