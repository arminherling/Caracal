#pragma once

namespace Caracal
{
    enum class DiagnosticKind
    {
        Unknown,
        _0001_FoundIllegalCharacter,
        _0002_UnterminatedString,
        _0003_ExpectedXButGotY,
        _0004_ExtraTokensRemaining,
        _0005_ExpectedEnumField,
        _0006_UnexpectedAnnotation,
        _0007_UnknownAnnotation,
        _0008_AnnotationMissingArguments,
        _0009_AnnotationWrongNumberOfArguments,
    };
}
