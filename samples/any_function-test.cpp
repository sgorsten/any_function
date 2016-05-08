#include "../any_function.h"

#define CATCH_CONFIG_MAIN
#include "thirdparty/catch.hpp"

TEST_CASE( "any_function can default construct" ) 
{
    const any_function f;
    REQUIRE( !f );
    REQUIRE( f.get_parameter_types().empty() );
    REQUIRE( f.get_return_type() == nullptr );
}

TEST_CASE( "any_function can constructed with nullptr" ) 
{
    const any_function f {nullptr};
    REQUIRE( !f );
    REQUIRE( f.get_parameter_types().empty() );
    REQUIRE( f.get_return_type() == nullptr );
}

double global_function(int a, double b, float c) { return a*b+c; }
TEST_CASE( "any_function can be constructed with a global function" )
{
    const any_function f {&global_function};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 3 );
    REQUIRE( f.get_parameter_types()[0] == &typeid(int) );
    REQUIRE( f.get_parameter_types()[1] == &typeid(double) );
    REQUIRE( f.get_parameter_types()[2] == &typeid(float) );
    REQUIRE( f.get_return_type() == &typeid(double) );
}

TEST_CASE( "any_function is callable with a global function" )
{
    const any_function f {&global_function};
    int a = 5; double b = 12.2; float c = 3.14f;
    auto r = f.invoke({&a,&b,&c});
    REQUIRE( r != nullptr );
    REQUIRE( *reinterpret_cast<double *>(r.get()) == a*b+c );
}