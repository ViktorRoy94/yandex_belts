cmake_minimum_required(VERSION 3.2)

project(week_6)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_final_task"
    main.cpp
    date.cpp
    date_test.cpp
    database.cpp
    database_test.cpp
    node.cpp
    node_test.cpp
    token.cpp
    condition_parser.cpp
    condition_parser_test.cpp
)

