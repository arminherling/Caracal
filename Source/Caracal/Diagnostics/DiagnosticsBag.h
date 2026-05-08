#pragma once

#include <Caracal/API.h>
#include <Caracal/Diagnostics/ArgumentTypeMismatchInfo.h>
#include <Caracal/Diagnostics/Diagnostic.h>
#include <Caracal/Syntax/TokenKind.h>

#include <vector>

namespace Caracal
{
    class CARACAL_API DiagnosticsBag
    {
    public:
        DiagnosticsBag() = default;

        // Lexer diagnostics
        void AddIllegalCharacterError(const SourceTextSharedPtr& source, const SourceLocation& location);
        void AddUnterminatedStringError(const SourceTextSharedPtr& source, const SourceLocation& location);

        // Parser syntax diagnostics
        void AddExpectedTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind expectedKind, TokenKind actualKind);
        void AddExpectedTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind expectedKind1, TokenKind expectedKind2, TokenKind actualKind);
        void AddExtraTokensRemainingError(const SourceTextSharedPtr& source, const SourceLocation& location);
        void AddExpectedEnumFieldError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind);
        void AddUnexpectedTopLevelTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind);
        void AddUnexpectedStatementTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind);
        void AddUnexpectedExpressionTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind);

        // Annotation diagnostics
        void AddUnexpectedAnnotationError(const SourceTextSharedPtr& source, const SourceLocation& location);
        void AddUnknownAnnotationError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& annotationName, TokenKind targetKind);
        void AddAnnotationMissingArgumentsError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& annotationName);
        void AddAnnotationWrongNumberOfArgumentsError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& annotationName, i32 actualCount);

        // Unknown symbol diagnostics
        void AddUnknownNameError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name);
        void AddUnknownFunctionError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName);
        void AddUnknownTypeError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& typeName);
        void AddUnknownMethodError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& receiverTypeName, const std::string& methodName);
        void AddUnknownFieldError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& receiverTypeName, const std::string& fieldName);

        // Call diagnostics
        void AddArgumentCountMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName, i32 expectedCount, i32 actualCount, bool isVariadic);
        void AddArgumentTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName, const std::vector<ArgumentTypeMismatchInfo>& mismatches);
        void AddInvalidVariadicArgumentTypeError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName, i32 argumentIndex, const std::string& actualTypeName);

        // Control flow and declaration-shape diagnostics
        void AddNonBoolIfConditionError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& actualTypeName);
        void AddNonBoolWhileConditionError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& actualTypeName);
        void AddNonExternVariadicFunctionError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName);

        // Type mismatch diagnostics
        void AddReturnTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& expectedTypeName, const std::string& actualTypeName);
        void AddAssignmentTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& expectedTypeName, const std::string& actualTypeName);
        void AddExplicitConstantTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& expectedTypeName, const std::string& actualTypeName);
        void AddExplicitVariableTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& expectedTypeName, const std::string& actualTypeName);
        void AddTypeFieldInitializerMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& expectedTypeName, const std::string& actualTypeName);
        void AddArithmeticOperandTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& operatorName, const std::string& leftTypeName, const std::string& rightTypeName);
        void AddComparisonOperandTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& operatorName, const std::string& leftTypeName, const std::string& rightTypeName);
        void AddEnumFieldValueTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& expectedTypeName, const std::string& actualTypeName);

        const std::vector<Diagnostic>& Diagnostics() const;

    private:
        std::vector<Diagnostic> diagnostics;
    };
}
