# WARNING: THIS LIBRARY IS UNFINISHED AND UNTESTED

# any_function.h

[![Release is 0.6](http://img.shields.io/badge/release-0.6-blue.svg?style=flat)](/any_function.h)
[![License is Unlicense](http://img.shields.io/badge/license-Unlicense-blue.svg?style=flat)](http://unlicense.org/)

Platform | Build Status |
-------- | ------------ |
Visual Studio 2013 | [AppVeyor](http://ci.appveyor.com/): [![Build status](https://ci.appveyor.com/api/projects/status/t9hynmje3af3t0eg?svg=true)](https://ci.appveyor.com/project/sgorsten/any-function) |
GCC 4.9 | [Travis CI](http://travis-ci.org): [![Build status](http://travis-ci.org/sgorsten/any_function.svg?branch=master)](https://travis-ci.org/sgorsten/any_function) |

[any_function.h](/any_function.h) is a [single header](http://github.com/nothings/stb/blob/master/docs/other_libs.md) [public domain](http://unlicense.org/) utility library for [C++11](http://en.cppreference.com/w/). 

It is intended to serve as a functional counterpart to the [`std::any`](http://en.cppreference.com/w/cpp/utility/any), by provided a single, concrete class `any_function` which can receive almost any callable object, from function pointers to lambdas to instantiations of [`std::function`](http://en.cppreference.com/w/cpp/utility/functional/function).

Once constructed, `any_function` can be called by passing it an array of void pointers to the intended arguments. It is the caller's responsibility to ensure that the number and type of arguments match the actual function. A [`shared_ptr`](http://en.cppreference.com/w/cpp/memory/shared_ptr) is constructed to the return value using [`make_shared`](http://en.cppreference.com/w/cpp/memory/shared_ptr/make_shared), which is then returned as a [`shared_ptr<void>`](http://en.cppreference.com/w/cpp/memory/shared_ptr). Functions without a return value will return an empty [`shared_ptr<void>`](http://en.cppreference.com/w/cpp/memory/shared_ptr) to be returned.

# TODO

- [X] Retain reference/const/volatile qualification in parameter and return type metadata
- [X] L-value reference parameters (should work, but needs testing)
- [ ] R-value reference parameters
- [ ] Const/volatile qualified parameters
- [ ] L-value reference return types
- [ ] R-value reference return types
- [ ] Const/volatile qualified return type
- [ ] Mutable lambdas / stateful function objects

