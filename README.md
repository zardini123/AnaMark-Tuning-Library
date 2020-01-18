# AnaMark C++ Tuning Library
Originally developed and maintained by Mark Henning in 2009, this C++ library is for interpreting and using AnaMark Tuning files (*.tun* V0.0 - V2.0), *.msf*, and Scala files (*.scl* and *.kbm*).  Mark Henning's original source code for this library was provided as a *.zip* file on his website, under the section [AnaMark tuning file format specifications](https://www.mark-henning.de/am_downloads_eng.php#Tuning).

## Features
This library (should) be fully compilant with the [AnaMark tuning file format V2.00](https://www.mark-henning.de/files/am/Tuning_File_V2_Doc.pdf) (Also see section **Goals**).

The following file types are known to be *readable* with this library:
- AnaMark tuning (*.tun*)
- AnaMark mutli-scale tuning (*.msf*)
- Scala Files (*.scl* and *.kbm*)

The following file types are known to be *writable* with this library:
- AnaMark tuning (*.tun*)
- AnaMark mutli-scale tuning (*.msf*)

## Copyright
Mark Henning's copyright statement in the original source code Readme.txt is as follows:  

"You may use [the source code] as-is or in any modified form in your own software (non-commercial or commercial) as long as you mention the above copyright somewhere in
- the software
- and the documentation
- and the tuning files written using it. (as comment line)

## Goals
The goal of this repository is to continue to maintian Mark Henning's original library source, and ensure its compatibility with modern compilers and systems.  Changes made to the source must still result in being fully compliant with Mark's AnaMark tuning file format specifications.
Along with that main mission, this repository also aims to
- Fully test (have full code coverage)
- Migrate original code to use standard library (std) calls that are platform independent
- Provide full documentation for future users
