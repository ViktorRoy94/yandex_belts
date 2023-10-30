cmake_minimum_required(VERSION 3.2)

project(week_4)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_print_vector_part" print_vector_part.cpp)
add_executable("${PROJECT_NAME}_find_greater_elements" find_greater_elements.cpp)
add_executable("${PROJECT_NAME}_split_into_words" split_into_words.cpp)

add_executable("${PROJECT_NAME}_remove_duplicates" remove_duplicates.cpp)
add_executable("${PROJECT_NAME}_print_permutations" print_permutations.cpp)
add_executable("${PROJECT_NAME}_print_stats" print_stats.cpp)
add_executable("${PROJECT_NAME}_merge_sort_div2" merge_sort_div2.cpp)
add_executable("${PROJECT_NAME}_merge_sort_div3" merge_sort_div3.cpp)

add_executable("${PROJECT_NAME}_find_nearest_element" find_nearest_element.cpp)
add_executable("${PROJECT_NAME}_person_names" person_names.cpp)
add_executable("${PROJECT_NAME}_find_starts_with_char" find_starts_with_char.cpp)
add_executable("${PROJECT_NAME}_find_starts_with_string" find_starts_with_string.cpp)
add_executable("${PROJECT_NAME}_build_expression" build_expression.cpp)
add_executable("${PROJECT_NAME}_build_expression_without_redundant_brackets" build_expression_without_redundant_brackets.cpp)
