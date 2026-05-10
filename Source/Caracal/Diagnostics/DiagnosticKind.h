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
        P0003_UnexpectedTopLevelToken,
        P0004_InvalidEnumField,
        P0005_InvalidStatement,
        P0006_InvalidExpression,
        P0007_DanglingAnnotation,

        // Annotation diagnostics
        T0001_MissingAnnotationArguments,
        T0002_WrongNumberOfAnnotationArguments,
        T0003_AnnotationArgumentTypeMismatch,
        T0004_UnexpectedAnnotationTarget,
        T0005_ConflictingEnumAnnotations,

        // Unknown symbol diagnostics
        T0006_UnknownAnnotation,
        T0007_UnknownName,
        T0008_UnknownFunction,
        T0009_UnknownType,
        T0010_UnknownMethod,
        T0011_UnknownField,

        // Call diagnostics
        T0012_ArgumentCountMismatch,
        T0013_ArgumentTypeMismatch,
        T0014_InvalidVariadicArgumentType,

        // Control flow and declaration-shape diagnostics
        T0015_NonBoolIfCondition,
        T0016_NonBoolWhileCondition,
        T0017_NonExternVariadicFunction,

        // Type mismatch diagnostics
        T0018_ReturnTypeMismatch,
        T0019_AssignmentTypeMismatch,
        T0020_ExplicitConstantTypeMismatch,
        T0021_ExplicitVariableTypeMismatch,
        T0022_TypeFieldInitializerMismatch,
        T0023_ArithmeticOperandTypeMismatch,
        T0024_ComparisonOperandTypeMismatch,
        T0025_EnumFieldValueTypeMismatch,
        T0026_DuplicateDeclaration,
        T0027_FlagEnumExplicitValue,
        T0028_ReferenceReturnType,
        T0029_ExplicitConstructorDeclaration,
        T0030_AlreadyReference,
        T0031_DuplicateTypeDeclaration,
        T0032_DuplicateFunctionDeclaration,
    };
}
