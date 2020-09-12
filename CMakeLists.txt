cmake_minimum_required(VERSION 3.10)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12 CACHE STRING "Build for 10.12")

# Use C++14
set(CMAKE_CXX_STANDARD 14)
# Don't use e.g. GNU extension (like -std=gnu++11) for portability
set(CMAKE_CXX_EXTENSIONS false)

# Have all compiler warnings be considered an error
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")

project(Anamark-Tuning-Library VERSION 0.9.0.0 LANGUAGES CXX)
message( STATUS "Compiler Version is ${CMAKE_CXX_COMPILER_VERSION}" )

# Code Coverage Configuration
option(CODE_COVERAGE "Enable coverage reporting" OFF)
if(CODE_COVERAGE)
  include(cmake/CodeCoverage.cmake)
  append_coverage_compiler_flags()
  setup_target_for_coverage_lcov(NAME anamark-tuning-library_coverage EXECUTABLE anamark-tuning-test DEPENDENCIES anamark-tuning-test)
endif()

add_library( anamark-tuning-library STATIC )
target_sources( anamark-tuning-library PRIVATE
  src/SCL_Import.cpp
  src/TUN_EmbedHTML.cpp
  src/TUN_Scale.cpp
  src/TUN_StringTools.cpp )
target_include_directories( anamark-tuning-library PRIVATE src )

add_executable( anamark-tuning-test )
target_sources( anamark-tuning-test PRIVATE
  tests/main.cpp
  tests/testutils.cpp
  tests/basics.cpp
  tests/scl_kbm.cpp
  )
target_include_directories( anamark-tuning-test PRIVATE include libs/catch2 )
target_link_libraries( anamark-tuning-test PRIVATE anamark-tuning-library )

include( cmake/build-docs.cmake )
