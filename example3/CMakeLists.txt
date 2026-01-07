cmake_minimum_required(VERSION 3.20)


add_library(example3 STATIC scheduler.cpp)


target_include_directories(example3 PUBLIC .)

target_link_libraries(example3 PUBLIC posix_runtime)
