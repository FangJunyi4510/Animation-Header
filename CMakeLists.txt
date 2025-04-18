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
add_compile_options(-std=c++17 -Wall -Wextra -Werror -fopenmp -g)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(WARNING "Pre Release flags:${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")
    message(WARNING "Cur Release flags:${CMAKE_CXX_FLAGS_RELEASE}")
endif()

add_subdirectory(./FFmpegEntity)

add_library(Animation ${SOURCES})
target_link_libraries(Animation PUBLIC FFmpegEntity)
target_link_libraries(Animation PUBLIC /lib/x86_64-linux-gnu/libgomp.so.1)
target_include_directories(Animation PUBLIC
    Animation
	${CMAKE_SOURCE_DIR}/../../include
)

add_executable(Test ./Test.cpp)
target_link_libraries(Test Animation)
