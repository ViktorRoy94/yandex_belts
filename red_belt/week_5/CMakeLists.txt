cmake_minimum_required(VERSION 3.2)

project(week_5)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_josephus_permutation" josephus_permutation.cpp)
add_executable("${PROJECT_NAME}_group_heavy_strings" group_heavy_strings.cpp)
add_executable("${PROJECT_NAME}_simple_vector_copy_assignment" simple_vector_copy_assignment.cpp)
add_executable("${PROJECT_NAME}_split_into_sentences" split_into_sentences.cpp)
add_executable("${PROJECT_NAME}_simple_vector_2" simple_vector_2.cpp)
add_executable("${PROJECT_NAME}_merge_sort_3" merge_sort_3.cpp)
add_executable("${PROJECT_NAME}_priority_collection_2" priority_collection_2.cpp)
target_compile_options("${PROJECT_NAME}_priority_collection_2" PRIVATE "-Wno-unused-variable" "-Wno-unused-but-set-variable")

find_package(Threads REQUIRED)
add_executable("${PROJECT_NAME}_matrix_sum" matrix_sum.cpp)
target_link_libraries("${PROJECT_NAME}_matrix_sum" PRIVATE Threads::Threads)

add_executable("${PROJECT_NAME}_explore_key_words" explore_key_words.cpp)
target_link_libraries("${PROJECT_NAME}_explore_key_words" PRIVATE Threads::Threads)

add_executable("${PROJECT_NAME}_synchronized" synchronized.cpp)
target_link_libraries("${PROJECT_NAME}_synchronized" PRIVATE Threads::Threads)

add_executable("${PROJECT_NAME}_concurrent_map" concurrent_map.cpp)
target_link_libraries("${PROJECT_NAME}_concurrent_map" PRIVATE Threads::Threads)
