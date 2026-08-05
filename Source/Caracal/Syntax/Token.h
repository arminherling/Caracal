#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>
#include <Caracal/Syntax/TokenKind.h>

struct CARACAL_API Token
{
    TokenKind kind = TokenKind::Unknown;
    u16 fileId = 0;
    i32 index = -1;

    [[nodiscard]] static Token ToError(const Token& token) noexcept
    {
        return {
            .kind = TokenKind::Error,
            .fileId = token.fileId,
            .index = token.index
        };
    }
};

static_assert(sizeof(Token) == 8, "Token must stay a packed 8-byte handle");
