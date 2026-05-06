#pragma once

#include <Caracal/API.h>
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

        const std::vector<Diagnostic>& Diagnostics() const;

    private:
        std::vector<Diagnostic> diagnostics;
    };
}
