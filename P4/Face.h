//
// Created by jhgri on 11/25/2019.
//

#ifndef GRAPHICS_FACE_H
#define GRAPHICS_FACE_H
#include <vector>

class Face {
public:
    std::vector<int> vertex_indices = std::vector<int>(3);
    bool calcVertexNorms = false;
    Eigen::Vector3d col1;
    Eigen::Vector3d col2;
    std::vector<Eigen::Vector3d> surfVertexNorms = std::vector<Eigen::Vector3d>(3);
    int matIndex;

    Face() = delete;
    Face(int v1, int v2, int v3, int mat_i) : matIndex(mat_i){
        vertex_indices = {v1, v2, v3};
    }
    Face(const Face &) = default;
    Face &operator=(const Face &) = default;
    ~Face() = default;
};


#endif //GRAPHICS_FACE_H
