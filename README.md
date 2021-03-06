# any_function.h

[![Release is 0.8](http://img.shields.io/badge/release-0.8-blue.svg?style=flat)](/any_function.h)
[![License is Unlicense](http://img.shields.io/badge/license-Unlicense-blue.svg?style=flat)](http://unlicense.org/)

Platform | Build Status |
-------- | ------------ |
Visual Studio 2013 | [AppVeyor](http://ci.appveyor.com/): [![Build status](https://ci.appveyor.com/api/projects/status/t9hynmje3af3t0eg?svg=true)](https://ci.appveyor.com/project/sgorsten/any-function) |
GCC 4.9 | [Travis CI](http://travis-ci.org): [![Build status](http://travis-ci.org/sgorsten/any_function.svg?branch=master)](https://travis-ci.org/sgorsten/any_function) |

[any_function.h](/any_function.h) is a [single header](http://github.com/nothings/stb/blob/master/docs/other_libs.md) [public domain](http://unlicense.org/) utility library for [C++11](http://en.cppreference.com/w/). 

It is intended to serve as a functional counterpart to the [`std::any`](http://en.cppreference.com/w/cpp/utility/any), by providing a single, concrete class `any_function` which can receive almost any callable object, from function pointers to lambdas to instantiations of [`std::function`](http://en.cppreference.com/w/cpp/utility/functional/function).

This library is still under development and its API and implementation details are subject to change.

# TODO

- [X] Retain reference/const/volatile qualification in parameter and return type metadata
- [X] L-value reference parameters (should work, but needs testing)
- [X] R-value reference parameters
- [X] L-value reference return types
- [X] R-value reference return types
- [X] Const/volatile qualified parameters
- [X] Const/volatile qualified return type
- [X] Mutable lambdas / stateful function objects

