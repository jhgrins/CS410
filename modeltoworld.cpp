#include <iostream>
#include <string>
#include <fstream>
#include "Eigen/Eigen/Eigen"
#include <sstream>
#include <vector>
#include <math.h>
#include <sys/stat.h>

#define PI 3.14159265

using namespace std;
using Eigen::MatrixXd;

string newFilename(string orig, string directory, string fileType){
    string dot = ".";
    string finalStr = "";
    size_t found = orig.find(dot);
    if (found != string::npos){
        finalStr = directory + "/" + orig.substr(0, found);
        if(fileType == "txt") {
            finalStr += "_transform";
        }
        finalStr += "_mw00" + fileType;
    }
    ifstream file(finalStr);
    if (file) {
        finalStr = directory + "/" + orig.substr(0, found);
        if(fileType == "txt") {
            finalStr += "_transform";
        }
        finalStr += "_mw01" + fileType;
    }
    return finalStr;
}

Eigen::MatrixXf makeTMatrix(double tx, double ty, double tz){
    Eigen::MatrixXf matrix(4, 4);
    matrix << 1, 0, 0, tx,
              0, 1, 0, ty,
              0, 0, 1, tz,
              0, 0, 0, 1;
    return matrix;
}

MatrixXd makeSMatrix(double scale){
    MatrixXd matrix(4, 4);
    matrix << scale, 0, 0, 0,
              0, scale, 0, 0,
              0, 0, scale, 0,
              0, 0, 0, 1;
    return matrix;
}

Eigen::Vector3d makeMVec(double wx, double wy, double wz){
    if(wx >= wy >= wz){
        wz = 1;
    } else if (wy >= wz >= wx){
        wx = 1;
    } else{
        wy = 1;
    }
    Eigen::Vector3d m(wx, wy, wz);
    return m.normalized();
}

MatrixXd makeZMatrix(double theta){
    double radian = theta * PI / 180.0;
    double z00 = cos(radian);
    double z01 = -1 * sin(radian);
    double z10 = sin(radian);
    double z11 = cos(radian);

    MatrixXd z(4,4);
    z << z00, z01, 0, 0,
         z10, z11, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 1;
    return z;
}
/*
MatrixXd makeRwMatrix(double wx, double wy, double wz){
    Eigen::Vector3d w(wx,wy,wz);
    w = w.normalized();
    Eigen::Vector3d m = makeMVec(wx, wy, wz);
    Eigen::Vector3d u = w.cross(m);
    Eigen::Vector3d v = u.cross(w);
    MatrixXd matrix(4,4);
    matrix << u.x(), u.y(), u.z(), 0,
              v.x(), v.y(), v.z(), 0,
              w.x(), w.y(), w.z(), 0,
              0,    0,      0,     1;
    return matrix;
}
*/
MatrixXd makeBigRMatrix(MatrixXd z, MatrixXd rw){
    MatrixXd rwTranspose(4,4);
    rwTranspose = rw.transpose();
    return rwTranspose * z * rw;
}


MatrixXd getOrigObjMatrix(string filename){
    ifstream file(filename);
    if(!file.good()) {
        cerr << filename << ": file does not exist!" << '\n';
        exit(1);
    }
    string line;
    stringstream matrix;
    while(getline(file, line)) {
        if(line[0] != '#' || (line[0] != 'v' && line[1] != 'n')){
            stringstream ss;
            string description;
            double x, y, z;
            ss << line;

            ss >> description >> x >> y >> z;
            matrix << x << y << z;
        }
    }
}

void writeObjFile(ofstream &file){
    // TODO: write new matrix to new obj file

}

void writeTxtFile(ofstream &file, MatrixXd transform, MatrixXd transformInverse, double sumOrigTran, double sumOrigTranOrig){
    file << "# Transformation matrix" << "\n";

    file << "\n" << "# Inverse transformation matrix" << "\n";

    file << "\n" << "# Sum absolute translations from original to transformed" << "\n"
         << sumOrigTran << "\n";

    file << "# Sum absolute translations from original to transformed to \"original\"" << "\n"
         << sumOrigTranOrig << "\n";
}

void readFile(ifstream &file, string dir){
    string line;

    while(getline(file, line)) {

        if(line[0] != '#') { // only read in non comments!

            stringstream ss;
            string description;
            double wx, wy, wz, theta, scale, tx, ty, tz;
            string obj_filename;

            ss << line;

            ss >> description >> wx >> wy >> wz >> theta >> scale >> tx >> ty >> tz >> obj_filename;

            auto t = makeTMatrix(tx, ty, tz);
            cout << "t" << "\n";
            auto s = makeSMatrix(scale);
            cout << "s" << "\n";
            auto rw = makeRwMatrix(wx, wy, wz);
            cout << "rw" << "\n";
            auto z = makeZMatrix(theta);
            cout << "z" << "\n";
            auto bigR = makeBigRMatrix(z, rw);
            cout << "bigR" << "\n";
            auto m = t * s * bigR;
            cout << "m" << "\n";

            string new_obj_filename = newFilename(obj_filename, dir, "obj");
            string txt_filename = newFilename(obj_filename, dir, "txt");

            cout << "new obj " << new_obj_filename << "\n";
            cout << "txt " << txt_filename << "\n";

            //auto orig_matrix = getOrigObjMatrix(obj_filename);

            ofstream new_obj(new_obj_filename);


            ofstream txt(txt_filename);
            //writeTxtFile(txt, );


            new_obj.close();
        }
    }
}

string makeDir(string filepath){
    int start_pos = 0;
    int end_pos = filepath.length();
    for(int i = 0; i < filepath.length(); i++){
        char c = filepath.at(i);
        if(c == '/'){
            start_pos = i + 1;
        } else if (c == '.'){
            end_pos = i;
        }
    }
    string name = filepath.substr(start_pos, end_pos - start_pos); // substr(start_pos, lenOfSubstr))
    if(name != ""){
        string str = "mkdir " + name;
        const char * command = str.c_str();
        system(command);
    }
    return name;
}

void getFile(int numArgs, char **args){
    if(numArgs == 2){
        string filepath = args[1];
        ifstream file(filepath);
        if(!file.good()) {
            cerr << args[1] << ": file does not exist!" << '\n';
            exit(1);
        }
        string dirname = makeDir(filepath);
        readFile(file, dirname);
        return;
    }
    cerr << "You didn't put in a file dummy." << '\n';
    exit(1);
}

int main(int argc, char **argv)
{
    getFile(argc,argv);

}
