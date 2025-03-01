cmake_minimum_required(VERSION 3.10)

project(AnimationHeader)

if(NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "CMAKE_BUILD_TYPE has not been specialized! ")
else()
    message(STATUS "Current CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
endif()

set(SOURCES
    Animation/Animation.cpp
    Animation/Basic.cpp
)
add_compile_options(-std=c++17 -Wall -Wextra -Werror)

add_subdirectory(./FFmpegEntity)

add_library(Animation ${SOURCES})
target_link_libraries(Animation FFmpegEntity)
target_include_directories(Animation PUBLIC Animation)

add_executable(Test ./Test.cpp)
target_link_libraries(Test Animation)
