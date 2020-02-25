cmake_minimum_required(VERSION 2.8.12)

project(white_belt_week_2)

add_executable("vector_monthly_tasks" vector_monthly_tasks.cpp)
add_executable("map_anagram" map_anagram.cpp)
add_executable("map_capitals" map_capitals.cpp)

file(COPY "tests" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
