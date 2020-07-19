#include "catch2.hpp"
#include "AnaMark-Tuning-Library.h"

std::string testFile(std::string fn) {
    return std::string( "tests/data/" ) + fn;
}

TEST_CASE( "SCL KBM Basic Classes" )
{
    SECTION( "12-intune test" )
    {
        TUN::CSCL_Import scli;
        REQUIRE( scli.ReadSCL( testFile( "12-intune.scl" ).c_str() ) );
        REQUIRE( scli.Err().IsOK() );

        TUN::CSingleScale s;
        scli.SetSingleScale(s);

        REQUIRE( s.GetMIDINoteFreqHz(60) == Approx( 261.625565 ) );
        REQUIRE( s.GetMIDINoteFreqHz(69) == 440.0 );
    }
}
