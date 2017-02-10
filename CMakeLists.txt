cmake_minimum_required(VERSION 3.6)
project(unity3d_inject)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES src/types.h src/main.cpp src/main.h src/utility.cpp src/utility.h src/mono.cpp src/mono.h)

add_subdirectory(subhook)

add_library(unity3d_inject SHARED ${SOURCE_FILES})

target_compile_options(unity3d_inject PRIVATE -fpermissive)

target_link_libraries(unity3d_inject subhook)