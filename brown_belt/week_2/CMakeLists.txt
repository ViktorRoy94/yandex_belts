cmake_minimum_required(VERSION 3.2)

project(week_2)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_spendings_xml" spendings_xml.cpp ${UTILS_DIR}/xml.cpp)
target_include_directories("${PROJECT_NAME}_spendings_xml" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_spendings_json" spendings_json.cpp ${UTILS_DIR}/json.cpp)
target_include_directories("${PROJECT_NAME}_spendings_json" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_ini_lib" test_ini.cpp ini.cpp)
target_include_directories("${PROJECT_NAME}_ini_lib" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_refactoring" refactoring.cpp
                                             ${UTILS_DIR}/xml.cpp
                                             ${UTILS_DIR}/json.cpp)
target_include_directories("${PROJECT_NAME}_refactoring" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_stats_aggregator_refactoring"
                                            stats_aggregator_refactoring/stats_aggregator.cpp
                                            stats_aggregator_refactoring/stats_aggregator_test.cpp
                                            stats_aggregator_refactoring/main.cpp)
target_include_directories("${PROJECT_NAME}_stats_aggregator_refactoring" PRIVATE ${UTILS_DIR})


add_executable("${PROJECT_NAME}_collide"
                                double_dispatch/collide.cpp
                                double_dispatch/geo2d.cpp)
target_include_directories("${PROJECT_NAME}_collide" PRIVATE ${UTILS_DIR})


add_executable("${PROJECT_NAME}_comment_server" comment_server.cpp)
target_include_directories("${PROJECT_NAME}_comment_server" PRIVATE ${UTILS_DIR})