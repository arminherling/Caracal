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
    Equal,
    Semicolon,
    Underscore,
    Bang,

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
    WhileKeyword,
    ReturnKeyword,
    TrueKeyword,
    FalseKeyword,
    RefKeyword,
    CppKeyword,

    EndOfFile
};

COMPILER_API [[nodiscard]] QString Stringify(TokenKind kind, bool quoteStrings = false);
COMPILER_API [[nodiscard]] i32 unaryOperatorPrecedence(TokenKind kind);
COMPILER_API [[nodiscard]] i32 binaryOperatorPrecedence(TokenKind kind);
