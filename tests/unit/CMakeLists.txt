# CMake Catch2 Unit test executable
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

# Unit test dependencies
find_package(Catch2 REQUIRED NO_MODULE)

# Unit test executable
add_executable(gdscpp_unit)

 # Unit test executable sources
target_sources(gdscpp_unit PRIVATE
  example_unit_test.cpp
  catch_main.cpp
)

# Link unit test dependencies
target_link_libraries(gdscpp_unit PRIVATE
  gdscpp
  Catch2::Catch2
)

# Add all test cases as individual tests
include(Catch)
catch_discover_tests(
  gdscpp_unit
  WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
  TEST_PREFIX gdscpp::unit::
)