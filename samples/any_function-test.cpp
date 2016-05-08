#include "../any_function.h"

#define CATCH_CONFIG_MAIN
#include "thirdparty/catch.hpp"

template<class T> void test_capture_type(const std::type_info & info, bool is_lvalue_reference, bool is_rvalue_reference, bool is_const, bool is_volatile)
{
    const auto t = any_function::capture_type<T>();
    REQUIRE( t.info                == &info               );
    REQUIRE( t.is_lvalue_reference == is_lvalue_reference );
    REQUIRE( t.is_rvalue_reference == is_rvalue_reference );
    REQUIRE( t.is_const            == is_const            );
    REQUIRE( t.is_volatile         == is_volatile         );
}

// Test capturing the traits of a number of different types:                               fully qualified type          base type     lvref  rvref  const  vol
TEST_CASE( "test any_function::capture_type<int>()"                  ) { test_capture_type<               int   >(typeid(      int  ), false, false, false, false); }
TEST_CASE( "test any_function::capture_type<const int>()"            ) { test_capture_type<const          int   >(typeid(      int  ), false, false,  true, false); }
TEST_CASE( "test any_function::capture_type<const int &>()"          ) { test_capture_type<const          int & >(typeid(      int  ),  true, false,  true, false); }
TEST_CASE( "test any_function::capture_type<int &&>()"               ) { test_capture_type<               int &&>(typeid(      int  ), false,  true, false, false); }
TEST_CASE( "test any_function::capture_type<const volatile int>()"   ) { test_capture_type<const volatile int   >(typeid(      int  ), false, false,  true,  true); }
TEST_CASE( "test any_function::capture_type<const volatile int &>()" ) { test_capture_type<const volatile int & >(typeid(      int  ),  true, false,  true,  true); }
TEST_CASE( "test any_function::capture_type<float>()"                ) { test_capture_type<               float >(typeid(      float), false, false, false, false); }
TEST_CASE( "test any_function::capture_type<void>()"                 ) { test_capture_type<               void  >(typeid(      void ), false, false, false, false); }
TEST_CASE( "test any_function::capture_type<int *>()"                ) { test_capture_type<               int * >(typeid(      int *), false, false, false, false); }
TEST_CASE( "test any_function::capture_type<const int *>()"          ) { test_capture_type<const          int * >(typeid(const int *), false, false, false, false); }

/////////////////////////////////////////////////
// Test capturing different types of functions //
/////////////////////////////////////////////////

TEST_CASE( "any_function can default construct" ) 
{
    const any_function f;
    REQUIRE( !f );
    REQUIRE( f.get_parameter_types().empty() );
    REQUIRE( f.get_return_type() == any_function::type{} );
}

TEST_CASE( "any_function can constructed with nullptr" ) 
{
    const any_function f {nullptr};
    REQUIRE( !f );
    REQUIRE( f.get_parameter_types().empty() );
    REQUIRE( f.get_return_type() == any_function::type{} );
}

double global_function(int a, double b, float c) { return a*b+c; }
TEST_CASE( "any_function can be constructed with a global function" )
{
    const any_function f {&global_function};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 3 );
    REQUIRE( f.get_parameter_types()[0] == any_function::capture_type<int>() );
    REQUIRE( f.get_parameter_types()[1] == any_function::capture_type<double>() );
    REQUIRE( f.get_parameter_types()[2] == any_function::capture_type<float>() );
    REQUIRE( f.get_return_type() == any_function::capture_type<double>() );
}

TEST_CASE( "any_function can be constructed with a std::function" )
{
    std::function<double(int,double,float)> sf {[](int a, double b, float c) { return a*b+c; }};
    const any_function f {sf};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 3 );
    REQUIRE( f.get_parameter_types()[0] == any_function::capture_type<int>() );
    REQUIRE( f.get_parameter_types()[1] == any_function::capture_type<double>() );
    REQUIRE( f.get_parameter_types()[2] == any_function::capture_type<float>() );
    REQUIRE( f.get_return_type() == any_function::capture_type<double>() );
}

TEST_CASE( "any_function can be constructed with an arity-0 stateless lambda" )
{
    const any_function f {[]() { return 5.0; }};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 0 );
    REQUIRE( f.get_return_type() == any_function::capture_type<double>() );
}

