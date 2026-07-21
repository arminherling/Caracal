#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>

#include <string>

enum class TokenKind
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
    SingleQuote,
    Hash,

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

    EndOfFile
};

[[nodiscard]] CARACAL_API std::string stringify(TokenKind kind);
[[nodiscard]] CARACAL_API i32 unaryOperatorPrecedence(TokenKind kind);
[[nodiscard]] CARACAL_API i32 binaryOperatorPrecedence(TokenKind kind);
