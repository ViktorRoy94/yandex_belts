cmake_minimum_required(VERSION 3.2)

project(yandex_belts)

################################################################################
# Input arguments
################################################################################

set(TARGET_COURSE "brown" CACHE STRING "Сhoose course color: white, yellow, red, brown, black or all")
set_property(CACHE TARGET_COURSE PROPERTY STRINGS "white" "yellow" "red" "brown" "black" "all")
set(TARGET_WEEK "week_1" CACHE STRING "Сhoose current week number: week_1, week_2 etc. or all")
set_property(CACHE TARGET_WEEK PROPERTY STRINGS "week_1" "week_2" "week_3"
                                                "week_4" "week_5" "week_6" "all")

message(STATUS "*** Input arguments ***")
message(STATUS "  TARGET_COURSE: ${TARGET_COURSE}")
message(STATUS "  TARGET_WEEK: ${TARGET_WEEK}")

################################################################################
# Checking input arguments
################################################################################

if (NOT "${TARGET_COURSE}" MATCHES "(white|yellow|red|brown|black|all)")
    message(FATAL_ERROR "TARGET_COURSE should be white, yellow, red, brown, black or all!")
endif()

################################################################################
# Setting Build Options
################################################################################

message(STATUS "*** Build options ***")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-reorder")
endif()
message(STATUS "  CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

################################################################################
# Setting global definitions
################################################################################

add_definitions(-DLOCAL_BUILD)

################################################################################
# Add Projects
################################################################################

if ("${TARGET_COURSE}" MATCHES "all")
    add_subdirectory ("white_belt")
    add_subdirectory ("yellow_belt")
    add_subdirectory ("red_belt")
    add_subdirectory ("brown_belt")
    add_subdirectory ("black_belt")
else()
    add_subdirectory ("${TARGET_COURSE}_belt")
endif()
