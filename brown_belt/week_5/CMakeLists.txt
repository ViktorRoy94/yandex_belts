cmake_minimum_required(VERSION 3.2)

project(week_5)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_print_stats_testing" print_stats_testing/print_stats_testing.cpp)
target_include_directories("${PROJECT_NAME}_print_stats_testing" PRIVATE ${UTILS_DIR}
                                                                         print_stats_testing)

add_executable("${PROJECT_NAME}_domains" domains.cpp)
target_include_directories("${PROJECT_NAME}_domains" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_test_domains" test_domains.cpp)
target_include_directories("${PROJECT_NAME}_test_domains" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_budget_home" budget_home.cpp)
target_include_directories("${PROJECT_NAME}_budget_home" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_budget_professional" budget_professional.cpp)
target_include_directories("${PROJECT_NAME}_budget_professional" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_budget_professional_mobile" budget_professional_mobile.cpp)
target_include_directories("${PROJECT_NAME}_budget_professional_mobile" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_budget_professional_mobile_legacy" budget_professional_mobile_legacy.cpp)
target_include_directories("${PROJECT_NAME}_budget_professional_mobile_legacy" PRIVATE ${UTILS_DIR})
