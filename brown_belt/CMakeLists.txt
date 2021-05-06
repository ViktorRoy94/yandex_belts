cmake_minimum_required(VERSION 3.2)

project(brown_belt)

set(UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/utils")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

if ("${TARGET_WEEK}" MATCHES "all")
    add_subdirectory ("week_1")
    add_subdirectory ("week_2")
    add_subdirectory ("week_3")
#    add_subdirectory ("week_4")
#    add_subdirectory ("week_5")
#    add_subdirectory ("week_6")
else()
    add_subdirectory ("${TARGET_WEEK}")
endif()
