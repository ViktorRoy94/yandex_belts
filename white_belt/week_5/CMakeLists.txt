cmake_minimum_required(VERSION 2.8.12)

project(white_belt_week_5)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/week_5)

add_executable("week5_final_task" final_task.cpp)

file(COPY "tests" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
