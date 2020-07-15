cmake_minimum_required(VERSION 3.10)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12 CACHE STRING "Build for 10.12")

set(CMAKE_CXX_EXTENSIONS false)
set(CMAKE_CXX_STANDARD 14)

project(Anamark-Tuning-Library VERSION 0.9.0.0 LANGUAGES CXX)
message( STATUS "Compiler Version is ${CMAKE_CXX_COMPILER_VERSION}" )

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
  tests/basics.cpp
  tests/scl_kbm.cpp
  )
target_include_directories( anamark-tuning-test PRIVATE include libs/catch2 )
target_link_libraries( anamark-tuning-test PRIVATE anamark-tuning-library )

include( cmake/build-docs.cmake )
