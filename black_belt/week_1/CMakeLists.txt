cmake_minimum_required(VERSION 3.2)

project(week_1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_domains_san_fail" domains_san_fail.cpp)
target_include_directories("${PROJECT_NAME}_domains_san_fail" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_budget_san_fail" budget_san_fail.cpp)
target_include_directories("${PROJECT_NAME}_budget_san_fail" PRIVATE ${UTILS_DIR})
target_compile_options("${PROJECT_NAME}_budget_san_fail" PRIVATE -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC)

add_executable("${PROJECT_NAME}_transport_e_san_fail" transport_e_san_fail/main.cpp
                                                      transport_e_san_fail/descriptions.cpp
                                                      transport_e_san_fail/json.cpp
                                                      transport_e_san_fail/requests.cpp
                                                      transport_e_san_fail/sphere.cpp
                                                      transport_e_san_fail/transport_catalog.cpp
                                                      transport_e_san_fail/transport_router.cpp
                                                      transport_e_san_fail/utils.cpp)
target_include_directories("${PROJECT_NAME}_transport_e_san_fail" PRIVATE ${UTILS_DIR})
target_compile_options("${PROJECT_NAME}_transport_e_san_fail" PRIVATE "-Wno-unused-variable"
#                                                                      -D_GLIBCXX_DEBUG
#                                                                      -D_GLIBCXX_DEBUG_PEDANTIC
                                                                      )

add_executable("${PROJECT_NAME}_int64_t_overflow" int64_t_overflow.cpp)
target_include_directories("${PROJECT_NAME}_int64_t_overflow" PRIVATE ${UTILS_DIR})
target_compile_options("${PROJECT_NAME}_int64_t_overflow" PRIVATE -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC)

add_executable("${PROJECT_NAME}_transport_f_svg" transport_f/svg.cpp)
target_include_directories("${PROJECT_NAME}_transport_f_svg" PRIVATE ${UTILS_DIR})
target_compile_options("${PROJECT_NAME}_transport_f_svg" PRIVATE -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC)