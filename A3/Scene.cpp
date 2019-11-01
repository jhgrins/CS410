//
// Created by jhgri on 10/28/2019.
//
#include "Scene.h"

using namespace std;

Scene::Scene(Camera &camera, Eigen::Vector3d &amb, vector<Light> &lits, vector<Sphere> &orbs, int depth) :
    cam(camera), ambient(amb), lights(lits),spheres(orbs), depth(depth){}



