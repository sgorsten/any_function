// any_function - v0.9 - Single-header public domain library
//
// The intent of this library is to provide a functional counterpart to
// std::any, called any_function. any_function can receive anything that is
// callable with a unique signature, such as a function pointer, a
// std::function, or any callable object with a single, non-generic
// operator() overload. any_function can be called with an array of pointers
// to void, and returns the return value in a std::shared_ptr<void>. It also
// remembers the std::type_info of the parameters and return value of the
// function it was constructed with.
//
// The original author of this software is Sterling Orsten, and its permanent
// home is <http://github.com/sgorsten/any_function/>. If you find this software
// useful, an acknowledgement in your source text and/or product documentation
// is appreciated, but not required.



// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// For more information, please refer to <http://unlicense.org/>



#pragma once
#ifndef ANY_FUNCTION_H
#define ANY_FUNCTION_H

#include <cassert>      // For assert(...)
#include <functional>   // For std::function<F>
#include <vector>       // For std::vector<T>
#include <memory>       // For std::unique_ptr<T>

struct any_function
{
public:
    struct type 
    { 
        const std::type_info *                          info; 
        bool                                            is_lvalue_reference, is_rvalue_reference;
        bool                                            is_const, is_volatile; 
        bool                                            operator == (const type & r) const                      { return info==r.info && is_lvalue_reference==r.is_lvalue_reference && is_rvalue_reference==r.is_rvalue_reference && is_const==r.is_const && is_volatile==r.is_volatile; }
        bool                                            operator != (const type & r) const                      { return !(*this == r); }
        template<class T> static type                   capture()                                               { return {&typeid(T), std::is_lvalue_reference<T>::value, std::is_rvalue_reference<T>::value, std::is_const<typename std::remove_reference<T>::type>::value, std::is_volatile<typename std::remove_reference<T>::type>::value}; }
    };

    class result
    {
        struct result_base
        {
            virtual                                     ~result_base()                                          {}
            virtual std::unique_ptr<result_base>        clone() const                                           = 0;
            virtual type                                get_type() const                                        = 0;
            virtual void *                              get_address()                                           = 0;     
        };
        template<class T> struct typed_result : result_base
        {
            T                                           x;
                                                        typed_result(T x)                                       : x(x) {}
            std::unique_ptr<result_base>                clone() const                                           { return std::unique_ptr<typed_result>(new typed_result(x)); }
            type                                        get_type() const                                        { return type::capture<T>(); }
            void *                                      get_address()                                           { return (void *)&x; }
        };
        template<class T> struct typed_result<T &&> : result_base
        {
            T &&                                        x;
                                                        typed_result(T && x)                                    : x(std::move(x)) {}
            std::unique_ptr<result_base>                clone() const                                           { return std::unique_ptr<typed_result>(new typed_result(std::move(x))); }
            type                                        get_type() const                                        { return type::capture<T &&>(); }
            void *                                      get_address()                                           { return (void *)&x; }
        };
        std::unique_ptr<result_base>                    p;
    public:
                                                        result()                                                {}
                                                        result(result && r)                                     : p(move(r.p)) {}
                                                        result(const result & r)                                { *this = r; }
        result &                                        operator = (result && r)                                { p.swap(r.p); return *this; }
        result &                                        operator = (const result & r)                           { p = r.p ? r.p->clone() : nullptr; return *this; }

        type                                            get_type() const                                        { return p ? p->get_type() : type::capture<void>(); }
        void *                                          get_address()                                           { return p ? p->get_address() : nullptr; }
        template<class T> T                             get_value()                                             { assert(get_type() == type::capture<T>()); return get(p->get_address(), tag<T>{}); }

        template<class T> static result                 capture(T x)                                            { result r; r.p.reset(new typed_result<T>(static_cast<T>(x))); return r; }
    };
                                                        any_function()                                          : result_type{} {}
                                                        any_function(std::nullptr_t)                            : result_type{} {}
    template<class R, class... A>                       any_function(R (*p)(A...))                              : any_function(p, tag<R>{}, tag<A...>{}, build_indices<sizeof...(A)>{}) {} 
    template<class R, class... A>                       any_function(std::function<R(A...)> f)                  : any_function(f, tag<R>{}, tag<A...>{}, build_indices<sizeof...(A)>{}) {} 
    template<class F>                                   any_function(F f)                                       : any_function(f, &F::operator()) {}   

