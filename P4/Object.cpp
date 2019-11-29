//
// Created by jhgri on 10/28/2019.
//

#include "Object.h"

using namespace std;

Object::Object(const Eigen::Vector3d &w, double theta, double scale, const Eigen::Vector3d &t_xyz, double angle, const std::string &name) : w(w), cutOffAngle(angle){
    t = makeTMatrix(t_xyz.x(), t_xyz.y(), t_xyz.z());
    s = makeSMatrix(scale);
    auto rw = makeRwMatrix(w.x(), w.y(), w.z());
    auto z = makeZMatrix(theta);
    r = makeBigRMatrix(z, rw);

    m = t * s * r;
    readInObj(name);
}

Eigen::MatrixXd Object::makeTMatrix(double tx, double ty, double tz){
    Eigen::MatrixXd matrix(4, 4);
    matrix << 1, 0, 0, tx,
            0, 1, 0, ty,
            0, 0, 1, tz,
            0, 0, 0, 1;
    return matrix;
}

Eigen::MatrixXd Object::makeSMatrix(double scale){
    Eigen::MatrixXd matrix(4, 4);
    matrix << scale, 0, 0, 0,
            0, scale, 0, 0,
            0, 0, scale, 0,
            0, 0, 0, 1;
    return matrix;
}

Eigen::Vector3d Object::makeMVec(double wx, double wy, double wz){
    if(wx >= wy && wy >= wz){
        wz = 1;
    } else if (wy >= wz && wz >= wx){
        wx = 1;
    } else{
        wy = 1;
    }
    Eigen::Vector3d m(wx, wy, wz);
    return m;
}

Eigen::MatrixXd Object::makeZMatrix(double theta){
    double radian = theta * PI / 180.0;
    double z00 = cos(radian);
    double z01 = -1 * sin(radian);
    double z10 = sin(radian);
    double z11 = cos(radian);

    Eigen::MatrixXd z(4,4);
    z << z00, z01, 0, 0,
            z10, z11, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;
    return z;
}

Eigen::MatrixXd Object::makeRwMatrix(double wx, double wy, double wz){
    Eigen::Vector3d w(wx,wy,wz);
    w = w.normalized();
    auto m = makeMVec(wx, wy, wz);
    auto u = m.cross(w);
    u = u.normalized();
    auto v = w.cross(u);

    Eigen::MatrixXd matrix(4,4);
    matrix << u.x(), u.y(), u.z(), 0,
            v.x(), v.y(), v.z(), 0,
            w.x(), w.y(), w.z(), 0,
            0,    0,      0,     1;
    return matrix;
}

Eigen::MatrixXd Object::makeBigRMatrix(const Eigen::MatrixXd &z, const Eigen::MatrixXd &rw){
    Eigen::MatrixXd rwTranspose(4,4);
    rwTranspose = rw.transpose();
    return rwTranspose * z * rw;
}

void Object::readMats(const string &filename){
    ifstream file((filename).c_str()); // Make sure the file path is correct on linux machines!!
    if(!file.good()) {
        cerr << filename << ": file does not exist!" << '\n';
        exit(1);
    }
    string line;
    string mat_name = "";
    double ns, x, y, z;
    Eigen::Vector3d ka,kd,ks;
    int illum = 0;
    while(getline(file, line)) {
        stringstream ss;
        string description;
        ss << line;
        ss >> description;
        if (description == "newmtl") {
            if(mat_name != "") {
                if (illum == 3) {
                    Material mat(ka, kd, ks, ks, mat_name, illum, ns);
                    mats.emplace_back(mat);
                } else {
                    Material mat(ka, kd, ks, Eigen::Vector3d(0, 0, 0), mat_name, illum, ns);
                    mats.emplace_back(mat);
                }
            }
            ss >> mat_name;
            //cout << "name " << mat_name << "\n";
        } else if (description == "Ns") {
            ss >> ns;
        } else if (description == "Ka") {
            ss >> x >> y >> z;
            ka = Eigen::Vector3d(x, y, z);
        } else if (description == "Kd") {
            ss >> x >> y >> z;
            kd = Eigen::Vector3d(x, y, z);
        } else if (description == "Ks") {
            ss >> x >> y >> z;
            ks = Eigen::Vector3d(x, y, z);
        } else if (description == "illum") {
            ss >> illum;
        }
    }
    // for last mat
    if (illum == 3) {
        Material mat(ka, kd, ks, ks, mat_name, illum, ns);
        mats.emplace_back(mat);
    } else {
        Material mat(ka, kd, ks, Eigen::Vector3d(0, 0, 0), mat_name, illum, ns);
        mats.emplace_back(mat);
    }
}

