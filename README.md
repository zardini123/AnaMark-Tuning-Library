[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# AnaMark C++ Tuning Library

Originally developed and maintained by Mark Henning in 2009, this C++ library is for interpreting and using AnaMark Tuning files (_.tun_ V0.0 - V2.0), multiple scales files (_.msf_), and Scala files (_.scl_ and _.kbm_). These files allow the definition of non-standard scales used for microtuning in music such as in a lot of eastern music styles. Mark Henning's original source code for this library was provided as a _.zip_ file on his website, under the section [AnaMark tuning file format specifications](https://www.mark-henning.de/am_downloads_eng.php#Tuning).

## Features

This library (should) be fully compliant with the [AnaMark tuning file format V2.00](https://www.mark-henning.de/files/am/Tuning_File_V2_Doc.pdf) (Also see section **Goals**).

The following file types are known to be _readable_ with this library:

- AnaMark tuning (_.tun_)
- AnaMark multiple scales tuning (_.msf_)
- Scala Files (_.scl_ and _.kbm_)

The following file types are known to be _writable_ with this library:

- AnaMark tuning (_.tun_)
- AnaMark multiple scales tuning (_.msf_)

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
