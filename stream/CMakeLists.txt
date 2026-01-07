cmake_minimum_required(VERSION 3.20)

add_library(stream INTERFACE)

target_include_directories(stream INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})