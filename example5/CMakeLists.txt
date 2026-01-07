cmake_minimum_required(VERSION 3.20)


add_library(example5 STATIC scheduler.cpp)


target_include_directories(example5 PUBLIC .)

target_link_libraries(example5 PUBLIC posix_runtime)
