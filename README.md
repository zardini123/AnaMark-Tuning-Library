[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# AnaMark C++ Tuning Library

Originally developed and maintained by Mark Henning in 2009, this C++ library is for interpreting and using AnaMark Tuning files (_.tun_ V0.0 - V2.0), multiple scales files (_.msf_), and Scala files (_.scl_ and _.kbm_). These files allow the definition of non-standard scales used for microtuning in music such as in a lot of eastern music styles. Mark Henning's original source code for this library was provided as a _.zip_ file on his website, under the section [AnaMark tuning file format specifications](https://www.mark-henning.de/am_downloads_eng.php#Tuning).

## Features

This library (should) be fully compliant with the [AnaMark tuning file format V2.00](https://www.mark-henning.de/files/am/Tuning_File_V2_Doc.pdf) 
(Also see section **Goals**).

The following file types are known to be _readable_ with this library:

- AnaMark tuning (_.tun_)
- AnaMark multiple scales tuning (_.msf_)
- Scala Files (_.scl_ and _.kbm_)

The following file types are known to be _writable_ with this library:

- AnaMark tuning (_.tun_)
- AnaMark multiple scales tuning (_.msf_)
- HTML (for scale distribution and sharing)

## License (X11 license type; also known as MIT license)

Full license is provided in file [LICENSE.md](LICENSE.md).

## Using the library in your code

There are a variety of ways to use the library in your code. The two most common are

1. Build the static library in a standalone fashion and set your include path to
   the source directory
2. Make this module a git submodule of your project and build and include the appropriate
   source.

This section will be expanded in the near future.

## Goals

The goal of this repository is to continue to maintain Mark Henning's original library source, and ensure its compatibility with modern compilers and systems. Changes made to the source must still result in being fully compliant with Mark's AnaMark tuning file format specifications.
Along with that main mission, this repository also aims to

- Fully test (have full code coverage)
- Migrate original code to use standard library (std) calls that are platform independent
- Provide full documentation for future users


## Building _(library standalone assets, test executable, and documentation)_

### Preparing for building

The AnaMark Tuning Library uses CMake to configure and build the solution.
CMake is a widely available open source build configuraiton tool which can 
create build assets for the development environment of your choice. You can
install it from [cmake.org](https://cmake.org); 

_Installing cmake:_

**Windows:**
**macOS:**  `brew install cmake`
**Linux:**

### Preparing for building documentation

To build the documentation, ensure _doxygen_ is installed on your system before running the steps for building the library standalone assets.

_Installing doxygen:_

**Windows:**
**macOS:**  `brew install doxygen`  (`brew` is a macOS package manager, download [here](https://brew.sh))
**Linux:**

### Platform-agnostic building

The following steps can be performed easily on Windows, macOS, and Linux once _cmake_ is installed (and if you want documentation, _doxygen_ too).

0) Clone / download this repository
1) Set your current directory to the root folder of this library
2) Run the build commands

```
cmake -B build
cmake --build build --config Release
```

### Results of build

If _doxygen_ was properly installed on your system, you can access the documentation by opening file `build/docs/html/index.html` in your favorite browser.

### Platform-specific building

If you want more fine control with building this library, follow these steps on your platform.

**Windows:**

Open a Visual Studio command prompt. Check out this code and change to the
directory. Type

```
cmake -B build
```

and you will have in your build directory a visual studio solutions file which
will allow you to create the library or test targets or you can build on the 
command line with

```
cmake --build build --config Release
```

**macOS:**

Run CMake to create xcode assets. For example

```
cmake -B build -G Xcode
```

You will then have an xcode file in the 'build' directory which you can
open and use with xcode.  

If you choose, you can build in xcode or from the command line with

```
cmake --build build --config Release 
```

Command line builds have far more readable output if you install
and use the optional '[xcpretty](https://github.com/xcpretty/xcpretty)' utility.

```
cmake --build build --config Release | xcpretty
```

**Linux:**

Identical to steps provided in section [Platform-agnostic building](#platform-agnostic-building).
