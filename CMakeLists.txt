cmake_minimum_required(VERSION 3.20.0)

project(MyProject LANGUAGES C CXX)

project(tutorial)

set(CMAKE_CXX_STANDARD 17)


add_executable(test main.cpp init.cpp)

add_subdirectory(posix_runtime)


# Uncomment the example you want to test
#add_subdirectory(example1) # Simple dataflow graph
add_subdirectory(example2) # Dataflow graph with one event node


target_link_libraries(test PUBLIC example)