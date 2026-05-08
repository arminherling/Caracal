#pragma once

namespace Caracal
{
    enum class DiagnosticKind
    {
        Unknown,

        // Lexer diagnostics
        L0001_IllegalCharacter,
        L0002_UnterminatedString,

        // Parser syntax diagnostics
        P0001_UnexpectedToken,
        P0002_UnexpectedTrailingTokens,
        P0003_InvalidEnumField,
        P0006_UnexpectedTopLevelToken,
        P0007_InvalidStatement,
        P0008_InvalidExpression,

        // Annotation diagnostics
        P0004_UnexpectedAnnotation,
        P0005_UnknownAnnotation,
        T0001_MissingAnnotationArguments,
        T0002_WrongNumberOfAnnotationArguments,

        // Unknown symbol diagnostics
        T0003_UnknownName,
        T0004_UnknownFunction,
        T0005_UnknownType,
        T0006_UnknownMethod,
        T0007_UnknownField,

        // Call diagnostics
        T0008_ArgumentCountMismatch,
        T0009_ArgumentTypeMismatch,
        T0010_InvalidVariadicArgumentType,

        // Control flow and declaration-shape diagnostics
        T0011_NonBoolIfCondition,
        T0012_NonBoolWhileCondition,
        T0013_NonExternVariadicFunction,

        // Type mismatch diagnostics
        T0014_ReturnTypeMismatch,
        T0015_AssignmentTypeMismatch,
        T0016_ExplicitConstantTypeMismatch,
        T0017_ExplicitVariableTypeMismatch,
        T0018_TypeFieldInitializerMismatch,
        T0019_ArithmeticOperandTypeMismatch,
        T0020_ComparisonOperandTypeMismatch,
        T0021_EnumFieldValueTypeMismatch,
    };
}
