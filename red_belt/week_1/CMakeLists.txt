cmake_minimum_required(VERSION 3.2)

project(week_1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_max_element_if" max_element_if.cpp)
add_executable("${PROJECT_NAME}_log" log.cpp)
add_executable("${PROJECT_NAME}_sort_by" sort_by.cpp)
add_executable("${PROJECT_NAME}_update_field" update_field.cpp)
add_executable("${PROJECT_NAME}_print_values" print_values.cpp)
add_executable("${PROJECT_NAME}_uniq_id" uniq_id.cpp)
target_compile_options("${PROJECT_NAME}_uniq_id" PRIVATE "-Wno-unused-variable")

add_executable("${PROJECT_NAME}_table" table.cpp)
add_executable("${PROJECT_NAME}_deque" deque.cpp)
add_executable("${PROJECT_NAME}_paginator" paginator.cpp)
