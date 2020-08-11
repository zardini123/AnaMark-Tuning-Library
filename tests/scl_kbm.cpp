#include "catch2.hpp"
#include "AnaMark-Tuning-Library.h"
#include "testutils.h"

TEST_CASE( "Can Find Test Files" )
{
    SECTION( "Can fopen a test File" )
    {
        auto f = TestUtils::testFile( "12-intune.scl" );
        INFO( "Trying to open '" << f << "'. If this fails try setting ANAMARK_TEST_DATA_DIRECTORY" );
        auto p = fopen( f.c_str(), "r" );
        bool canOpen = p != nullptr;
        if( p )
            fclose(p);
        REQUIRE( canOpen );
    }
}

TEST_CASE( "SCL KBM Basic Classes" )
{
    SECTION( "12-intune test" )
    {
        TUN::CSCL_Import scli;
        REQUIRE( scli.ReadSCL( TestUtils::testFile( "12-intune.scl" ).c_str() ) );
        REQUIRE( scli.Err().IsOK() );

        TUN::CSingleScale s;
        scli.SetSingleScale(s);

        REQUIRE( s.GetMIDINoteFreqHz(60) == Approx( 261.625565 ) );
        REQUIRE( s.GetMIDINoteFreqHz(69) == 440.0 );
    }
}
