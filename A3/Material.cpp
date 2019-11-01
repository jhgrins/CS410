//
// Created by jhgri on 10/30/2019.
//

#include "Material.h"

Material::Material(const Eigen::Vector3d &ka, const Eigen::Vector3d &kd,const Eigen::Vector3d &ks,const Eigen::Vector3d &kr): ka(ka), kd(kd),
    ks(ks), kr(kr) {}