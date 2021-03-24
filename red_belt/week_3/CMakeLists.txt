cmake_minimum_required(VERSION 3.2)

project(week_3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_object_pool" object_pool.cpp)
add_executable("${PROJECT_NAME}_swap_sort_copy" swap_sort_copy.cpp)
add_executable("${PROJECT_NAME}_simple_vector" simple_vector.cpp)
add_executable("${PROJECT_NAME}_linked_list" linked_list.cpp)
