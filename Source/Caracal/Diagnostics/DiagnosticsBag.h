#pragma once

#include <Caracal/API.h>
#include <Caracal/Diagnostics/ArgumentTypeMismatchInfo.h>
#include <Caracal/Diagnostics/Diagnostic.h>
#include <Caracal/Semantic/AnnotationKind.h>
#include <Caracal/Syntax/TokenKind.h>

#include <vector>

namespace Caracal
{
    class CARACAL_API DiagnosticsBag
    {
    public:
        DiagnosticsBag() = default;

        // Lexer diagnostics
        void addIllegalCharacterError(const SourceTextSharedPtr& source, const SourceLocation& location);
        void addUnterminatedStringError(const SourceTextSharedPtr& source, const SourceLocation& location);

        // Parser syntax diagnostics
        void addExpectedTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind expectedKind, TokenKind actualKind);
        void addExpectedTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind expectedKind1, TokenKind expectedKind2, TokenKind actualKind);
        void addExtraTokensRemainingError(const SourceTextSharedPtr& source, const SourceLocation& location);
        void addUnexpectedTopLevelTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind);
        void addExpectedEnumFieldError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind);
        void addUnexpectedStatementTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind);
        void addUnexpectedExpressionTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind);

        // Annotation diagnostics
        void addDanglingAnnotationError(const SourceTextSharedPtr& source, const SourceLocation& location);
        void addUnknownAnnotationError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& annotationName, TokenKind targetKind);
        void addUnexpectedAnnotationTargetError(const SourceTextSharedPtr& source, const SourceLocation& location);
        void addAnnotationMissingArgumentsError(const SourceTextSharedPtr& source, const SourceLocation& location, AnnotationKind annotationKind, const std::string& annotationName);
        void addAnnotationWrongNumberOfArgumentsError(const SourceTextSharedPtr& source, const SourceLocation& location, AnnotationKind annotationKind, const std::string& annotationName, i32 expectedCount, i32 actualCount);
        void addAnnotationArgumentTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, AnnotationKind annotationKind, const std::string& annotationName, const std::string& expectedDescription, const std::string& actualDescription);
        void addConflictingEnumAnnotationsError(const SourceTextSharedPtr& source, const SourceLocation& location, const SourceLocation& otherLocation, const std::string& annotationName, const std::string& otherAnnotationName);

        // Unknown symbol and member access diagnostics
        void addUnknownNameError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name);
        void addUnknownFunctionError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName);
        void addUnknownTypeError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& typeName);
        void addUnknownMethodError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& receiverTypeName, const std::string& methodName);
        void addUnknownFieldError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& receiverTypeName, const std::string& fieldName);
        void addUnknownEnumFieldError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& enumName, const std::string& fieldName);
        void addInvalidEnumMemberAccessError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& enumName);
        void addInvalidMemberAccessReceiverError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& receiverTypeName);

        // Call diagnostics
        void addArgumentCountMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName, i32 expectedCount, i32 actualCount, bool isVariadic);
        void addArgumentTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName, const std::vector<ArgumentTypeMismatchInfo>& mismatches);
        void addInvalidVariadicArgumentTypeError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName, i32 argumentIndex, const std::string& actualTypeName);

        // Control flow diagnostics
        void addNonBoolIfConditionError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& actualTypeName);
        void addNonBoolWhileConditionError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& actualTypeName);

        // Type mismatch diagnostics
        void addReturnTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& expectedTypeName, const std::string& actualTypeName);
        void addAssignmentTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& expectedTypeName, const std::string& actualTypeName);
        void addExplicitConstantTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& expectedTypeName, const std::string& actualTypeName);
        void addExplicitVariableTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& expectedTypeName, const std::string& actualTypeName);
        void addTypeFieldInitializerMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& expectedTypeName, const std::string& actualTypeName);
        void addArithmeticOperandTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& operatorName, const std::string& leftTypeName, const std::string& rightTypeName);
        void addComparisonOperandTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& operatorName, const std::string& leftTypeName, const std::string& rightTypeName);
        void addEnumFieldValueTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& expectedTypeName, const std::string& actualTypeName);

        // Declaration-shape and reference diagnostics
        void addNonExternVariadicFunctionError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName);
        void addFlagEnumExplicitValueError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& fieldName);
        void addExplicitConstructorDeclarationError(const SourceTextSharedPtr& source, const SourceLocation& location, const SourceLocation& otherLocation, const std::string& typeName);
        void addReferenceReturnTypeError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& typeName);
        void addAlreadyReferenceError(const SourceTextSharedPtr& source, const SourceLocation& location);

        // Duplicate declaration diagnostics
        void addDuplicateDeclarationError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name, const SourceTextSharedPtr& otherSource = nullptr, std::optional<SourceLocation> otherLocation = std::nullopt);
        void addDuplicateConstantDeclarationError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name, const SourceTextSharedPtr& otherSource = nullptr, std::optional<SourceLocation> otherLocation = std::nullopt);
        void addDuplicateVariableDeclarationError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name, const SourceTextSharedPtr& otherSource = nullptr, std::optional<SourceLocation> otherLocation = std::nullopt);
        void addDuplicateParameterDeclarationError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name, const SourceTextSharedPtr& otherSource = nullptr, std::optional<SourceLocation> otherLocation = std::nullopt);
        void addDuplicateEnumFieldDeclarationError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name, std::optional<SourceLocation> otherLocation = std::nullopt);
        void addDuplicateTypeFieldDeclarationError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name, std::optional<SourceLocation> otherLocation = std::nullopt);
        void addDuplicateFunctionDeclarationError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName, const SourceTextSharedPtr& otherSource = nullptr, std::optional<SourceLocation> otherLocation = std::nullopt);
        void addDuplicateTypeDeclarationError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& typeName, const SourceTextSharedPtr& otherSource = nullptr, std::optional<SourceLocation> otherLocation = std::nullopt);

        // Literal diagnostics
        void addNumberLiteralOutOfRangeError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& literalText, const std::string& targetTypeName);

        // Warning diagnostics
        void addUnusedLocalVariableWarning(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name);
        void addUnusedParameterWarning(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name);
        void addUnusedLocalConstantWarning(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name);

        const std::vector<Diagnostic>& diagnostics() const;
        [[nodiscard]] bool hasErrors() const noexcept;
        [[nodiscard]] bool hasWarnings() const noexcept;

    private:
        std::vector<Diagnostic> m_diagnostics;
    };
}
