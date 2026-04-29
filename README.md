[![Windows-MSVC-Tests](https://github.com/arminherling/Caracal/actions/workflows/run-tests-windows-msvc.yaml/badge.svg)](https://github.com/arminherling/Caracal/actions/workflows/run-tests-windows-msvc.yaml) [![Windows-Clang-Tests](https://github.com/arminherling/Caracal/actions/workflows/run-tests-windows-clang.yaml/badge.svg)](https://github.com/arminherling/Caracal/actions/workflows/run-tests-windows-clang.yaml) [![Ubuntu-Clang-Tests](https://github.com/arminherling/Caracal/actions/workflows/run-tests-ubuntu-clang.yaml/badge.svg)](https://github.com/arminherling/Caracal/actions/workflows/run-tests-ubuntu-clang.yaml)
# Caracal

<img src="Assets/caracal-logo.png"  alt="Caracal logo" height="250" />

> [!NOTE]
> Everything from syntax to semantics is currently work-in-progress and might change at any point.

## Description
Caracal is an imperative, compiled programming language designed with a focus on simple and enjoyable syntax with sensible defaults.
The goal is a statically typed language that feels like a dynamically typed language thanks to type inference.

## Goals
- Good sensible defaults
- Powerful generics
- Low barrier for test integration
- Native compilation
- Maybe not too ugly syntax

## Roadmap
The current focus is on getting a minimal version up and running. For that, I'm generating LLVM IR.

- Lexer: mostly done
- Parser: basic syntax can be parsed but is missing advanced features
- Typechecker: some done
- Optimizer: none yet
- Codegen: some code is already executable

## Language overview
- [Examples](#examples)
- [Functions](#functions)
- [Constants](#constants)
- [Variables](#variables)
- [Types](#types)
- [Enums](#enums)
- [Variants](#variants)
- [Control flow](#control-flow)
  - [Return](#return)
  - [If](#if)
  - [While](#while)
  - [Skip](#skip)
  - [Break](#break)
  - [Trailing if](#trailing-if)
- [Operators](#operators)

## Examples
Some examples can be found under /Tests/TestData/Input here: [Link](Tests/TestData/Input)


## Functions
Caracal uses the ``def`` keyword to declare functions. Parameters start with the name and then the type, they are immutable by default.

```odin
def square(x: i32) 
{ 
    return x * x;
}
```
We can also declare external C functions with the ``extern`` annotation. This form also supports C's variadic parameters with ``...``.

```odin
#extern()
def puts(message: cstring) i32 {}

#extern()
def printf(message: cstring, ...) i32 {}
```


## Constants
To declare constants in Caracal, we use similar syntax to parameters, except we use double colons. They can't be changed during runtime as the name implies. Es can be declared in global scope or function scope.
The type is infered during compilation, but you can use an explicit type.

```odin
// infered type of OS
currentOS :: OS.Windows;

// explicit type of i32
year : i32 : 2026;
```


## Variables
Variables are similar to constants but they can be changed during runtime. We declare them with ``:=``. Unlike constants, variables can't be declared in global scope. We can also state the type explicitly,same as constants.

```odin
def getRandomNumber() i32
{
    // chosen by fair dice roll.
    // guaranteed to be random.
    x := 4;
    return x;
}
```


## Enums
Enums in Caracal are scoped constants with the same type, they work similar to C/C++.

```odin
enum Values
{
    First :: 1
    Second :: 2
    Third :: 3
}

s :: Values.Second;
```

If the enum has the ``autoIncrement`` annotation, then the first member starts with the value 0, and each successive member has a value one greater than the previous one, unless the value is manually assigned.

```odin
#autoIncrement()
enum Values
{
    First        // 0
    Second :: 5  // 5
    Third        // 6
}
```

The base type of the enum is infered by default but we can also use an explicit type.

```odin
enum Values : i32
{
    First :: 1
    Second :: 2
}
```

## Types
Types are the data objects of the language, they are similar to structs in other languages and can contain fields and methods. Members are accessed with a ``.`` , similar to ``this.`` in other languages.

```odin
type Two
{
    one :: 1
    
    def one() i32
    {
        return .one;
    }

    def value() i32
    {
        return .one() + .one();
    }
}
```

## Variants
Variants, also known as sum types or tagged unions, are Caracal's way to do polymorphism, they allow you to put different types behind a unified interface. They can be extended from multiple files.

## Control flow

### Return
Return works like in other languages, you can either specify what you want to return or just have it on it's own for functions that return nothing.

```odin
def five() i32
{
    return 5;
}

def nothing()
{
    return;
}
```


### If
If works like in other languages, except that the parenthesis around the condition aren't required. You can still add them if you want tho.

```odin
def func() i32
{ 
    // parenthesis around the condition are allowed but not required
    if (false)  
    {
        return 1;
    }
    else if true
    {
        return 2;
    }
    else
    {
        return 3;
    }
}
```


### While
While also works like in other languages, it loops until the condition isn't true anymore.

```odin
def stuff()
{
    i := 0;
    while i < 3
    {
        i = i + 1;
    }
}
```

### Skip
Skip works like ``continue`` in other languages, it skips the rest of the loop body and restarts the loop.

```odin
def stuff()
{
    i := 0;
    while i < 10
    {
        i = i + 1;
        if i > 5 and i < 7
        {
            skip;
        }
    }
}
```

### Break
Break works like in other languages, it breaks out of the loop and continues after the loop body.

```odin
def stuff()
{
    i := 0;
    while true
    {
        i = i + 1;
        if i >= 10
            break;
    }
}
```

### Trailing if
Caracal supports trailing if for return, break and skip statements. It is just syntax sugar but makes the code a bit nicer to read.

```odin
break if x > 10;

skip if i > 3;

return 0 if true;
```
vs
```odin
if x > 10
  break;

if i > 3
  skip;

if true
  return 0;
```


## Operators
...
