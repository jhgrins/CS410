cmake_minimum_required(VERSION 3.10)
project(Graphics)
add_compile_options(-Wall -O4)
set(CMAKE_CXX_STANDARD 14)

# Parallel Capabilities
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_executable(Graphics raytracer.cpp Scene.cpp Scene.h Light.cpp Light.h Camera.cpp Camera.h Object.cpp Object.h Sphere.cpp Sphere.h Ray.cpp Ray.h Material.cpp Material.h Face.h)