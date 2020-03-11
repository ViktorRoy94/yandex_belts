cmake_minimum_required(VERSION 2.8.12)

project(white_belt_week_3)

add_executable("alg_int_sort" alg_int_sort.cpp)
add_executable("alg_str_sort" alg_str_sort.cpp)
add_executable("class_sorted_strings" class_sorted_strings.cpp)
add_executable("class_person_1" class_person_1.cpp)
add_executable("class_person_2" class_person_2.cpp)
add_executable("class_reversible_string" class_reversible_string.cpp)
add_executable("class_person_3" class_person_3.cpp)
add_executable("class_incognizable" class_incognizable.cpp)

#file(COPY "tests" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
