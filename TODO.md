# TODO

## Type system
- optional and optional ref types (x : int?, x : ref int?)
- generics and constrains
- variant types (tagged union + pattern matching methods)
- arrays and slices
- multiple return types / tuples
- alias keyword for type aliases to creating new types
- better string type
- more numeric type
- casts

## Syntax & control flow
- for loops
- ranges
- exhaustive pattern matching
- if/while/for/match expressions returning values or arrays
- named and default arguments
- string interpolation
- flow typing for optionals/variants

## Operators
- defining operators for builtin types
- wrapping and non-wrapping operators similar to zig, (+, +%, -, -%, ...)
- bitwise operators as functions

## Functions & first-class values
- first class functions
- lambdas

## Compile-time & metaprogramming
- static if for compile time eval
- injecting data like enum values during build
- synthesized methods for types and enums (stringify, ...)

## Memory & resource management
- different allocators, Type.new'stack(), Type.new'heap(), stack is the default
- RAII
    - destructors
- move semantics

## Backend & codegen
- optimization passes in the ir layer
- debug and release mode compilation
- debug annotation on functions that get stripped away in release mode
- debug information for llvm
- wasm compilation target

## Standard library & runtime
- caracal entrypoint that calls user main function
    - global Program object containing cli params, app path
- standard library
    - console
    - files
    - collections
    - json
    - random
    - math
    - time
    - test runner
    - logging

## Interop / FFI
- extern annotation for static methods
- noreturn annotation
- C binding generator
- generate C headers for calls to Caracal dlls

## Tooling & DX
- cli interface similar to dotnet
- project files
- build.cara script similar to zig
- lsp
- formatter
- package manager
- documentation generator

## Testing, quality & profiling
- builtin test framework
- builtin code coverage
- fuzzing for the compiler
- tracy
