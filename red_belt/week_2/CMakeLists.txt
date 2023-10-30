cmake_minimum_required(VERSION 3.2)

project(week_2)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_sort_students" sort_students.cpp)
add_executable("${PROJECT_NAME}_learner" learner.cpp)

add_executable("${PROJECT_NAME}_expresses" expresses.cpp)
add_executable("${PROJECT_NAME}_ebook" ebook.cpp)
add_executable("${PROJECT_NAME}_booking" booking.cpp)
