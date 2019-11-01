//
// Created by jhgri on 10/28/2019.
//

#ifndef GRAPHICS_LIGHT_H
#define GRAPHICS_LIGHT_H


class Light {
public:
    Eigen::Vector3d location;
    Eigen::Vector3d color;

    Light(Eigen::Vector3d &, Eigen::Vector3d &);
    Light() = default;
    Light(const Light &) = default;
    Light &operator=(const Light &) = default;
    ~Light() = default;
};


#endif //GRAPHICS_LIGHT_H
