cmake_minimum_required(VERSION 3.2)

project(week_6)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_transport_a_b" transport_a_b/main.cpp
                                               transport_a_b/request.cpp
                                               transport_a_b/response.cpp
                                               transport_a_b/read_utils.cpp
                                               transport_a_b/manager.cpp
                                               transport_a_b/data_types.cpp)
target_include_directories("${PROJECT_NAME}_transport_a_b" PRIVATE transport_a_b
                                                                   ${UTILS_DIR})

add_executable("${PROJECT_NAME}_transport_c" transport_c/main.cpp
                                               transport_c/request.cpp
                                               transport_c/response.cpp
                                               transport_c/read_utils.cpp
                                               transport_c/manager.cpp
                                               transport_c/data_types.cpp)
target_include_directories("${PROJECT_NAME}_transport_c" PRIVATE transport_c
                                                                 ${UTILS_DIR})

add_executable("${PROJECT_NAME}_transport_d" transport_d/main.cpp
                                             transport_d/request.cpp
                                             transport_d/response.cpp
                                             transport_d/read_utils.cpp
                                             transport_d/manager.cpp
                                             transport_d/data_types.cpp
                                             transport_d/json.cpp)
target_include_directories("${PROJECT_NAME}_transport_d" PRIVATE transport_d
                                                                 ${UTILS_DIR})

add_executable("${PROJECT_NAME}_transport_e" transport_e/main.cpp
                                             transport_e/request.cpp
                                             transport_e/response.cpp
                                             transport_e/read_utils.cpp
                                             transport_e/manager.cpp
                                             transport_e/data_types.cpp
                                             transport_e/json.cpp)
target_include_directories("${PROJECT_NAME}_transport_e" PRIVATE transport_e
                                                                 ${UTILS_DIR})

file(COPY "transport_e/tests" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})