void Object::readInObj(const string &obj_name){
    ifstream obj((obj_name).c_str()); // Make sure the file path is correct on linux machines!!
    if(!obj.good()) {
        cerr << obj_name << ": file does not exist!" << '\n';
        exit(1);
    }
    string line;
    int currMatIndex = 0;
    //cout << "yooooooo" << endl;
    while(getline(obj, line)) {
        if(!(line[0] == 'v' && line[1] == 'n')) { // not a surface normal
            stringstream ss;
            string description;
            ss << line;
            ss >> description;
            if (description == "v") {
                double x, y, z;
                ss >> x >> y >> z;
                Eigen::Vector4d vec(x, y, z, 1);
                auto a = m * vec;
                Eigen::Vector3d vertex(a.x(), a.y(), a.z());
                vertices.emplace_back(vertex);

            } else if (description == "mtllib") {
                string mat_filename;
                ss >> mat_filename;
                readMats(mat_filename);
            } else if (description == "usemtl"){
                string name;
                ss >> name;
                for(size_t i = 0; i < mats.size(); i++){
                    if(mats[i].descriptor == name){
                        currMatIndex = i;
                        break;
                    }
                }
            } else if (description == "f") {
                int v1, v2, v3;
                string blah1, blah2, blah3;
                ss >> v1 >> blah1 >> v2 >> blah2 >> v3 >> blah3;
                //cout << "about to create face" << endl;
                //cout << "v1" << v1 << " v2 " << v2 << " v3 " << v3 << " mat " << currMatIndex << endl;
                Face face(v1, v2, v3, currMatIndex);
                faces.emplace_back(face);
                //cout << "created face" << endl;
            }
        }
    }
    obj.close();
    //cout << "ended reading file" << endl;
    /*for(const auto &face : faces){
        cout << face.vertex_indices[0] << " " << face.vertex_indices[1] << " " << face.vertex_indices[2] << endl;
    }*/
    calcVerNorms();

    /*cout << faces.size() << "\n";
    for(const auto &face : faces){
        cout << face[0] << " " << face[1] << " " << face[2] << " " << face[3]<< "\n";
    }
    for(const auto &mat : mats){
        cout << mat.descriptor << " " << mat.illum << " " << mat.ka << "\n\n" << mat.kd << "\n\n" << mat.kr << "\n\n" << mat.ks << "\n\n";
    }
    for(const auto &vert:vertices){
        cout << vert << "\n\n";
    }*/

    //cout << "length " << vertices.size();
}

