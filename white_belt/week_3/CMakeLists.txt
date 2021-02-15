cmake_minimum_required(VERSION 3.2)

project(week_3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_alg_int_sort" alg_int_sort.cpp)
add_executable("${PROJECT_NAME}_alg_str_sort" alg_str_sort.cpp)
add_executable("${PROJECT_NAME}_class_sorted_strings" class_sorted_strings.cpp)
add_executable("${PROJECT_NAME}_class_person_1" class_person_1.cpp)
add_executable("${PROJECT_NAME}_class_person_2" class_person_2.cpp)
add_executable("${PROJECT_NAME}_class_reversible_string" class_reversible_string.cpp)
add_executable("${PROJECT_NAME}_class_person_3" class_person_3.cpp)
add_executable("${PROJECT_NAME}_class_incognizable" class_incognizable.cpp)
