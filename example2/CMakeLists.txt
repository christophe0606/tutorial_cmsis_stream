cmake_minimum_required(VERSION 3.20)


add_library(example2 STATIC scheduler.cpp)


target_include_directories(example2 PUBLIC .)

target_link_libraries(example2 PUBLIC posix_runtime)
