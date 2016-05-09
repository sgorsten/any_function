#include "../any_function.h"

#define CATCH_CONFIG_MAIN
#include "thirdparty/catch.hpp"

template<class T> void test_type_capture(const std::type_info & info, bool is_lvalue_reference, bool is_rvalue_reference, bool is_const, bool is_volatile)
{
    const auto t = any_function::type::capture<T>();
    REQUIRE( t.info                == &info               );
    REQUIRE( t.is_lvalue_reference == is_lvalue_reference );
    REQUIRE( t.is_rvalue_reference == is_rvalue_reference );
    REQUIRE( t.is_const            == is_const            );
    REQUIRE( t.is_volatile         == is_volatile         );
}

// Test capturing the traits of a number of different types:                                fully qualified type          base type     lvref  rvref  const  vol
TEST_CASE( "test any_function::type::capture<int>()"                  ) { test_type_capture<               int   >(typeid(      int  ), false, false, false, false); }
TEST_CASE( "test any_function::type::capture<const int>()"            ) { test_type_capture<const          int   >(typeid(      int  ), false, false,  true, false); }
TEST_CASE( "test any_function::type::capture<const int &>()"          ) { test_type_capture<const          int & >(typeid(      int  ),  true, false,  true, false); }
TEST_CASE( "test any_function::type::capture<int &&>()"               ) { test_type_capture<               int &&>(typeid(      int  ), false,  true, false, false); }
TEST_CASE( "test any_function::type::capture<const volatile int>()"   ) { test_type_capture<const volatile int   >(typeid(      int  ), false, false,  true,  true); }
TEST_CASE( "test any_function::type::capture<const volatile int &>()" ) { test_type_capture<const volatile int & >(typeid(      int  ),  true, false,  true,  true); }
TEST_CASE( "test any_function::type::capture<float>()"                ) { test_type_capture<               float >(typeid(      float), false, false, false, false); }
TEST_CASE( "test any_function::type::capture<void>()"                 ) { test_type_capture<               void  >(typeid(      void ), false, false, false, false); }
TEST_CASE( "test any_function::type::capture<int *>()"                ) { test_type_capture<               int * >(typeid(      int *), false, false, false, false); }
TEST_CASE( "test any_function::type::capture<const int *>()"          ) { test_type_capture<const          int * >(typeid(const int *), false, false, false, false); }

/////////////////////////////////////////////////
// Test capturing different types of functions //
/////////////////////////////////////////////////

TEST_CASE( "any_function can default construct" ) 
{
    const any_function f;
    REQUIRE( !f );
    REQUIRE( f.get_parameter_types().empty() );
    REQUIRE( f.get_result_type() == any_function::type{} );
}

TEST_CASE( "any_function can constructed with nullptr" ) 
{
    const any_function f {nullptr};
    REQUIRE( !f );
    REQUIRE( f.get_parameter_types().empty() );
    REQUIRE( f.get_result_type() == any_function::type{} );
}

double global_function(int a, double b, float c) { return a*b+c; }
TEST_CASE( "any_function can be constructed with a global function" )
{
    const any_function f {&global_function};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 3 );
    REQUIRE( f.get_parameter_types()[0] == any_function::type::capture<int>() );
    REQUIRE( f.get_parameter_types()[1] == any_function::type::capture<double>() );
    REQUIRE( f.get_parameter_types()[2] == any_function::type::capture<float>() );
    REQUIRE( f.get_result_type() == any_function::type::capture<double>() );
}

TEST_CASE( "any_function can be constructed with a std::function" )
{
    std::function<double(int,double,float)> sf {[](int a, double b, float c) { return a*b+c; }};
    const any_function f {sf};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 3 );
    REQUIRE( f.get_parameter_types()[0] == any_function::type::capture<int>() );
    REQUIRE( f.get_parameter_types()[1] == any_function::type::capture<double>() );
    REQUIRE( f.get_parameter_types()[2] == any_function::type::capture<float>() );
    REQUIRE( f.get_result_type() == any_function::type::capture<double>() );
}

