cmake_minimum_required(VERSION 3.2)

project(week_4)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_athlets" athlets.cpp)
add_executable("${PROJECT_NAME}_stack_vector" stack_vector.cpp)
add_executable("${PROJECT_NAME}_translator" translator.cpp)
add_executable("${PROJECT_NAME}_airport_counter" airport_counter.cpp)
target_compile_options("${PROJECT_NAME}_airport_counter" PRIVATE "-Wno-unused-variable" "-Wno-sign-compare")
add_executable("${PROJECT_NAME}_text_editor" text_editor.cpp)

add_executable("${PROJECT_NAME}_server_stats" server_stats/server_stats.cpp server_stats/stats.cpp)
target_include_directories("${PROJECT_NAME}_server_stats" PRIVATE .)