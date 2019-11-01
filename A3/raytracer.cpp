#include <iostream>
#include <string>
#include <fstream>
#include "Eigen/Eigen/Eigen"
#include <sstream>
#include <vector>
#include <math.h>
#include <sys/stat.h>
#include <iomanip>
#include <array>

#include "Scene.h"
#include "Sphere.h"
#include "Light.h"
#include "Ray.h"
#include "Camera.h"
#include "Material.h"

using namespace std;

vector<Light> createLights(const vector<vector<double>>& orig){
    vector<Light> lits;
    for(vector<double> vec : orig) {
        vector<Eigen::Vector3d> eigenVec;
        Eigen::Vector3d loc(vec[0], vec[1], vec[2]);
        // vec.at(3) <-- w
        Eigen::Vector3d color(vec[4], vec[5], vec[6]);
        Light lit(loc, color);
        lits.push_back(lit);
    }
    return lits;
}

vector<Sphere> createSpheres(const vector<vector<double>>& orig){
    vector<Sphere> globes;
    for(vector<double> v : orig){
        Eigen::Vector3d pos(v[0], v[1], v[2]);
        double rad = v.at(3);
        Eigen::Vector3d ka(v[4], v[5], v[6]);
        //cout << "\nka\n" << ka << "\n";
        Eigen::Vector3d kd(v[7], v[8], v[9]);
        //cout << "\nkd\n" << kd << "\n";
        Eigen::Vector3d ks(v[10], v[11], v[12]);
        //cout << "\nks\n" << ks << "\n";
        Eigen::Vector3d kr(v[13], v[14], v[15]);
        //cout << "\nkr\n" << kr << "\n";
        //cout << "hi" << endl;
        Material mat(ka, kd, ks, kr);
        //cout << "here!" << endl;
        Sphere s(pos, rad, mat);
        globes.push_back(s);
    }
    return globes;
}

Scene readDriver(ifstream &file) {
    string line;
    Eigen::Vector3d eye, look, up, ambient;
    vector<double> bounds(4);
    int width, height;
    double distance = 0;
    vector<vector<double>> lights, spheres;
    int depth;
    while (getline(file, line)) {
        stringstream ss;
        string descriptor;
        ss << line;
        ss >> descriptor;
        double x, y, z, w, r, g, b, a_r, a_g, a_b, d_r, d_g, d_b, s_r, s_g, s_b, r_r, r_g, r_b;
        double left, right, bottom, top;
        if (descriptor == "eye"){
            ss >> x >> y >> z;
            eye = Eigen::Vector3d(x, y, z);
        } else if (descriptor == "look"){
            ss >> x >> y >> z;
            look = Eigen::Vector3d(x, y, z);
        } else if (descriptor == "up"){
            ss >> x >> y >> z;
            up = Eigen::Vector3d(x, y, z);
        } else if (descriptor == "d"){
            ss >> x;
            distance = x;
        } else if (descriptor == "bounds"){
            ss >> left >> right >> bottom >> top;
            bounds[0] = left;
            bounds[1] = right;
            bounds[2] = bottom;
            bounds[3] = top;
        } else if (descriptor == "res"){
            ss >> width >> height;
        } else if (descriptor == "ambient"){
            ss >> r >> g >> b;
            ambient = Eigen::Vector3d(r, g, b);
        } else if (descriptor == "light"){
            ss >> x >> y >> z >> w >> r >> g >> b;
            vector<double> light = {x, y, z, w, r, g, b};
            lights.push_back(light);
        } else if (descriptor == "sphere"){
            ss >> x >> y >> z >> r >> a_r >> a_g >> a_b >> d_r >> d_g >> d_b >> s_r >> s_g >> s_b >> r_r >> r_g >> r_b;
            vector<double> sphere = {x, y, z, r, a_r, a_g, a_b, d_r, d_g, d_b, s_r, s_g, s_b, r_r, r_g, r_b};
            spheres.push_back(sphere);
        } else if (descriptor == "recursionlevel"){
            ss >> depth;
        }
    }
    Camera camera(eye, look, up, bounds, distance, width, height);
    //cout << camera.cuv << endl;
    vector<Sphere> real_spheres = createSpheres(spheres);
    vector<Light> real_lights = createLights(lights);
    Scene scene(camera, ambient, real_lights, real_spheres, depth);
    return scene;
}

Sphere closestSphere(Ray &ray, const Scene &scene){
    for(const Sphere &orb : scene.spheres){
        ray.isBest(orb);
    }
}

