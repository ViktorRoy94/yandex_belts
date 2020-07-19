cmake_minimum_required(VERSION 2.8.12)

project(week_2)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_vector_monthly_tasks" vector_monthly_tasks.cpp)
add_executable("${PROJECT_NAME}_map_anagram" map_anagram.cpp)
add_executable("${PROJECT_NAME}_map_capitals" map_capitals.cpp)
add_executable("${PROJECT_NAME}_map_bus_stops_1" map_bus_stops_1.cpp)
add_executable("${PROJECT_NAME}_map_bus_stops_2" map_bus_stops_2.cpp)
add_executable("${PROJECT_NAME}_set_unique_strings" set_unique_strings.cpp)
add_executable("${PROJECT_NAME}_set_map_values" set_map_values.cpp)
add_executable("${PROJECT_NAME}_set_synonyms" set_synonyms.cpp)
add_executable("${PROJECT_NAME}_set_bus_stops_3" set_bus_stops_3.cpp)

file(COPY "tests" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
