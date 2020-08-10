#include "testutils.h"
#include <cstdlib>

std::string TestUtils::testFile(std::string fn) {
    const char* dir = getenv( "ANAMARK_TUNING_TEST_DIRECTORY" );
    std::string td;
    if( ! dir )
        td = "tests/data/";
    else
        td = std::string( dir );

            
    return std::string( td ) + fn;
}

