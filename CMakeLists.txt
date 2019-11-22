cmake_minimum_required(VERSION 2.6)

project("cpptalksindex")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake-modules")
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib/)

add_subdirectory(deps)

set(WARNING_FLAGS
  -Wall
  -Wextra
  -Wshadow
  -Wnon-virtual-dtor
  -pedantic
  -Wold-style-cast
  -Wcast-align
  -Wunused
  -Woverloaded-virtual
  -Wpedantic
  -Wconversion
  -Wsign-conversion
  -Wdouble-promotion
  -Wformat=2
)

option(BUILD_TESTS "Build the tests" ON)

add_executable(cpptalksindex
  src/main.cpp
  src/TalksDB.cpp
  src/TalksLoader.cpp
)
target_compile_options(cpptalksindex PRIVATE ${WARNING_FLAGS})
target_link_libraries(cpptalksindex json expected range-v3)
target_include_directories(cpptalksindex PUBLIC include)
set_property(TARGET cpptalksindex PROPERTY CXX_STANDARD 17)
set_property(TARGET cpptalksindex PROPERTY CXX_STANDARD_REQUIRED ON)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