TEST_CASE( "any_function can be constructed with an arity-0 stateless lambda" )
{
    const any_function f {[]() { return 5.0; }};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 0 );
    REQUIRE( f.get_result_type() == any_function::type::capture<double>() );
}

TEST_CASE( "any_function can be constructed with an arity-1 stateless lambda" )
{
    const any_function f {[](int a) { return a; }};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 1 );
    REQUIRE( f.get_parameter_types()[0] == any_function::type::capture<int>() );
    REQUIRE( f.get_result_type() == any_function::type::capture<int>() );
}

TEST_CASE( "any_function can be constructed with an arity-2 stateless lambda" )
{
    const any_function f {[](int a, double b) { return a*b; }};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 2 );
    REQUIRE( f.get_parameter_types()[0] == any_function::type::capture<int>() );
    REQUIRE( f.get_parameter_types()[1] == any_function::type::capture<double>() );
    REQUIRE( f.get_result_type() == any_function::type::capture<double>() );
}

TEST_CASE( "any_function can be constructed with an arity-3 stateless lambda" )
{
    const any_function f {[](int a, double b, float c) { return a*b+c; }};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 3 );
    REQUIRE( f.get_parameter_types()[0] == any_function::type::capture<int>() );
    REQUIRE( f.get_parameter_types()[1] == any_function::type::capture<double>() );
    REQUIRE( f.get_parameter_types()[2] == any_function::type::capture<float>() );
    REQUIRE( f.get_result_type() == any_function::type::capture<double>() );
}

TEST_CASE( "any_function can be constructed with an arity-4 stateless lambda" )
{
    const any_function f {[](int a, double b, float c, short d) { return a*b+c*d; }};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 4 );
    REQUIRE( f.get_parameter_types()[0] == any_function::type::capture<int>() );
    REQUIRE( f.get_parameter_types()[1] == any_function::type::capture<double>() );
    REQUIRE( f.get_parameter_types()[2] == any_function::type::capture<float>() );
    REQUIRE( f.get_parameter_types()[3] == any_function::type::capture<short>() );
    REQUIRE( f.get_result_type() == any_function::type::capture<double>() );
}

double g;
TEST_CASE( "any_function can be constructed with an arity-3 stateless lambda returning void" )
{
    const any_function f {[](int a, double b, float c) { g=a*b+c; }};
    REQUIRE( f );
    REQUIRE( f.get_parameter_types().size() == 3 );
    REQUIRE( f.get_parameter_types()[0] == any_function::type::capture<int>() );
    REQUIRE( f.get_parameter_types()[1] == any_function::type::capture<double>() );
    REQUIRE( f.get_parameter_types()[2] == any_function::type::capture<float>() );
    REQUIRE( f.get_result_type() == any_function::type::capture<void>() );
}

///////////////////////////////////////////////
// Test calling different types of functions //
///////////////////////////////////////////////

TEST_CASE( "any_function is callable with a global function" )
{
    const any_function f {&global_function};
    int a = 5; double b = 12.2; float c = 3.14f;
    auto r = f.invoke({&a,&b,&c});
    REQUIRE( r.get_type() == any_function::type::capture<double>() );
    REQUIRE( r.get_value<double>() == a*b+c );
}

TEST_CASE( "any_function is callable with a std::function" )
{
    std::function<double(int,double,float)> sf {[](int a, double b, float c) { return a*b+c; }};
    const any_function f {sf};
    int a = 5; double b = 12.2; float c = 3.14f;
    auto r = f.invoke({&a,&b,&c});
    REQUIRE( r.get_type() == any_function::type::capture<double>() );
    REQUIRE( r.get_value<double>() == a*b+c );
}

TEST_CASE( "any_function is callable with an arity-0 stateless lambda" )
{
    const any_function f {[]() { return 5.0; }};
    auto r = f.invoke({});
    REQUIRE( r.get_type() == any_function::type::capture<double>() );
    REQUIRE( r.get_value<double>() == 5.0 );
}

