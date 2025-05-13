#pragma once

namespace Caracal
{
    enum class DiagnosticKind
    {
        Unknown,
        _0001_FoundIllegalCharacter,
        _0002_UnterminatedString,
        _0003_ExpectedXButGotY,
    };
}
