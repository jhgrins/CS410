cmake_minimum_required(VERSION 3.10)
project(Graphics)
add_compile_options(-Wall -O4)
set(CMAKE_CXX_STANDARD 14)

add_executable(Graphics raytracer.cpp Scene.cpp Scene.h Light.cpp Light.h Camera.cpp Camera.h Object.cpp Object.h Sphere.cpp Sphere.h Ray.cpp Ray.h Material.cpp Material.h Face.h)