cmake_minimum_required(VERSION 3.20)


add_library(example4 STATIC scheduler.cpp)


target_include_directories(example4 PUBLIC .)

target_link_libraries(example4 PUBLIC posix_runtime)
