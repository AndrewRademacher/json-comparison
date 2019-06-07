cmake_minimum_required(VERSION 3.14)

project(json_comparison)
set(CMAKE_CXX_STANDARD 17)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(json_comparison src/main.cpp)
target_link_libraries(json_comparison PRIVATE ${CONAN_LIBS})
