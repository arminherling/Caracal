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
        P0008_UninitializedField,
        P0009_PositionalArgumentAfterNamed,
        P0010_AnnotationNotAllowedHere,
        P0011_InvalidParameter,

        // Annotation diagnostics
        T0001_UnknownAnnotation,
        T0002_UnexpectedAnnotationTarget,
        T0003_MissingAnnotationArguments,
        T0004_WrongNumberOfAnnotationArguments,
        T0005_AnnotationArgumentTypeMismatch,
        T0006_ConflictingEnumAnnotations,

        // Unknown symbol and member access diagnostics
        T0007_UnknownName,
        T0008_UnknownFunction,
        T0009_UnknownType,
        T0010_UnknownMethod,
        T0011_UnknownField,
        T0012_UnknownEnumField,
        T0013_InvalidEnumMemberAccess,
        T0014_InvalidMemberAccessReceiver,

        // Call diagnostics
        T0015_ArgumentCountMismatch,
        T0016_ArgumentTypeMismatch,
        T0017_InvalidVariadicArgumentType,

        // Control flow diagnostics
        T0018_NonBoolIfCondition,
        T0019_NonBoolWhileCondition,

        // Type mismatch diagnostics
        T0020_ReturnTypeMismatch,
        T0021_AssignmentTypeMismatch,
        T0022_ExplicitConstantTypeMismatch,
        T0023_ExplicitVariableTypeMismatch,
        T0024_TypeFieldInitializerMismatch,
        T0025_ArithmeticOperandTypeMismatch,
        T0026_ComparisonOperandTypeMismatch,
        T0027_EnumFieldValueTypeMismatch,

        // Declaration-shape and reference diagnostics
        T0028_NonExternVariadicFunction,
        T0029_FlagEnumExplicitValue,
        T0030_TypeDotNewDeclaration,
        T0031_ReferenceReturnType,
        T0032_AlreadyReference,

        // Duplicate declaration diagnostics
        T0033_DuplicateDeclaration,
        T0034_DuplicateConstantDeclaration,
        T0035_DuplicateVariableDeclaration,
        T0036_DuplicateParameterDeclaration,
        T0037_DuplicateEnumFieldDeclaration,
        T0038_DuplicateTypeFieldDeclaration,
        T0039_DuplicateFunctionDeclaration,
        T0040_DuplicateTypeDeclaration,

        // Literal diagnostics
        T0041_NumberLiteralOutOfRange,

        // Warning diagnostics
        T0042_UnusedLocalVariable,
        T0043_UnusedLocalConstant,
        T0044_UnusedParameter,

        // Annotation argument diagnostics
        T0045_UnexpectedAnnotationArgument,
        T0046_DuplicateAnnotationArgument,

        // Extern diagnostics
        T0047_ExternMethodRequiresSymbol,

        // Call argument binding diagnostics
        T0048_UnknownArgumentName,
        T0049_DuplicateArgumentBinding,
        T0050_MissingRequiredArgument,

        // Default parameter diagnostics
        T0051_DefaultParameterTypeMismatch,
        T0052_NonTrailingDefaultParameter,

        // init (write-once) constant diagnostics
        T0053_AssignmentToInitConstant,
        T0054_InitConstantAlreadyInitialized,
        T0055_UninitializedInitConstant,
        T0056_NonGlobalInitConstant,

        // Constness diagnostics
        T0057_AssignmentToConstant,
        T0058_AssignmentThroughConstantReference,

        // Control-flow diagnostics
        T0059_UnreachableCode,
        T0060_MissingReturn,

        // Arithmetic diagnostics
        T0061_DivisionByZero,
        T0062_ConstantOverflow,
    };
}