void makePrettyColor(const Scene &scene, Ray &ray, Eigen::Vector3d &rgb, Eigen::Vector3d &reflec, int depth){
    cout << "before dist " << ray.best_dist;
    closestSphere(ray, scene);
    cout << "Depth " << depth << " dist " << ray.best_dist;

    if(ray.best_dist != DBL_MAX){
        auto n = (ray.best_point - ray.best_sphere.position).normalized();
        // materials
        Eigen::Vector3d color = scene.ambient.cwiseProduct(ray.best_sphere.mat.ka);

        for(const Light &lit : scene.lights){
            bool isShadow = false;
            auto litToItem = (lit.location - ray.best_point).normalized();
            double hitItem = n.dot(litToItem);
            // check if shadow
            Ray litRay(ray.best_point, litToItem);
            for(Sphere s : scene.spheres){
                if(litRay.checkIntersect(s)){
                    isShadow = true;
                }
            }
            if(!isShadow) {
                if (hitItem > 0) {
                    color += ray.best_sphere.mat.kd.cwiseProduct(lit.color) * hitItem;
                    Eigen::Vector3d toc = (ray.point - ray.best_point).normalized();
                    Eigen::Vector3d spr = ((2 * hitItem * n) - litToItem).normalized();
                    double cdr = toc.dot(spr);
                    if (cdr > 0) {
                        Eigen::Vector3d vec(ray.best_sphere.mat.ks.cwiseProduct(lit.color));
                        color += vec * pow(cdr, 16);
                    }
                }
            }
        }
        rgb += reflec.cwiseProduct(color);

        if(depth > 0){
            cout << "Recursion " << depth << endl;
            Eigen::Vector3d uinv = -1 * ray.direct;
            Eigen::Vector3d refR = ((2 * n.dot(uinv) * n) - uinv).normalized();
            Ray new_ray = Ray(ray.best_point, refR);
            Eigen::Vector3d new_vec(ray.best_sphere.mat.ks.cwiseProduct(reflec));
            makePrettyColor(scene, new_ray, rgb, new_vec, --depth);
        }
    }
}
void writePPM(Scene &scene, const string &filename){
    ofstream ppm;
    ppm.open (filename);

    ppm << "P3\n" << scene.cam.height << " " << scene.cam.width << " " << 255 << '\n';

    for(size_t i = 0; i < scene.cam.height; i++){
        for(size_t j = 0; j < scene.cam.width; j++){
            auto uvVecs = scene.cam.pixelRay(j, i);
            cout << "(" << i << " " << j << ") ";
            Eigen::Vector3d rgb(0, 0, 0);
            Eigen::Vector3d reflect(1.0, 1.0, 1.0);
            makePrettyColor(scene, uvVecs, rgb, reflect, scene.depth);
            rgb *= 255;
            ppm << (int)rgb.x() << " " << (int)rgb.y() << " " << (int)rgb.z() << " ";
        }
        ppm << "\n";
    }
    ppm.close();
}

int main(int argc, char **argv) {
    if(argc == 3){
        string driverName = argv[1];
        ifstream driver(driverName);
        if(!driver){
            cerr << "Could not open file: " << driverName <<"\n";
            return 1;
        }
        string ppmName = argv[2];
        Scene sc = readDriver(driver);
        readDriver(driver);/*
        cout << "Lights\n" << endl;
        for(Light lit : sc.lights){
            cout << "color\n" << lit.color << "loc\n" << lit.location;
        }
        cout << "Ambient\n" << sc.ambient;
        cout << "Camera\n" << "eye\n" << sc.cam.eye << "look\n" << sc.cam.look << "up\n" << sc.cam.up;
        cout << "cuv\n" << sc.cam.cuv << "cvv\n" << sc.cam.cvv << "cwv\n" << sc.cam.cwv;
        cout << "left " << sc.cam.left << " right " << sc.cam.right << " top " << sc.cam.top << " bottom " << sc.cam.bottom;
        cout << "\n" << "width " << sc.cam.width << " height " << sc.cam.height << " near " << sc.cam.near;
        cout << "\nSpheres\n";
        for(Sphere sp : sc.spheres){
            cout << "position\n" << sp.position << "\nRadius " << sp.radius;
            //cout << "\nMaterials\n" << "ka\n" << sp.mat.ka
        }*/
        writePPM(sc, ppmName);
    } else {
        cerr << "You didn't put in any files dummy." << '\n';
        return 1;
    }
    return 0;
}