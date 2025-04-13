cmake_minimum_required(VERSION 3.10)
project(GI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(GI
    src/main.cpp
    src/geometry.cpp
    src/scene.cpp
    src/render.cpp
    src/utils.cpp
)

if(UNIX)
    target_link_libraries(GI m)
endif()