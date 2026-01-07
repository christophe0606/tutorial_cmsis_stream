cmake_minimum_required(VERSION 3.20)


add_library(example1 STATIC scheduler.cpp)

target_include_directories(example1 PUBLIC .)

target_link_libraries(example1 PUBLIC posix_runtime)
