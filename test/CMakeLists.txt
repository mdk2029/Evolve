cmake_minimum_required(VERSION 3.11)
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_executable(evolve_test "")
set_target_properties(evolve_test PROPERTIES LINKER_LANGUAGE CXX)
target_sources(evolve_test PUBLIC
  "${TEST_SOURCES}"
  ${CMAKE_CURRENT_LIST_DIR}/../random.cpp
    )

target_include_directories(evolve_test
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
    )
