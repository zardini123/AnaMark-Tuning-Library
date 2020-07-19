#include "catch2.hpp"
#include "AnaMark-Tuning-Library.h"

TEST_CASE( "Error Class" )
{
    SECTION( "A Standalone Error is OK" )
    {
        TUN::CErr err;
        REQUIRE( err.IsOK() );
    }

    SECTION( "A Real Error is Not OK and Preserves Message" )
    {
        TUN::CErr err;
        err.SetError( "Something is Fishy in Denmark" );
        REQUIRE( !err.IsOK() );
        REQUIRE( err.GetLastError() == "Something is Fishy in Denmark" );
    }

    SECTION( "A Line Number comes through" )
    {
        // Note there's a feature in SetError for without space for large line numbers and we don't check bounds but it's probably OK
        TUN::CErr err;
        err.SetError( "Oops", 23 );
        REQUIRE( !err.IsOK() );
        REQUIRE( err.GetLastError() == "Line 23: Oops" );
    }
}
