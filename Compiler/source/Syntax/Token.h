#pragma once

#include <Defines.h>
#include <Compiler/API.h>
#include <Syntax/TokenKind.h>

struct COMPILER_API Token
{
    TokenKind kind = TokenKind::Unknown;
    i32 lexemeIndex = -1;
    i32 locationIndex = -1;
    i32 triviaIndex = -1;

    [[nodiscard]] static Token ToError(const Token& token) noexcept
    {
        return { .kind = TokenKind::Error, .locationIndex = token.locationIndex };
    }
};
