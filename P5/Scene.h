//
// Created by jhgri on 10/28/2019.
//

#ifndef GRAPHICS_SCENE_H
#define GRAPHICS_SCENE_H


#include "Eigen/Eigen/Eigen"
#include "Light.h"
#include "Sphere.h"
#include "Camera.h"
#include "Object.h"

class Scene {
public:
    Camera cam;

    Eigen::Vector3d ambient;

    std::vector<Light> lights;
    std::vector<Sphere> spheres;
    std::vector<Object> objects;

    int depth;

    Scene() = delete;
    Scene(Camera &, Eigen::Vector3d &, std::vector<Light> &, std::vector<Sphere> &, int, std::vector<Object>);
    Scene(const Scene &) = default;
    Scene &operator=(const Scene &) = default;
    ~Scene() = default;
};

#endif //GRAPHICS_SCENE_H
