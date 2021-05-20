cmake_minimum_required(VERSION 3.2)

project(week_4)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

add_executable("${PROJECT_NAME}_textures" textures/main.cpp
                                          textures/Solution.cpp
                                          textures/Textures.cpp)
target_include_directories("${PROJECT_NAME}_textures" PRIVATE textures
                                                              ${UTILS_DIR})
target_compile_options("${PROJECT_NAME}_textures" PRIVATE "-Wno-unused-variable")

find_package(Threads REQUIRED)
add_executable("${PROJECT_NAME}_cache" cache/main.cpp
                                       cache/Solution.cpp)
target_include_directories("${PROJECT_NAME}_cache" PRIVATE cache
                                                           ${UTILS_DIR})
target_link_libraries("${PROJECT_NAME}_cache" PRIVATE Threads::Threads)
target_compile_options("${PROJECT_NAME}_cache" PRIVATE "-Wno-unused-variable")

add_executable("${PROJECT_NAME}_unique_ptr" unique_ptr.cpp)
target_include_directories("${PROJECT_NAME}_unique_ptr" PRIVATE ${UTILS_DIR})

add_executable("${PROJECT_NAME}_raii_booking" raii_booking/test.cpp)
target_include_directories("${PROJECT_NAME}_raii_booking" PRIVATE raii_booking
                                                              ${UTILS_DIR})
target_compile_options("${PROJECT_NAME}_raii_booking" PRIVATE "-Wno-unused-variable")
