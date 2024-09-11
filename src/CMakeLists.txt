cmake_minimum_required(VERSION 3.10)

# Project name
project(Showcase)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable
add_executable(main showcase.cc)

# Find and link SDL2 libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

target_link_libraries(main SDL2::SDL2 SDL2::SDL2_image SDL2::SDL2_ttf)
