#include <Caracal/Diagnostics/DiagnosticsBag.h>

#include <unordered_map>

namespace Caracal
{
    static std::string ToTokenSource(TokenKind kind)
    {
        const std::unordered_map<TokenKind, std::string_view> tokenSource{
            { TokenKind::Error,             std::string_view("error token") },

            { TokenKind::Plus,              std::string_view("'+'") },
            { TokenKind::Minus,             std::string_view("'-'") },
            { TokenKind::Star,              std::string_view("'*'") },
            { TokenKind::Slash,             std::string_view("'/'") },
            { TokenKind::Dot,               std::string_view("'.'") },
            { TokenKind::Comma,             std::string_view("','") },
            { TokenKind::Colon,             std::string_view("':'") },
            { TokenKind::Semicolon,         std::string_view("';'") },
            { TokenKind::Underscore,        std::string_view("'_'") },
            { TokenKind::Uptick,            std::string_view("'`'") },
            { TokenKind::Hash,              std::string_view("'#'") },

            { TokenKind::Equal,             std::string_view("'='") },
            { TokenKind::EqualEqual,        std::string_view("'=='") },
            { TokenKind::Bang,              std::string_view("'!'") },
            { TokenKind::BangEqual,         std::string_view("'!='") },
            { TokenKind::LessThan,          std::string_view("'<'") },
            { TokenKind::LessThanEqual,     std::string_view("'<='") },
            { TokenKind::GreaterThan,       std::string_view("'>'") },
            { TokenKind::GreaterThanEqual,  std::string_view("'>='") },

            { TokenKind::OpenParenthesis,   std::string_view("'('") },
            { TokenKind::CloseParenthesis,  std::string_view("')'") },
            { TokenKind::OpenBracket,       std::string_view("'{'") },
            { TokenKind::CloseBracket,      std::string_view("'}'") },

            { TokenKind::Identifier,        std::string_view("identifier") },
            { TokenKind::Number,            std::string_view("number") },
            { TokenKind::String,            std::string_view("string") },

            { TokenKind::DefKeyword,        std::string_view("'def'") },
            { TokenKind::EnumKeyword,       std::string_view("'enum'") },
            { TokenKind::TypeKeyword,       std::string_view("'type'") },
            { TokenKind::IfKeyword,         std::string_view("'if'") },
            { TokenKind::ElseKeyword,       std::string_view("'else'") },
            { TokenKind::WhileKeyword,      std::string_view("'while'") },
            { TokenKind::BreakKeyword,      std::string_view("'break'") },
            { TokenKind::SkipKeyword,       std::string_view("'skip'") },
            { TokenKind::ReturnKeyword,     std::string_view("'return'") },
            { TokenKind::TrueKeyword,       std::string_view("'true'") },
            { TokenKind::FalseKeyword,      std::string_view("'false'") },
            { TokenKind::AndKeyword,        std::string_view("'and'") },
            { TokenKind::OrKeyword,         std::string_view("'or'") },
            { TokenKind::RefKeyword,        std::string_view("'ref'") },

            { TokenKind::EndOfFile,         std::string_view("end of file") },
        };

        const auto it = tokenSource.find(kind);
        if (it != tokenSource.end())
            return std::string(it->second);

        return stringify(kind);
    }

    static std::string ToExpectedTokenFix(TokenKind expectedKind, TokenKind actualKind)
    {
        if (actualKind == TokenKind::EndOfFile)
        {
            return "Add a " + ToTokenSource(expectedKind) + " before the end of file.";
        }
        if (expectedKind == TokenKind::Semicolon)
        {
            return "Add a ';' at the end of the previous statement.";
        }

        return "Replace " + ToTokenSource(actualKind) + " with " + ToTokenSource(expectedKind) + ".";
    }

    static std::string ToExpectedTokenFix(TokenKind expectedKind1, TokenKind expectedKind2, TokenKind actualKind)
    {
        if (actualKind == TokenKind::EndOfFile)
        {
            return "Add a " + ToTokenSource(expectedKind1) + " or " + ToTokenSource(expectedKind2) + " before the end of file.";
        }

        return "Replace "
            + ToTokenSource(actualKind)
            + " with "
            + ToTokenSource(expectedKind1)
            + " or "
            + ToTokenSource(expectedKind2)
            + ".";
    }

    static std::string ToExpectedEnumFieldMessage(TokenKind actualKind)
    {
        return "Expected an enum field name, but got a " + ToTokenSource(actualKind);
    }

    static std::string ToExpectedEnumFieldFix(TokenKind actualKind)
    {
        if (actualKind == TokenKind::Number)
        {
            return "Start the enum field name with an identifier, for example 'FieldName' or 'FieldName :: 1'.";
        }

        if (actualKind == TokenKind::CloseBracket || actualKind == TokenKind::EndOfFile)
        {
            return "Add an enum field name before the end of the enum body.";
        }

        return "Replace " + ToTokenSource(actualKind) + " with an enum field name such as 'FieldName'.";
    }

    static std::string ToUnknownAnnotationFix(const std::string& annotationName, TokenKind targetKind)
    {
        switch (targetKind)
        {
            case TokenKind::DefKeyword:
                return "Rename '#" + annotationName + "' to #extern, or remove the annotation from the function.";
            case TokenKind::EnumKeyword:
                return "Rename '#" + annotationName + "' to #flag or #step, or remove the annotation from the enum.";
            default:
                return "Rename '#" + annotationName + "' to a supported annotation, or remove it.";
        }
    }

