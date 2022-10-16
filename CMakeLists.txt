cmake_minimum_required(VERSION 3.22)

project(memorizer CXX)

link_directories(/usr/local/lib)
include_directories(include)
add_subdirectory(src bin)

target_link_libraries(memo icui18n icuuc icudata icuio)
