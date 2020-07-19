cmake_minimum_required(VERSION 2.8.12)

project(yandex_belts)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Require C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # enable warnings and treat warnings as errors
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-reorder")

    if ("${CMAKE_BUILD_TYPE}" MATCHES "(Debug|DEBUG|debug)")
        message(STATUS "  Setting Debug specific options...")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -ggdb")
        add_definitions(-DDEBUG)
    endif()
endif()

message(STATUS "  CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory ("white_belt")
add_subdirectory ("yellow_belt")
