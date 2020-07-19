#define CATCH_CONFIG_RUNNER
#include "catch2.hpp"

/*
** Main from the tuning library so we can test locales
*/

int main(int argc, char **argv)
{
    if( getenv( "LANG" ) != nullptr )
    {
        try {
            std::locale::global( std::locale( getenv( "LANG" ) ) );
            std::cout << "Setting LOCALE to '" << getenv( "LANG" ) << "'. ";

            time_t rawtime;
            struct tm * timeinfo;
            char buffer[512];

            time (&rawtime);
            timeinfo = localtime(&rawtime);

            // work around a windows g++ warning error
            const char* fmt = "%A %e %B %Y";
            strftime(buffer, sizeof(buffer), fmt ,timeinfo);
    
            std::cout << "Date in this locale: '" << buffer << "'" << std::endl;;
        } catch( std::exception &e ) {
            std::locale::global( std::locale ("C") );
        }

    }
    int result = Catch::Session().run( argc, argv );
    return result;
}


