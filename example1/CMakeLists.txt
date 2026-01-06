cmake_minimum_required(VERSION 3.20)


add_library(example STATIC scheduler.cpp)

target_include_directories(example PUBLIC .)

target_link_libraries(example PUBLIC posix_runtime)
