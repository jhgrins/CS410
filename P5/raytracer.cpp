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
#include "Object.h"

using namespace std;

vector<Light> createLights(const vector<vector<double>>& orig){
    vector<Light> lits;
    for(vector<double> vec : orig) {
        vector<Eigen::Vector3d> eigenVec;
        Eigen::Vector3d loc(vec[0], vec[1], vec[2]);
        // vec.at(3) <-- w
        Eigen::Vector3d color(vec[4], vec[5], vec[6]);
        Light lit(loc, color);
        lits.emplace_back(lit);
    }
    return lits;
}

vector<Object> createObjs(const vector<vector<double>> & data, const vector<string> &names){
    vector<Object> objs;
    for(size_t i = 0; i < data.size(); i++){
        Eigen::Vector3d w(data[i][0], data[i][1], data[i][2]);
        double theta = data[i][3];
        double scale = data[i][4];
        Eigen::Vector3d t(data[i][5], data[i][6], data[i][7]);
        double angle = data[i][8];
        Object obj(w, theta, scale, t, angle, names[i]);
        objs.emplace_back(obj);
    }
    return objs;
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
        globes.emplace_back(s);
    }
    return globes;
}

Scene readDriver(ifstream &file) {
    string line;
    Eigen::Vector3d eye, look, up, ambient;
    vector<double> bounds(4);
    int width, height;
    double distance = 0;
    vector<vector<double>> lights, spheres, obj_data;
    vector<string> obj_names;
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
            lights.emplace_back(light);
        } else if (descriptor == "sphere"){
            ss >> x >> y >> z >> r >> a_r >> a_g >> a_b >> d_r >> d_g >> d_b >> s_r >> s_g >> s_b >> r_r >> r_g >> r_b;
            vector<double> sphere = {x, y, z, r, a_r, a_g, a_b, d_r, d_g, d_b, s_r, s_g, s_b, r_r, r_g, r_b};
            spheres.emplace_back(sphere);
        } else if (descriptor == "recursionlevel"){
            ss >> depth;
        } else if (descriptor == "model"){
            double theta, scale, tx, ty, tz, angle;
            string obj_filename;
            ss >> x >> y >> z >> theta >> scale >> tx >> ty >> tz >> angle >> obj_filename;
            vector<double> obj = {x, y, z, theta, scale, tx, ty, tz, angle};
            obj_data.emplace_back(obj);
            obj_names.emplace_back(obj_filename);
        }
    }
    Camera camera(eye, look, up, bounds, distance, width, height);
    //cout << camera.cuv << endl;
    // what happens when i have no spheres or no lights or no objs?
    vector<Sphere> real_spheres = createSpheres(spheres);
    vector<Light> real_lights = createLights(lights);
    //cout << "Havent read obj yet" << "\n";
    vector<Object> real_objs = createObjs(obj_data, obj_names);
    //cout << "Lets make a scene object with real objs" << "\n";
    Scene scene(camera, ambient, real_lights, real_spheres, depth, real_objs);
    return scene;
}

void closestObject(Ray &ray, const Scene &scene){
    for(const Sphere &orb : scene.spheres){
        orb.checkIntersect(ray);
    }
    for(const Object &obj : scene.objects){
        obj.checkIntersect(ray);
    }
}

