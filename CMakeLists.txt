cmake_minimum_required(VERSION 3.20)
project(
  LearnVulkan
  VERSION 1.0.0
  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(libs)
add_subdirectory(src)
# add_subdirectory(customClass)
add_subdirectory(app)
