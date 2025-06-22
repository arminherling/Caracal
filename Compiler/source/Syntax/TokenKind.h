#pragma once

#include <Defines.h>
#include <Compiler/API.h>

#include <QString>

enum class COMPILER_API TokenKind
{
    Unknown,
    Error,

    Plus,
    Minus,
    Star,
    Slash,
    Dot,
    Comma,
    Colon,
    Semicolon,
    Underscore,
    Uptick,

    Equal,
    EqualEqual,
    Bang,
    BangEqual,
    LessThan,
    LessThanEqual,
    GreaterThan,
    GreaterThanEqual,

    OpenParenthesis,
    CloseParenthesis,
    OpenBracket,
    CloseBracket,

    Identifier,
    Number,
    String,

    DefKeyword,
    EnumKeyword,
    TypeKeyword,
    IfKeyword,
    ElseKeyword,
    WhileKeyword,
    BreakKeyword,
    SkipKeyword,
    ReturnKeyword,
    TrueKeyword,
    FalseKeyword,
    AndKeyword,
    OrKeyword,
    RefKeyword,
    CppKeyword,

    EndOfFile
};

[[nodiscard]] COMPILER_API QString Stringify(TokenKind kind);
[[nodiscard]] COMPILER_API i32 unaryOperatorPrecedence(TokenKind kind);
[[nodiscard]] COMPILER_API i32 binaryOperatorPrecedence(TokenKind kind);
