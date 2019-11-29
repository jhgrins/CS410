//
// Created by jhgri on 10/30/2019.
//

#include "Material.h"

using namespace std;

Material::Material(const Eigen::Vector3d &ka, const Eigen::Vector3d &kd,const Eigen::Vector3d &ks,const Eigen::Vector3d &kr)

: Material(ka,kd,ks,kr, "", 0, 0.0)

{}

Material::Material(const Eigen::Vector3d &ka, const Eigen::Vector3d &kd,const Eigen::Vector3d &ks,const Eigen::Vector3d &kr,
       const string &descript, int ill, double ns): ka(ka), kd(kd),ks(ks),kr(kr), descriptor(descript), illum(ill), ns(ns) {}