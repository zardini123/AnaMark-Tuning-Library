cmake_minimum_required(VERSION 3.10)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12 CACHE STRING "Build for 10.12")

# Use C++14
set(CMAKE_CXX_STANDARD 14)
# Don't use e.g. GNU extension (like -std=gnu++11) for portability
set(CMAKE_CXX_EXTENSIONS false)

if(WIN32)
    macro(get_WIN32_WINNT version)
        if(CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
            string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
            # Check for Windows 10, b/c we'll need to convert to hex 'A'.
            if("${verMajor}" MATCHES "10")
                set(verMajor "A")
                string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
            endif()
            # Remove all remaining '.' characters.
            string(REPLACE "." "" ver ${ver})
            # Prepend each digit with a zero.
            string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
            set(${version} "0x${ver}")
        endif()
    endmacro()

    get_WIN32_WINNT(ver)
    # add_definitions(-D_WIN32_WINNT=${ver})

    message( STATUS "Windows version detected by CMake: ${ver}")
endif()

# Have all compiler warnings be considered an error
if (WIN32)
  # Windows version
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /Wall /WX")
else()
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif()

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