    explicit                                            operator bool() const                                   { return static_cast<bool>(func); }
    const std::vector<type> &                           get_parameter_types() const                             { return parameter_types; }
    const type &                                        get_result_type() const                                 { return result_type; }
    result                                              invoke(void * const args[]) const                       { return func(args); }
    result                                              invoke(std::initializer_list<void *> args) const        { return invoke(args.begin()); }

private:
    template<class... T> struct                         tag                                                     {};
    template<std::size_t... IS> struct                  indices                                                 {};
    template<std::size_t N, std::size_t... IS> struct   build_indices                                           : build_indices<N-1, N-1, IS...> {};
    template<std::size_t... IS> struct                  build_indices<0, IS...>                                 : indices<IS...> {};

    template<class T> static T                          get(void * arg, tag<T>   )                              { return           *reinterpret_cast<T *>(arg);  }
    template<class T> static T &                        get(void * arg, tag<T &> )                              { return           *reinterpret_cast<T *>(arg);  }
    template<class T> static T &&                       get(void * arg, tag<T &&>)                              { return std::move(*reinterpret_cast<T *>(arg)); }
    template<class F, class R, class... A, size_t... I> any_function(F f, tag<R   >, tag<A...>, indices<I...>)  : parameter_types({type::capture<A>()...}), result_type(type::capture<R   >()) { func = [f](void * const args[]) mutable { return result::capture<R   >(f(get(args[I], tag<A>{})...));          }; }
    template<class F, class R, class... A, size_t... I> any_function(F f, tag<R & >, tag<A...>, indices<I...>)  : parameter_types({type::capture<A>()...}), result_type(type::capture<R & >()) { func = [f](void * const args[]) mutable { return result::capture<R & >(f(get(args[I], tag<A>{})...));          }; }
    template<class F, class R, class... A, size_t... I> any_function(F f, tag<R &&>, tag<A...>, indices<I...>)  : parameter_types({type::capture<A>()...}), result_type(type::capture<R &&>()) { func = [f](void * const args[]) mutable { return result::capture<R &&>(f(get(args[I], tag<A>{})...));          }; }
    template<class F,          class... A, size_t... I> any_function(F f, tag<void>, tag<A...>, indices<I...>)  : parameter_types({type::capture<A>()...}), result_type(type::capture<void>()) { func = [f](void * const args[]) mutable { return                       f(get(args[I], tag<A>{})...), result{}; }; }
    template<class F, class R                         > any_function(F f, tag<R   >, tag<    >, indices<    >)  : parameter_types({                     }), result_type(type::capture<R   >()) { func = [f](void * const args[]) mutable { return result::capture<R   >(f(                         ));          }; }
    template<class F, class R                         > any_function(F f, tag<R & >, tag<    >, indices<    >)  : parameter_types({                     }), result_type(type::capture<R & >()) { func = [f](void * const args[]) mutable { return result::capture<R & >(f(                         ));          }; }
    template<class F, class R                         > any_function(F f, tag<R &&>, tag<    >, indices<    >)  : parameter_types({                     }), result_type(type::capture<R &&>()) { func = [f](void * const args[]) mutable { return result::capture<R &&>(f(                         ));          }; }
    template<class F                                  > any_function(F f, tag<void>, tag<    >, indices<    >)  : parameter_types({                     }), result_type(type::capture<void>()) { func = [f](void * const args[]) mutable { return                       f(                         ), result{}; }; }
    template<class F, class R, class... A             > any_function(F f, R (F::*p)(A...)      )                : any_function(f, tag<R>{}, tag<A...>{}, build_indices<sizeof...(A)>{}) {}
    template<class F, class R, class... A             > any_function(F f, R (F::*p)(A...) const)                : any_function(f, tag<R>{}, tag<A...>{}, build_indices<sizeof...(A)>{}) {}

    std::function<result(void * const *)>               func;
    std::vector<type>                                   parameter_types;
    type                                                result_type;
};

#endif