TEST_CASE( "any_function can be constructed with an arity-1 stateless lambda" )
{
    const any_function f {[](int a) { return a; }};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 1 );
    REQUIRE( f.get_parameter_types()[0] == any_function::capture_type<int>() );
    REQUIRE( f.get_return_type() == any_function::capture_type<int>() );
}

TEST_CASE( "any_function can be constructed with an arity-2 stateless lambda" )
{
    const any_function f {[](int a, double b) { return a*b; }};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 2 );
    REQUIRE( f.get_parameter_types()[0] == any_function::capture_type<int>() );
    REQUIRE( f.get_parameter_types()[1] == any_function::capture_type<double>() );
    REQUIRE( f.get_return_type() == any_function::capture_type<double>() );
}

TEST_CASE( "any_function can be constructed with an arity-3 stateless lambda" )
{
    const any_function f {[](int a, double b, float c) { return a*b+c; }};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 3 );
    REQUIRE( f.get_parameter_types()[0] == any_function::capture_type<int>() );
    REQUIRE( f.get_parameter_types()[1] == any_function::capture_type<double>() );
    REQUIRE( f.get_parameter_types()[2] == any_function::capture_type<float>() );
    REQUIRE( f.get_return_type() == any_function::capture_type<double>() );
}

TEST_CASE( "any_function can be constructed with an arity-4 stateless lambda" )
{
    const any_function f {[](int a, double b, float c, short d) { return a*b+c*d; }};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 4 );
    REQUIRE( f.get_parameter_types()[0] == any_function::capture_type<int>() );
    REQUIRE( f.get_parameter_types()[1] == any_function::capture_type<double>() );
    REQUIRE( f.get_parameter_types()[2] == any_function::capture_type<float>() );
    REQUIRE( f.get_parameter_types()[3] == any_function::capture_type<short>() );
    REQUIRE( f.get_return_type() == any_function::capture_type<double>() );
}

double g;
TEST_CASE( "any_function can be constructed with an arity-3 stateless lambda returning void" )
{
    const any_function f {[](int a, double b, float c) { g=a*b+c; }};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 3 );
    REQUIRE( f.get_parameter_types()[0] == any_function::capture_type<int>() );
    REQUIRE( f.get_parameter_types()[1] == any_function::capture_type<double>() );
    REQUIRE( f.get_parameter_types()[2] == any_function::capture_type<float>() );
    REQUIRE( f.get_return_type() == any_function::capture_type<void>() );
}

///////////////////////////////////////////////
// Test calling different types of functions //
///////////////////////////////////////////////

TEST_CASE( "any_function is callable with a global function" )
{
    const any_function f {&global_function};
    int a = 5; double b = 12.2; float c = 3.14f;
    auto r = f.invoke({&a,&b,&c});
    REQUIRE( r != nullptr );
    REQUIRE( *reinterpret_cast<double *>(r.get()) == a*b+c );
}

TEST_CASE( "any_function is callable with a std::function" )
{
    std::function<double(int,double,float)> sf {[](int a, double b, float c) { return a*b+c; }};
    const any_function f {sf};
    int a = 5; double b = 12.2; float c = 3.14f;
    auto r = f.invoke({&a,&b,&c});
    REQUIRE( r != nullptr );
    REQUIRE( *reinterpret_cast<double *>(r.get()) == a*b+c );
}

TEST_CASE( "any_function is callable with an arity-0 stateless lambda" )
{
    const any_function f {[]() { return 5.0; }};
    auto r = f.invoke({});
    REQUIRE( r != nullptr );
    REQUIRE( *reinterpret_cast<double *>(r.get()) == 5.0 );
}

TEST_CASE( "any_function is callable with an arity-3 stateless lambda" )
{
    const any_function f {[](int a, double b, float c) { return a*b+c; }};
    int a = 5; double b = 12.2; float c = 3.14f;
    auto r = f.invoke({&a,&b,&c});
    REQUIRE( r != nullptr );
    REQUIRE( *reinterpret_cast<double *>(r.get()) == a*b+c );
}

TEST_CASE( "any_function is callable with an arity-0 stateless lambda returning void" )
{
    const any_function f {[]() { g=5.0; }};
    auto r = f.invoke({});
    REQUIRE( r == nullptr );
}

TEST_CASE( "any_function is callable with an arity-3 stateless lambda returning void" )
{
    const any_function f {[](int a, double b, float c) { g=a*b+c; }};
    int a = 5; double b = 12.2; float c = 3.14f;
    auto r = f.invoke({&a,&b,&c});
    REQUIRE( r == nullptr );
}
