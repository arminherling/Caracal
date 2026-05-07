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

        void AddError(const SourceTextSharedPtr& source, DiagnosticKind kind, const SourceLocation& location);
        void AddIllegalCharacterError(const SourceTextSharedPtr& source, const SourceLocation& location);
        void AddUnterminatedStringError(const SourceTextSharedPtr& source, const SourceLocation& location);
        void AddExpectedTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind expectedKind, TokenKind actualKind);
        void AddExpectedTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind expectedKind1, TokenKind expectedKind2, TokenKind actualKind);
        void AddExtraTokensRemainingError(const SourceTextSharedPtr& source, const SourceLocation& location);
        void AddExpectedEnumFieldError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind);
        void AddUnexpectedAnnotationError(const SourceTextSharedPtr& source, const SourceLocation& location);
        void AddUnknownAnnotationError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& annotationName, TokenKind targetKind);
        void AddAnnotationMissingArgumentsError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& annotationName);
        void AddAnnotationWrongNumberOfArgumentsError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& annotationName, i32 actualCount);
        void AddUnexpectedTopLevelTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind);
        void AddUnexpectedStatementTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind);
        void AddUnexpectedExpressionTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind);
        void AddUnknownNameError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name);
        void AddUnknownFunctionError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName);
        void AddUnknownTypeError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& typeName);
        void AddUnknownMethodError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& receiverTypeName, const std::string& methodName);
        void AddUnknownFieldError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& receiverTypeName, const std::string& fieldName);
        void AddArgumentCountMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName, i32 expectedCount, i32 actualCount, bool isVariadic);
        void AddArgumentTypeMismatchError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName, const std::vector<ArgumentTypeMismatchInfo>& mismatches);
        void AddInvalidVariadicArgumentTypeError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName, i32 argumentIndex, const std::string& actualTypeName);

        const std::vector<Diagnostic>& Diagnostics() const;

    private:
        std::vector<Diagnostic> diagnostics;
    };
}
