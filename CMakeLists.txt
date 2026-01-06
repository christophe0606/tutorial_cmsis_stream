cmake_minimum_required(VERSION 3.20.0)

project(MyProject LANGUAGES C CXX)

project(tutorial)

set(CMAKE_CXX_STANDARD 17)


add_executable(test main.cpp init.cpp)

add_subdirectory(posix_runtime)


# Uncomment the example you want to test
add_subdirectory(example1) # Simple dataflow graph
#add_subdirectory(example2) # Dataflow graph with one event node
#add_subdirectory(example3) # Dataflow graph with one named event node
#add_subdirectory(example4) # Dataflow graph with one named event node and buffer exchange
#add_subdirectory(example5) # Dataflow graph with one named event node and buffer exchange with in-place processing

target_link_libraries(test PUBLIC example)