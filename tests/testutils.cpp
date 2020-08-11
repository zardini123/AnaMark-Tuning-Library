#include "testutils.h"
#include <cstdlib>

std::string TestUtils::testFile(std::string fn) {
    const char* dir = getenv( "ANAMARK_TEST_DATA_DIRECTORY" );
    std::string td;
    if (dir == nullptr)
        td = "tests/data/";
    else
        td = std::string( dir );


    return std::string( td ) + fn;
}
