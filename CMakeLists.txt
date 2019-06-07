cmake_minimum_required(VERSION 3.14)

project(json_comparison)
set(CMAKE_CXX_STANDARD 17)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

if (APPLE)
    include_directories(/usr/local/include)
endif()

# Copy Data

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Dependencies

find_library(LIB_BENCHMARK benchmark REQUIRED)

# Executables

add_executable(json_comparison src/main.cpp)
target_link_libraries(json_comparison PRIVATE ${CONAN_LIBS})

add_executable(nlohmann_benchmark src/nlohmann_benchmark.cpp)
target_link_libraries(nlohmann_benchmark PRIVATE ${CONAN_LIBS} ${LIB_BENCHMARK})

add_executable(rapid_benchmark src/rapid_benchmark.cpp)
target_link_libraries(rapid_benchmark PRIVATE ${CONAN_LIBS} ${LIB_BENCHMARK})
