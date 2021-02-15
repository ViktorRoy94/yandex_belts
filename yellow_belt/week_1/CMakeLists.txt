cmake_minimum_required(VERSION 3.2)

project(week_1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_matrix_sum" matrix_sum.cpp)
add_executable("${PROJECT_NAME}_average_temperature" average_temperature.cpp)
add_executable("${PROJECT_NAME}_block_mass" block_mass.cpp)
add_executable("${PROJECT_NAME}_region_base" region_base.cpp)
add_executable("${PROJECT_NAME}_team_tasks" team_tasks.cpp)
add_executable("${PROJECT_NAME}_templates_square" templates_square.cpp)
add_executable("${PROJECT_NAME}_templates_element_reference" templates_element_reference.cpp)

file(COPY "tests" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