void makePrettyColor(const Scene &scene, Ray &ray, Eigen::Vector3d &rgb, const Eigen::Vector3d &reflec, int depth){
    //cout << "before dist " << ray.best_dist;
    closestObject(ray, scene);
    //cout << "Depth " << depth << " dist " << ray.best_dist;

    if(ray.best_dist != DBL_MAX){
        // materials
        Eigen::Vector3d color = scene.ambient.cwiseProduct(ray.mat.ka);
        auto n = ray.surf_norm;
        for(const Light &lit : scene.lights){
            bool isShadow = false;
            // check if shadow
            auto litToItem_norm = (lit.location - ray.best_point).normalized();
            auto litToItem = (lit.location - ray.best_point);
            Ray litRay(ray.best_point, litToItem_norm, litToItem.norm());
            //double distToLit = litToItem.norm();
            //cout << "distToLit " << distToLit << "\n";
            for(const Sphere &s : scene.spheres){
                if(s.checkIntersect(litRay)){
                    isShadow = true;
                    break;
                }
            }
            if(!isShadow){ // check for shadows for objects
                for(const Object &obj : scene.objects){
                    if(obj.checkIntersect(litRay)){
                        isShadow = true;
                        break;
                    }
                }
            }
            if(!isShadow) {
                double hitItem = n.dot(litToItem_norm);
                if (hitItem > 0) {
                    color += ray.mat.kd.cwiseProduct(lit.color) * hitItem;
                    Eigen::Vector3d toc = (ray.point - ray.best_point).normalized();
                    Eigen::Vector3d spr = ((2 * hitItem * n) - litToItem_norm).normalized();
                    double cdr = toc.dot(spr);
                    if (cdr > 0) {
                        Eigen::Vector3d vec(ray.mat.ks.cwiseProduct(lit.color));
                        color += vec * pow(cdr, 16);
                    }
                }
            }
        }
        rgb += reflec.cwiseProduct(color);
        if(depth > 0){
            //cout << "Recursion " << depth << endl;
            Eigen::Vector3d uinv = -1 * ray.direct;
            Eigen::Vector3d refR = ((2 * n.dot(uinv) * n) - uinv).normalized();
            Ray new_ray = Ray(ray.best_point, refR);
            Eigen::Vector3d new_vec(ray.mat.ks.cwiseProduct(reflec));
            makePrettyColor(scene, new_ray, rgb, new_vec, --depth);
        }
    }
}
void createPic(Scene &scene, const string &filename){
    int height = scene.cam.height;
    int width = scene.cam.width;

    vector<vector<vector<int>>> picture(height);

    cout << "Beginning Raytracing..." << endl;
    int counter = 10;

    //how to do MP here to speed things up
    #pragma omp parallel for num_threads(omp_get_max_threads() - 1) schedule(dynamic)
    for(size_t i = 0; i < height; i++){
        picture[i] = vector<vector<int>>(width);
        for(size_t j = 0; j < width; j++){
            picture[i][j] = vector<int>(3);
            auto uvVecs = scene.cam.pixelRay(j, i);
            //cout << "(" << i << " " << j << ") ";
            Eigen::Vector3d rgb(0, 0, 0);
            Eigen::Vector3d reflect(1.0, 1.0, 1.0);

            //Before we processed each ray
            //now we we will process a sample
            makePrettyColor(scene, uvVecs, rgb, reflect, scene.depth);
            rgb *= 255;
            vector<int> rgb_int;
            int red = (int)rgb.x();
            if(red > 255){
                red = 255;
            } else if(red < 0) {
                red = 0;
            }
            int green = (int)rgb.y();
            if(green > 255){
                green = 255;
            } else if(green < 0) {
                green = 0;
            }
            int blue = (int)rgb.z();
            if(blue > 255){
                blue = 255;
            } else if(blue < 0) {
                blue = 0;
            }

            picture[i][j][0] = red;
            picture[i][j][1] = green;
            picture[i][j][2] = blue;
        }

        //make sure threads don't all update this
        #pragma omp critical
        if(static_cast<int>(floor((static_cast<double>(i) / scene.cam.height) * 100)) == counter)
        {

            cout << floor((static_cast<double>(i) / scene.cam.height) * 100) << "% complete..." << endl;

            counter += 10;

        }

    }
    cout << "100% complete!" << endl;
    // write stuff to file now
    ofstream ppm;
    ppm.open (filename);
    ppm << "P3\n" << height << " " << width << " " << 255 << '\n';
    for(size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            ppm << picture[i][j][0] << " " << picture[i][j][1] << " " << picture[i][j][2] << " ";
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
        createPic(sc, ppmName);
    } else {
        cerr << "You didn't put in any files dummy." << '\n';
        return 1;
    }
    return 0;
}
