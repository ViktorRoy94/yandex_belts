cmake_minimum_required(VERSION 2.8.12)

project(week_2)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_bus_decomposition" bus_stops_decomposition.cpp)
add_executable("${PROJECT_NAME}_test_equation" test_equation.cpp)
add_executable("${PROJECT_NAME}_test_person" test_person.cpp)
add_executable("${PROJECT_NAME}_test_rational" test_rational.cpp)
add_executable("${PROJECT_NAME}_test_palindrome" test_palindrome.cpp)

file(COPY "tests" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
