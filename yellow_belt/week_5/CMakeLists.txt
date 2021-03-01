cmake_minimum_required(VERSION 3.2)

project(week_5)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_inheritance" inheritance.cpp)
add_executable("${PROJECT_NAME}_notifiers" notifiers.cpp)
add_executable("${PROJECT_NAME}_figures" figures.cpp)
add_executable("${PROJECT_NAME}_refactoring" refactoring.cpp)