TEST_CASE( "any_function is callable with an arity-3 stateless lambda" )
{
    const any_function f {[](int a, double b, float c) { return a*b+c; }};
    int a = 5; double b = 12.2; float c = 3.14f;
    auto r = f.invoke({&a,&b,&c});
    REQUIRE( r.get_type() == any_function::type::capture<double>() );
    REQUIRE( r.get_value<double>() == a*b+c );
}

TEST_CASE( "any_function is callable with an arity-0 stateless lambda returning void" )
{
    const any_function f {[]() { g=5.0; }};
    auto r = f.invoke({});
    REQUIRE( r.get_type() == any_function::type::capture<void>() );
}

TEST_CASE( "any_function is callable with an arity-3 stateless lambda returning void" )
{
    const any_function f {[](int a, double b, float c) { g=a*b+c; }};
    int a = 5; double b = 12.2; float c = 3.14f;
    auto r = f.invoke({&a,&b,&c});
    REQUIRE( r.get_type() == any_function::type::capture<void>() );
}

//////////////////////////////////////////////////////
// Test calling functions with qualified parameters //
//////////////////////////////////////////////////////

TEST_CASE( "any_function is callable with l-value references" )
{
    const any_function f {[](double & x) { x=5; }};
    REQUIRE( f.get_parameter_types().size() == 1 );
    REQUIRE( f.get_parameter_types()[0] == any_function::type::capture<double &>() );
    REQUIRE( f.get_result_type() == any_function::type::capture<void>() );

    double x {0};
    REQUIRE( x == 0 );
    
    auto r = f.invoke({&x});
    REQUIRE( x == 5 ); // x should have been modified by reference by f
    REQUIRE( r.get_type() == any_function::type::capture<void>() );
}

TEST_CASE( "any_function is callable with r-value references" )
{
    const any_function f {[](std::vector<double> && x) { return std::move(x); }};
    REQUIRE( f.get_parameter_types().size() == 1 );
    REQUIRE( f.get_parameter_types()[0] == any_function::type::capture<std::vector<double> &&>() );
    REQUIRE( f.get_result_type() == any_function::type::capture<std::vector<double>>() );

    std::vector<double> x = {1,2,3,4,5,6,7,8,9,10};
    REQUIRE( x.size() == 10 );
    
    auto r = f.invoke({&x});
    REQUIRE( x.size() == 0 ); // x should have been moved-from by f
    REQUIRE( r.get_type() == any_function::type::capture<std::vector<double>>() );
    REQUIRE( r.get_value<std::vector<double>>().size() == 10 );
}

TEST_CASE( "any_function can return l-value references" )
{
    double x {};
    const any_function f {[&x]() -> double & { return x; }};
    REQUIRE( f.get_parameter_types().size() == 0 );
    REQUIRE( f.get_result_type() == any_function::type::capture<double &>() );
        
    auto r = f.invoke({});
    REQUIRE( r.get_type() == any_function::type::capture<double &>() );
    REQUIRE( &r.get_value<double &>() == &x );    
}

TEST_CASE( "any_function can return r-value references" )
{
    double x {};
    const any_function f {[&x]() -> double && { return std::move(x); }};
    REQUIRE( f.get_parameter_types().size() == 0 );
    REQUIRE( f.get_result_type() == any_function::type::capture<double &&>() );
        
    auto r = f.invoke({});
    REQUIRE( r.get_type() == any_function::type::capture<double &&>() );

    double && ref_x = r.get_value<double &&>();
    REQUIRE( &ref_x == &x );
}

TEST_CASE( "any_function can return const l-value references" )
{
    double x {};
    const any_function f {[&x]() -> const double & { return x; }};
    REQUIRE( f.get_parameter_types().size() == 0 );
    REQUIRE( f.get_result_type() == any_function::type::capture<const double &>() );
        
    auto r = f.invoke({});
    REQUIRE( r.get_type() == any_function::type::capture<const double &>() );
    REQUIRE( &r.get_value<const double &>() == &x );
}