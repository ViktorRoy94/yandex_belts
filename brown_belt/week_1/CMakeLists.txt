cmake_minimum_required(VERSION 3.2)

project(week_1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_hash_set" hash_set.cpp)
target_include_directories("${PROJECT_NAME}_hash_set" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_set_iterator_next" set_iterator_next.cpp)
target_include_directories("${PROJECT_NAME}_set_iterator_next" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_hash_point" hash_point.cpp)
target_include_directories("${PROJECT_NAME}_hash_point" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_hash_person" hash_person.cpp)
target_include_directories("${PROJECT_NAME}_hash_person" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_secondary_index" secondary_index.cpp)
target_include_directories("${PROJECT_NAME}_secondary_index" PRIVATE ${UTILS_DIR})
