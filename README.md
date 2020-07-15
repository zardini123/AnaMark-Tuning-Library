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

## Building the Library and Tests as Standalone Assets

The AnaMark Tuning Library uses CMake to configure and build the solution.
CMake is a widely available open source build configuraiton tool which can 
create build assets for the development environment of your choice. You can
install it from [cmake.org](https://cmake.org); if you install VS2017 or 2019
you already have it; you can easily install it with homebrew on mac or apt on
linux. 

But once it is installed the recipe is simple. Run cmake to make your build
assets, and either then open those build assets or run cmake to run a build.

### Windows

Open a Visual Studio command prompt. Check out this code and change to the
directory. Type

```
cmake -Bbuild
```

and you will have in your build directory a visual studio solutions file which
will allow you to create the library or test targets or you can build on the 
command line with

```
cmake --build build --config Release
```

### macOS

Run CMake to create xcode assets. For example

```
cmake -Bbuild -GXcode
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

### Linux

Very similar.

```
cmake -Bbuild
cmake --build build --config Release
```




## Using the library in your code

There are a variety of ways to use the library in your code. The two most common are

1. Build the static library in a standalone fashion and set your include path to
   the source directory
2. Make this module a git submodule of your project and build and include the appropriate
   source.

This section will be expanded in the near future.

## Building the documentation



## License (X11 license type; also known as MIT license)

Copyright (C) 2009 Mark Henning, Germany, http://www.mark-henning.de

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Goals

The goal of this repository is to continue to maintain Mark Henning's original library source, and ensure its compatibility with modern compilers and systems. Changes made to the source must still result in being fully compliant with Mark's AnaMark tuning file format specifications.
Along with that main mission, this repository also aims to

- Fully test (have full code coverage)
- Migrate original code to use standard library (std) calls that are platform independent
- Provide full documentation for future users
