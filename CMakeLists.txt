cmake_minimum_required(VERSION 3.20)
project(DOpenGL_VS_)
add_executable(DOpenGL_VS math3d.h main.cpp
        main.cpp)
target_link_libraries(DOpenGL_VS ws2_32)