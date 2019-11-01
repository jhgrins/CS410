//
// Created by jhgri on 10/30/2019.
//

#ifndef GRAPHICS_MATERIAL_H
#define GRAPHICS_MATERIAL_H

#include <string>
#include "Eigen/Eigen/Eigen"

class Material {
public:
    //std::string descriptor;
    Eigen::Vector3d ka, kd, ks, kr;
    //double ns;
    //int illum;

    Material(const Eigen::Vector3d &,const Eigen::Vector3d &,const Eigen::Vector3d &,const Eigen::Vector3d &);
    Material() = default;
    Material(const Material &) = default;
    Material &operator=(const Material &) = default;
    ~Material() = default;

};


#endif //GRAPHICS_MATERIAL_H
