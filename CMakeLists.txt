cmake_minimum_required(VERSION 3.20.0)

project(MyProject LANGUAGES C CXX)

project(tutorial)

set(CMAKE_CXX_STANDARD 17)


# Initialization of CMSIS-Stream graphs (threads ...)
add_executable(test1 main.cpp init.cpp)
add_executable(test2 main.cpp init.cpp)
add_executable(test3 main.cpp init.cpp)
add_executable(test4 main.cpp init.cpp)
add_executable(test5 main.cpp init.cpp)


# OS independent CMSIS-Stream definitions (C++ templates)
add_subdirectory(stream)
# Posix runtime for CMSIS-Stream event system
add_subdirectory(posix_runtime)


add_subdirectory(example1) # Simple dataflow graph
add_subdirectory(example2) # Dataflow graph with one event node
add_subdirectory(example3) # Dataflow graph with event node and named event
add_subdirectory(example4) # Dataflow graph with event node and named event and buffer exchange
add_subdirectory(example5) # Dataflow graph with event node and named event and buffer exchange with in-place processing

# Link the example you want to run
target_link_libraries(test1 PUBLIC example1)
target_link_libraries(test2 PUBLIC example2)
target_link_libraries(test3 PUBLIC example3)
target_link_libraries(test4 PUBLIC example4)
target_link_libraries(test5 PUBLIC example5)