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
    Colon,
    DoubleColon,
    Comma,
    Equal,
    ColonEqual,
    Underscore,
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
COMPILER_API [[nodiscard]] i32 UnaryOperatorPrecedence(TokenKind kind);
COMPILER_API [[nodiscard]] i32 BinaryOperatorPrecedence(TokenKind kind);
