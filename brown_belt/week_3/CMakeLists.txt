cmake_minimum_required(VERSION 3.2)

project(week_3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_print_stats_2" print_stats_2.cpp)
target_include_directories("${PROJECT_NAME}_print_stats_2" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_lazy_value" lazy_value.cpp)
target_include_directories("${PROJECT_NAME}_lazy_value" PRIVATE ${UTILS_DIR})

find_package(Threads REQUIRED)
add_executable("${PROJECT_NAME}_synchronized_2" synchronized_2.cpp)
target_include_directories("${PROJECT_NAME}_synchronized_2" PRIVATE ${UTILS_DIR})
target_link_libraries("${PROJECT_NAME}_synchronized_2" PRIVATE Threads::Threads)

add_executable("${PROJECT_NAME}_concurrent_map_2" concurrent_map_2.cpp)
target_include_directories("${PROJECT_NAME}_concurrent_map_2" PRIVATE ${UTILS_DIR})
target_link_libraries("${PROJECT_NAME}_concurrent_map_2" PRIVATE Threads::Threads)

add_executable("${PROJECT_NAME}_polynomial" polynomial.cpp)
target_include_directories("${PROJECT_NAME}_polynomial" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_zoo" zoo.cpp)
target_include_directories("${PROJECT_NAME}_zoo" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_pipeline" pipeline.cpp)
target_include_directories("${PROJECT_NAME}_pipeline" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_expression" expression.cpp)
target_include_directories("${PROJECT_NAME}_expression" PRIVATE ${UTILS_DIR})