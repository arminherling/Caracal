#pragma once

namespace Caracal
{
    enum class DiagnosticKind
    {
        Unknown,
        _0001_IllegalCharacter,
        _0002_UnterminatedString,
        _0003_UnexpectedToken,
        _0004_UnexpectedTrailingTokens,
        _0005_InvalidEnumField,
        _0006_UnexpectedAnnotation,
        _0007_UnknownAnnotation,
        _0008_MissingAnnotationArguments,
        _0009_WrongNumberOfAnnotationArguments,
        _0010_UnexpectedTopLevelToken,
        _0011_InvalidStatement,
        _0012_InvalidExpression,
        _0013_UnknownName,
        _0014_UnknownFunction,
        _0015_UnknownType,
        _0016_UnknownMethod,
        _0017_UnknownField,
        _0018_ArgumentCountMismatch,
        _0019_ArgumentTypeMismatch,
        _0020_InvalidVariadicArgumentType,
    };
}
