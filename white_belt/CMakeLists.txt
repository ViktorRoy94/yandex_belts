cmake_minimum_required(VERSION 2.8.12)

project(white_belt)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

if ("${TARGET_WEEK}" MATCHES "all")
    add_subdirectory ("week_2")
    add_subdirectory ("week_3")
    add_subdirectory ("week_4")
    add_subdirectory ("week_5")
elseif(NOT "${TARGET_WEEK}" MATCHES "week_1")
    add_subdirectory ("${TARGET_WEEK}")
endif()
