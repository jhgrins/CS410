//
// Created by jhgri on 10/28/2019.
//

#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include "Eigen/Eigen/Eigen"
#include "Material.h"
#include "Ray.h"
#include "Face.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#define PI 3.14159265

class Object {
public:
    std::vector<Material> mats;
    std::vector<Face> faces;
    std::vector<Eigen::Vector3d> vertices;
    Eigen::Vector3d w;
    Eigen::MatrixXd t, s, r, m;
    double cutOffAngle;

    Object() = delete;
    Object(const Eigen::Vector3d &, double, double, const Eigen::Vector3d &, double, const std::string &);
    Object(const Object &) = default;
    Object &operator=(const Object &) = default;
    ~Object() = default;

    void readInObj(const std::string &);
    bool checkIntersect(Ray &) const;

private:
    void calcVerNorms();
    Eigen::MatrixXd makeTMatrix(double, double, double);
    Eigen::MatrixXd makeSMatrix(double);
    Eigen::Vector3d makeMVec(double, double, double);
    Eigen::MatrixXd makeZMatrix(double);
    Eigen::MatrixXd makeRwMatrix(double, double, double);
    Eigen::MatrixXd makeBigRMatrix(const Eigen::MatrixXd &, const Eigen::MatrixXd &);
    void readMats(const std::string &);
};


#endif //GRAPHICS_OBJECT_H