    static std::string ToAnnotationMissingArgumentsFix(const std::string& annotationName)
    {
        if (annotationName == "step")
        {
            return "Add one argument to '#step', for example #step(10).";
        }

        return "Provide the required annotation arguments.";
    }

    static std::string ToAnnotationWrongNumberOfArgumentsFix(const std::string& annotationName, i32 actualCount)
    {
        if (annotationName == "step")
        {
            if (actualCount == 0)
            {
                return "Add an argument to '#step', for example #step(10).";
            }

            return "Change '#step' to take exactly one argument, for example #step(10).";
        }

        return "Adjust the annotation arguments to the expected count.";
    }

    static std::string ToUnexpectedTopLevelTokenFix(TokenKind actualKind)
    {
        if (actualKind == TokenKind::CloseBracket)
        {
            return "Remove the stray '}' or move it inside a declaration body.";
        }

        return "Remove the unexpected top-level token or start a declaration with 'def', 'enum', or 'type'.";
    }

    static std::string ToUnexpectedStatementTokenFix(TokenKind actualKind)
    {
        return "Remove " + ToTokenSource(actualKind) + " or rewrite the statement so it starts with a valid statement token.";
    }

    static std::string ToUnexpectedExpressionTokenFix(TokenKind actualKind)
    {
        return "Remove " + ToTokenSource(actualKind) + " or replace it with a valid expression.";
    }

    void DiagnosticsBag::AddError(const SourceTextSharedPtr& source, DiagnosticKind kind, const SourceLocation& location)
    {
        auto diagnostic = Diagnostic(DiagnosticLevel::Error, kind, source, location);
        diagnostic.addPrimaryLabel(location, diagnostic.message());

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddIllegalCharacterError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0001_IllegalCharacter,
            source,
            location,
            "Remove the unsupported character.");
        diagnostic.addPrimaryLabel(location, diagnostic.message());

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnterminatedStringError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0002_UnterminatedString,
            source,
            location,
            "Add a closing quote to terminate the string.");
        diagnostic.addPrimaryLabel(location, diagnostic.message());

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddExpectedTokenError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        TokenKind expectedKind,
        TokenKind actualKind)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0003_UnexpectedToken,
            source,
            location,
            ToExpectedTokenFix(expectedKind, actualKind));
        diagnostic.addPrimaryLabel(location, "Expected " + ToTokenSource(expectedKind) + " but got " + ToTokenSource(actualKind));

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddExpectedTokenError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        TokenKind expectedKind1,
        TokenKind expectedKind2,
        TokenKind actualKind)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0003_UnexpectedToken,
            source,
            location,
            ToExpectedTokenFix(expectedKind1, expectedKind2, actualKind));
        diagnostic.addPrimaryLabel(
            location,
            "Expected "
            + ToTokenSource(expectedKind1)
            + " or "
            + ToTokenSource(expectedKind2)
            + "but got "
            + ToTokenSource(actualKind));

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddExtraTokensRemainingError(const SourceTextSharedPtr& source, const SourceLocation& location)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0004_UnexpectedTrailingTokens,
            source,
            location);
        diagnostic.addPrimaryLabel(location, diagnostic.message());

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddExpectedEnumFieldError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0005_InvalidEnumField,
            source,
            location,
            ToExpectedEnumFieldFix(actualKind));
        diagnostic.addPrimaryLabel(location, ToExpectedEnumFieldMessage(actualKind));

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnexpectedAnnotationError(const SourceTextSharedPtr& source, const SourceLocation& location)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0006_UnexpectedAnnotation,
            source,
            location,
            "Remove the extra annotation or attach it to the next supported declaration.");
        diagnostic.addPrimaryLabel(location, diagnostic.message());

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnknownAnnotationError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& annotationName, TokenKind targetKind)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0007_UnknownAnnotation,
            source,
            location,
            ToUnknownAnnotationFix(annotationName, targetKind));
        diagnostic.addPrimaryLabel(location, "Unknown annotation '#" + annotationName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddAnnotationMissingArgumentsError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& annotationName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0008_MissingAnnotationArguments,
            source,
            location,
            ToAnnotationMissingArgumentsFix(annotationName));
        diagnostic.addPrimaryLabel(location, "Annotation '#" + annotationName + "' requires one argument.");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddAnnotationWrongNumberOfArgumentsError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& annotationName, i32 actualCount)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0009_WrongNumberOfAnnotationArguments,
            source,
            location,
            ToAnnotationWrongNumberOfArgumentsFix(annotationName, actualCount));
        diagnostic.addPrimaryLabel(location, "Annotation '#" + annotationName + "' expects 1 argument, but got " + std::to_string(actualCount) + ".");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnexpectedTopLevelTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0010_UnexpectedTopLevelToken,
            source,
            location,
            ToUnexpectedTopLevelTokenFix(actualKind));
        diagnostic.addPrimaryLabel(location, "Unexpected top-level token " + ToTokenSource(actualKind));

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnexpectedStatementTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0011_InvalidStatement,
            source,
            location,
            ToUnexpectedStatementTokenFix(actualKind));
        diagnostic.addPrimaryLabel(location, "Unexpected token " + ToTokenSource(actualKind) + " in statement");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnexpectedExpressionTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0012_InvalidExpression,
            source,
            location,
            ToUnexpectedExpressionTokenFix(actualKind));
        diagnostic.addPrimaryLabel(location, "Unexpected token " + ToTokenSource(actualKind) + " in expression");

        diagnostics.push_back(std::move(diagnostic));
    }

    const std::vector<Diagnostic>& DiagnosticsBag::Diagnostics() const
    {
        return diagnostics;
    }
}
