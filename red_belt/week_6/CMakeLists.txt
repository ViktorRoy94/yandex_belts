cmake_minimum_required(VERSION 3.2)

project(week_6)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

find_package(Threads REQUIRED)
add_executable("${PROJECT_NAME}_final_task"
               main.cpp
               parse.cpp
               search_server.cpp)
target_link_libraries("${PROJECT_NAME}_final_task" PRIVATE Threads::Threads)
target_compile_options("${PROJECT_NAME}_final_task" PRIVATE "-Wno-unused-variable")
