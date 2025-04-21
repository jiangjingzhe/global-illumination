cmake_minimum_required(VERSION 3.10)
project(GI)

find_package(OpenMP REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
add_compile_options(-O3 -march=native -ffast-math)

aux_source_directory(src src_list)
include_directories(include)
link_directories(lib)

add_executable(GI ${src_list})          

target_link_libraries(GI PRIVATE glfw3)

if(MSVC) 
    target_compile_options(GI PRIVATE "/F2000000") 
    target_link_options(GI PRIVATE "/STACK:20000000") 
endif()

if(OpenMP_CXX_FOUND)
    target_link_libraries(GI PRIVATE OpenMP::OpenMP_CXX)
endif()

if(UNIX)
    target_link_libraries(GI m)
endif()