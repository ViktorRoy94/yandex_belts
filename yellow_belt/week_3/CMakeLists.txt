cmake_minimum_required(VERSION 3.2)

project(week_3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_sum_reverse_sort" sum_reverse_sort.cpp)
add_executable("${PROJECT_NAME}_phone_number" phone_number.cpp)

add_subdirectory(bus_stops_decomposition)
