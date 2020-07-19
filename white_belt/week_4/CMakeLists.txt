cmake_minimum_required(VERSION 2.8.12)

project(week_4)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_class_lecture_title" class_lecture_title.cpp)
add_executable("${PROJECT_NAME}_class_invertible_function" class_invertible_function.cpp)
add_executable("${PROJECT_NAME}_fstream_read_1" fstream_read_1.cpp)
add_executable("${PROJECT_NAME}_fstream_read_2" fstream_read_2.cpp)
add_executable("${PROJECT_NAME}_fstream_read_print_precision" fstream_read_print_precision.cpp)
add_executable("${PROJECT_NAME}_fstream_read_print_table" fstream_read_print_table.cpp)
add_executable("${PROJECT_NAME}_fstream_students_list" fstream_students_list.cpp)
add_executable("${PROJECT_NAME}_class_rational" class_rational.cpp)
add_executable("${PROJECT_NAME}_exceptions_ensure_equal" exceptions_ensure_equal.cpp)
add_executable("${PROJECT_NAME}_exceptions_rational_calculator" exceptions_rational_calculator.cpp)
add_executable("${PROJECT_NAME}_exceptions_time_server" exceptions_time_server.cpp)

file(COPY "tests" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
