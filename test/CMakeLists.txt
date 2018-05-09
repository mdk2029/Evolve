
# Download and unpack googletest at configure time

if(NOT GTEST_FOUND)
  find_package(gtest REQUIRED HINTS ${CMAKE_CURRENT_LIST_DIR}/../cmake)
endif()

file(GLOB TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_executable(evolve_test "")
set_target_properties(evolve_test PROPERTIES LINKER_LANGUAGE CXX)
target_sources(evolve_test PUBLIC
    "${TEST_SOURCES}"
    )
target_link_libraries(evolve_test gmock_main )
target_include_directories(evolve_test
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
    )
