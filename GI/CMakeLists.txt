cmake_minimum_required(VERSION 3.10)
project(GI)

find_package(OpenMP REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

include_directories(include)

add_executable(GI
    src/main.cpp
    src/geometry.cpp
    src/scene.cpp
    src/render.cpp
    src/utils.cpp
)

if(MSVC) #扩展 MSVC 堆栈大小
    target_compile_options(GI PRIVATE "/F2000000") # 20MB 堆栈
    target_link_options(GI PRIVATE "/STACK:20000000") # 200MB 总栈
endif()

if(OpenMP_CXX_FOUND)
    target_link_libraries(GI PRIVATE OpenMP::OpenMP_CXX)
endif()

if(UNIX)
    target_link_libraries(GI m)
endif()