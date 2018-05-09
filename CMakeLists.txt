cmake_minimum_required(VERSION 3.1)

##############################################################
# Some common stuff
##############################################################
option(USE_GCC "Use gcc instead of clang" OFF)

if(USE_GCC)
    set(CMAKE_CXX_COMPILER g++ CACHE STRING "CXX Compiler")
    set(CMAKE_C_COMPILER gcc CACHE STRING "C Compiler")
else()
    set(CMAKE_CXX_COMPILER clang++ CACHE STRING "CXX Compiler")
    set(CMAKE_C_COMPILER clang CACHE STRING "C Compiler")
endif()

set(CMAKE_CXX_STANDARD 14 CACHE STRING "CXX Standard")
set(CMAKE_VERBOSE_MAKEFILE true CACHE BOOL "verbose make output")
set(THREADS_PREFER_PTHREAD_FLAG ON CACHE STRING "use pthread")

##############################################################
# Define the project
##############################################################

project(evolve LANGUAGES CXX)

# Use -O3 instead of -O2
string(REPLACE "-O2" "-O3" newFlags ${CMAKE_CXX_FLAGS_RELEASE})
set(CMAKE_CXX_FLAGS_RELEASE "${newFlags}")

add_executable(${PROJECT_NAME} "")
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

FILE(GLOB EVOLVE_SRC "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
)

target_sources(${PROJECT_NAME}
        PUBLIC
        "${EVOLVE_SRC}"
)

find_package(gtest REQUIRED HINTS ${CMAKE_CURRENT_LIST_DIR}/cmake)
find_package(Boost REQUIRED)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

##############################################################
# Add tests
##############################################################

message("Adding test dir")
add_subdirectory("test")
