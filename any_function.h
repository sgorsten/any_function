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
    template<std::size_t... IS> struct                      indices                 {};
    template<std::size_t N, std::size_t... IS> struct       build_indices           : build_indices<N-1, N-1, IS...> {};
    template<std::size_t... IS> struct                      build_indices<0, IS...> : indices<IS...> {};

    std::function<std::shared_ptr<void>(void * const *)>    func;
    std::vector<const std::type_info *>                     parameter_types;
    const std::type_info *                                  return_type = nullptr;

    template<class F, class R, class... A, size_t... I>     any_function(F f, R    *, std::tuple<A...> *, indices<I...>) : parameter_types({&typeid(A)...}), return_type(&typeid(R   )) { func = [f](void * const args[]) { return std::make_shared<R>(f(*reinterpret_cast<std::remove_reference_t<A> *>(args[I])...));                }; }
    template<class F,          class... A, size_t... I>     any_function(F f, void *, std::tuple<A...> *, indices<I...>) : parameter_types({&typeid(A)...}), return_type(&typeid(void)) { func = [f](void * const args[]) {                            f(*reinterpret_cast<std::remove_reference_t<A> *>(args[I])...); return nullptr; }; }
    template<class F, class R, class... A>                  any_function(F f, R (F::*p)(A...) const)                     : any_function(f, (R*)0, (std::tuple<A...>*)0, build_indices<sizeof...(A)>{}) {}
public:
                                                            any_function()                          {}
                                                            any_function(nullptr_t)                 {}
    template<class R, class... A>                           any_function(R (*p)(A...))              : any_function(p, (R*)0, (std::tuple<A...>*)0, build_indices<sizeof...(A)>{}) {} 
    template<class R, class... A>                           any_function(std::function<R(A...)> f)  : any_function(f, (R*)0, (std::tuple<A...>*)0, build_indices<sizeof...(A)>{}) {} 
    template<class F>                                       any_function(F f)                       : any_function(f, &F::operator()) {}   

    explicit                                                operator bool() const                   { return static_cast<bool>(func); }
    const std::vector<const std::type_info *> &             get_parameter_types() const             { return parameter_types; }
    const std::type_info *                                  get_return_type() const                 { return return_type; }
    std::shared_ptr<void>                                   invoke(void * const args[]) const       { return func(args); }
};

#endif
