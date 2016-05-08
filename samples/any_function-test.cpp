#include "../any_function.h"

#define CATCH_CONFIG_MAIN
#include "thirdparty/catch.hpp"

TEST_CASE( "any_function can default construct" ) 
{
    any_function f;
    REQUIRE( !f );
    REQUIRE( f.get_parameter_types().empty() );
    REQUIRE( f.get_return_type() == nullptr );
}

TEST_CASE( "any_function can constructed with nullptr" ) 
{
    any_function f {nullptr};
    REQUIRE( !f );
    REQUIRE( f.get_parameter_types().empty() );
    REQUIRE( f.get_return_type() == nullptr );
}