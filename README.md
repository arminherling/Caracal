# Caracal

> [!NOTE]
> Everything from syntax to semantics is currently work-in-progress and might change at any point.

## Description
Caracal is an imperative, compiled programming language designed with a focus on simple and enjoyable syntax with sensible defaults.
The goal is a statically typed language that feels like a dynamically typed language thanks to type inference.

## Features
- Static typing
- Type inference

## Roadmap
The current focus is on getting a minimal version up and running. For that, I'm generating C++ code, later on I'm probably gonna use LLVM.

- Lexer: mostly done
- Parser: basic syntax can be parsed but is missing advanced features
- Typechecker: none yet
- Optimizer: none yet
- Codegen: some code is already executable

## Examples

### Constants

The global scope can't contain variables, any identifier defined here is a constant and can't be changed at runtime.

```rb
year :: 2024;
currentOS :: OS.Windows;
```

### Functions

The global scope can also contain functions, they can be used to initialize constants.

```rb
def square(x: i32) 
{ 
    return x * x;
}

y :: square(10);
```

### Types

Types are the data objects of the language, they are similar to structs in other languages and can contain fields and methods. Members are accessed with a ``.`` , similar to ``this.`` in other languages.

```rb
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

### Enums

Enums are a way to define constant values, they work similar to C/C++.
The first member has a value of 0, and each successive member has a value one greater than the previous one, unless the value is manually assigned.

```rb
enum Values
{
    First
    Second :: 5
    Third
}

s :: Values.Second;
```

### Variants

Variants, also known as sum types or tagged unions, are Caracal's way to do polymorphism, they allow you to put different types behind a unified interface. They can be extended from multiple files.

```rb
variant V : Vector1D | Vector2D | Vector3D
{
    def distance()
    {
        case Vector1D d
        {
            return 1
        }
        case Vector2D d
        {
            return 2
        }
        case Vector3D d
        {
            return 3
        }
    }
}

extend variant V : Vector4D
{
    def distance()
    {
        case Vector4d d
        {
            return 4
        }
    }
}
```
