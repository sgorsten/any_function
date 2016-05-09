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

#include <functional>   // For std::function
#include <vector>       // For std::vector
#include <memory>       // For std::shared_ptr

struct any_function
{
public:
    struct type 
    { 
        const std::type_info * info; bool is_lvalue_reference, is_rvalue_reference, is_const, is_volatile; 
        bool operator == (const type & r) const { return info==r.info && is_lvalue_reference==r.is_lvalue_reference && is_rvalue_reference==r.is_rvalue_reference && is_const==r.is_const && is_volatile==r.is_volatile; }
        bool operator != (const type & r) const { return !(*this == r); }
    };
    template<class T> static type capture_type() 
    { 
        return {&typeid(T), std::is_lvalue_reference<T>::value, std::is_rvalue_reference<T>::value, 
            std::is_const<typename std::remove_reference<T>::type>::value, 
            std::is_volatile<typename std::remove_reference<T>::type>::value}; 
    }

    struct result_base
    {
        virtual ~result_base() {}
        virtual type get_type() const = 0;
        virtual void * get_address() = 0;     
    };

    template<class T> struct typed_result : result_base
    {
        T x;
        typed_result(T x) : x(static_cast<T>(x)) {}
        type get_type() const { return capture_type<T>(); }
        void * get_address() { return const_cast<typename std::remove_cv<typename std::remove_reference<T>::type>::type *>(&x); }
    };

    class result
    {
        std::unique_ptr<result_base> p;
    public:
        result() {}
        result(result && r) : p(move(r.p)) {}
        result(std::unique_ptr<result_base> && r) : p(move(r)) {}
        result & operator = (result && r) { p.swap(r.p); return *this; }

        type get_type() const { return p ? p->get_type() : capture_type<void>(); }
        void * get_address() { return p ? p->get_address() : nullptr; }
        template<class T> T get_value() { return get(p->get_address(), tag<T>{}); }
    };
public:
    template<class T> struct                                tag {};
    template<std::size_t... IS> struct                      indices                 {};
    template<std::size_t N, std::size_t... IS> struct       build_indices           : build_indices<N-1, N-1, IS...> {};
    template<std::size_t... IS> struct                      build_indices<0, IS...> : indices<IS...> {};

    std::function<result(void * const *)>                   func;
    std::vector<type>                                       parameter_types;
    type                                                    return_type {};
   
    template<class T> static T                              get(void * arg, tag<T>   ) { return           *reinterpret_cast<T *>(arg);  }
    template<class T> static T &                            get(void * arg, tag<T &> ) { return           *reinterpret_cast<T *>(arg);  }
    template<class T> static T &&                           get(void * arg, tag<T &&>) { return std::move(*reinterpret_cast<T *>(arg)); }
    template<class F, class R, class... A, size_t... I>     any_function(F f, tag<R   >, std::tuple<A...> *, indices<I...>) : parameter_types({capture_type<A>()...}), return_type(capture_type<R   >()) { func = [f](void * const args[]) -> result { return std::unique_ptr<result_base>(new typed_result<R   >{f(get(args[I], tag<A>{})...)});        }; }
    template<class F, class R, class... A, size_t... I>     any_function(F f, tag<R & >, std::tuple<A...> *, indices<I...>) : parameter_types({capture_type<A>()...}), return_type(capture_type<R & >()) { func = [f](void * const args[]) -> result { return std::unique_ptr<result_base>(new typed_result<R & >{f(get(args[I], tag<A>{})...)});        }; }
    template<class F, class R, class... A, size_t... I>     any_function(F f, tag<R &&>, std::tuple<A...> *, indices<I...>) : parameter_types({capture_type<A>()...}), return_type(capture_type<R &&>()) { func = [f](void * const args[]) -> result { return std::unique_ptr<result_base>(new typed_result<R &&>{f(get(args[I], tag<A>{})...)});        }; }
    template<class F,          class... A, size_t... I>     any_function(F f, tag<void>, std::tuple<A...> *, indices<I...>) : parameter_types({capture_type<A>()...}), return_type(capture_type<void>()) { func = [f](void * const args[]) -> result { return                                                     f(get(args[I], tag<A>{})...), nullptr; }; }
    template<class F, class R                         >     any_function(F f, tag<R   >, std::tuple<    > *, indices<    >) : parameter_types({                    }), return_type(capture_type<R   >()) { func = [f](void * const args[]) -> result { return std::unique_ptr<result_base>(new typed_result<R   >{f(                         )});        }; }
    template<class F, class R                         >     any_function(F f, tag<R & >, std::tuple<    > *, indices<    >) : parameter_types({                    }), return_type(capture_type<R & >()) { func = [f](void * const args[]) -> result { return std::unique_ptr<result_base>(new typed_result<R & >{f(                         )});        }; }
    template<class F, class R                         >     any_function(F f, tag<R &&>, std::tuple<    > *, indices<    >) : parameter_types({                    }), return_type(capture_type<R &&>()) { func = [f](void * const args[]) -> result { return std::unique_ptr<result_base>(new typed_result<R &&>{f(                         )});        }; }
    template<class F                                  >     any_function(F f, tag<void>, std::tuple<    > *, indices<    >) : parameter_types({                    }), return_type(capture_type<void>()) { func = [f](void * const args[]) -> result { return                                                     f(                         ), nullptr; }; }
    template<class F, class R, class... A             >     any_function(F f, R (F::*p)(A...) const)                     : any_function(f, tag<R>{}, (std::tuple<A...>*)0, build_indices<sizeof...(A)>{}) {}
public:
                                                            any_function()                                      {}
                                                            any_function(std::nullptr_t)                        {}
    template<class R, class... A>                           any_function(R (*p)(A...))                          : any_function(p, tag<R>{}, (std::tuple<A...>*)0, build_indices<sizeof...(A)>{}) {} 
    template<class R, class... A>                           any_function(std::function<R(A...)> f)              : any_function(f, tag<R>{}, (std::tuple<A...>*)0, build_indices<sizeof...(A)>{}) {} 
    template<class F>                                       any_function(F f)                                   : any_function(f, &F::operator()) {}   

    explicit                                                operator bool() const                               { return static_cast<bool>(func); }
    const std::vector<type> &                               get_parameter_types() const                         { return parameter_types; }
    const type &                                            get_return_type() const                             { return return_type; }
    result                                                  invoke(void * const args[]) const                   { return func(args); }

    result                                                  invoke(std::initializer_list<void *> args) const    { return invoke(args.begin()); }
};

#endif