void Object::calcVerNorms(){
    for(Face &face : faces) {
        //cout << "Check face" << endl;
        auto a = vertices[face.vertex_indices[0]-1];
        auto b = vertices[face.vertex_indices[1]-1];
        auto c = vertices[face.vertex_indices[2]-1];
        if(!face.calcVertexNorms) {
            //cout << "calc cols" << endl;
            Eigen::Vector3d col1(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
            face.col1 = col1;
            Eigen::Vector3d col2(a.x() - c.x(), a.y() - c.y(), a.z() - c.z());
            face.col2 = col2;
            face.calcVertexNorms = true;
        }
        auto surf_norm1 = face.col1.cross(face.col2).normalized();
        int count = 0;
        for (int ver_i : face.vertex_indices) {
            face.surfVertexNorms[count] = surf_norm1;
            for (Face &other_face : faces) {
                /*cout << "faces each time" << endl;
                for(const auto &face : faces){
                    cout << face.vertex_indices[0] << " " << face.vertex_indices[1] << " " << face.vertex_indices[2] << endl;
                }*/
                if (face.vertex_indices[0] == other_face.vertex_indices[0] &&
                    face.vertex_indices[1] == other_face.vertex_indices[1] && face.vertex_indices[2] == other_face.vertex_indices[2]) {
                    continue;
                } else if (other_face.vertex_indices[0] == ver_i || other_face.vertex_indices[1] == ver_i || other_face.vertex_indices[2] == ver_i) {
                    if(!other_face.calcVertexNorms) {
                        //cout << "Calculating cols" << endl;
                        //cout << other_face.vertex_indices[0] << " " << other_face.vertex_indices[1] << " " << other_face.vertex_indices[2] << endl;
                        auto a1 = vertices[other_face.vertex_indices[0] - 1];
                        //cout << "a1\n" << a1 << endl;
                        auto b1 = vertices[other_face.vertex_indices[1] - 1];
                        //cout << "b1\n" << b1 << endl;
                        auto c1 = vertices[other_face.vertex_indices[2] - 1];
                        //cout << "c1\n" << c1 << endl;
                        Eigen::Vector3d vec1(a1.x() - b1.x(), a1.y() - b1.y(), a1.z() - b1.z());
                        other_face.col1 = vec1;
                        Eigen::Vector3d vec2(a1.x() - c1.x(), a1.y() - c1.y(), a1.z() - c1.z());
                        other_face.col2 = vec2;
                        other_face.calcVertexNorms = true;
                        //cout << "success" << endl;
                    }
                    auto other_norm = other_face.col1.cross(other_face.col2).normalized();

                    if (abs(acos(min(max(surf_norm1.dot(other_norm), -1.0), 1.0)) * (180 / M_PI)) <= cutOffAngle) {
                        face.surfVertexNorms[count] += other_norm;
                    }
                }

            }
            count++;
        }
    }

}

bool Object::checkIntersect(Ray &ray) const{
    bool hit = false;
    const double EPSILON = 1 * pow(10, -5);
    for(const auto &face : faces){
        int mat_index = face.matIndex;
        auto a = vertices[face.vertex_indices[0]-1];
        //cout << ray.point << "\n";
        //cout << "a\n" << b << "\n";
        Eigen::MatrixXd matrix(3,3);
        Eigen::Vector3d col1 = face.col1;
        Eigen::Vector3d col2 = face.col2;
        matrix << col1.x(), col2.x(), ray.direct.x(),
                col1.y(), col2.y(), ray.direct.y(),
                col1.z(), col2.z(), ray.direct.z();
        Eigen::Vector3d vec(a.x()-ray.point.x(), a.y()-ray.point.y(), a.z()-ray.point.z());
        Eigen::Vector3d byt = matrix.inverse() * vec;
        double beta = byt.x();
        double gamma = byt.y();
        double hit_dist = byt.z();
        //cout << byt << "\n\n";
        if((beta >= EPSILON) && (gamma >= EPSILON) && ((beta + gamma) <= (1 - EPSILON)) && (hit_dist > EPSILON)){
            if(ray.best_dist > hit_dist){
                ray.best_dist = hit_dist;
                ray.best_point = ray.point + hit_dist * ray.direct;
                ray.mat = mats[mat_index];
                // calculate smoothing normal for each vertex
                ray.surf_norm = (((1 - beta - gamma) * face.surfVertexNorms[0]) + (beta * face.surfVertexNorms[1]) + (gamma * face.surfVertexNorms[2])).normalized();
                if(ray.direct.dot(ray.surf_norm) > 0){
                    ray.surf_norm *= -1;
                }
                hit = true;
            }
        }
    }
    return hit;
}
