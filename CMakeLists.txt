cmake_minimum_required(VERSION 3.31)
project(adampelc.com
  LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Required packages
find_package(GTest CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)
