Josette Grinslade A3:
  raytracer.cpp -- my main program
  Scene.cpp -- class to bring everything together
  Scene.h
  Camera.cpp -- Camera vars
  Camera.h
  Sphere.cpp -- Sphere vars
  Sphere.h
  Light.cpp -- Light vars
  Light.h
  Ray.cpp -- Closest sphere
  Ray.h
  
  Eigen -- the library for linear alg
  
  README.txt
  CMakeList.txt -- generates my makefile
 
  
  driver00.txt -- driver given from assignment
  driver01.txt -- driver given
  
  checker.mtl
  checker.obj
  
to compile:
 cmake .
 make

run:
    ./raytracer driver**.txt driver**.ppm

output:
    ./driver**.ppm
  
I was unable to complete the assignment but I completed recursive raytracing and shadows for spheres.
