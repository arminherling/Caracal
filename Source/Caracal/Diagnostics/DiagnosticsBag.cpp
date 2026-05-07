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
        auto fix = std::string{};
        if (actualKind == TokenKind::EndOfFile)
        {
            fix = "Add a " + ToTokenSource(expectedKind) + " before the end of file.";
        }
        else if (expectedKind == TokenKind::Semicolon)
        {
            fix = "Add a ';' at the end of the previous statement.";
        }
        else
        {
            fix = "Replace " + ToTokenSource(actualKind) + " with " + ToTokenSource(expectedKind) + ".";
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0003_UnexpectedToken,
            source,
            location,
            fix);
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
        auto fix = std::string{};
        if (actualKind == TokenKind::EndOfFile)
        {
            fix = "Add a " + ToTokenSource(expectedKind1) + " or " + ToTokenSource(expectedKind2) + " before the end of file.";
        }
        else
        {
            fix = "Replace "
                + ToTokenSource(actualKind)
                + " with "
                + ToTokenSource(expectedKind1)
                + " or "
                + ToTokenSource(expectedKind2)
                + ".";
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0003_UnexpectedToken,
            source,
            location,
            fix);
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
        auto fix = std::string{};
        if (actualKind == TokenKind::Number)
        {
            fix = "Start the enum field name with an identifier, for example 'FieldName' or 'FieldName :: 1'.";
        }
        else if (actualKind == TokenKind::CloseBracket || actualKind == TokenKind::EndOfFile)
        {
            fix = "Add an enum field name before the end of the enum body.";
        }
        else
        {
            fix = "Replace " + ToTokenSource(actualKind) + " with an enum field name such as 'FieldName'.";
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0005_InvalidEnumField,
            source,
            location,
            fix);
        diagnostic.addPrimaryLabel(location, "Expected an enum field name, but got a " + ToTokenSource(actualKind));

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
        auto fix = std::string{};
        switch (targetKind)
        {
            case TokenKind::DefKeyword:
                fix = "Rename '#" + annotationName + "' to #extern, or remove the annotation from the function.";
                break;
            case TokenKind::EnumKeyword:
                fix = "Rename '#" + annotationName + "' to #flag or #step, or remove the annotation from the enum.";
                break;
            default:
                fix = "Rename '#" + annotationName + "' to a supported annotation, or remove it.";
                break;
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0007_UnknownAnnotation,
            source,
            location,
            fix);
        diagnostic.addPrimaryLabel(location, "Unknown annotation '#" + annotationName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddAnnotationMissingArgumentsError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& annotationName)
    {
        auto fix = std::string{};
        if (annotationName == "step")
        {
            fix = "Add one argument to '#step', for example #step(10).";
        }
        else
        {
            fix = "Provide the required annotation arguments.";
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0008_MissingAnnotationArguments,
            source,
            location,
            fix);
        diagnostic.addPrimaryLabel(location, "Annotation '#" + annotationName + "' requires one argument.");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddAnnotationWrongNumberOfArgumentsError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& annotationName, i32 actualCount)
    {
        auto fix = std::string{};
        if (annotationName == "step")
        {
            if (actualCount == 0)
            {
                fix = "Add an argument to '#step', for example #step(10).";
            }
            else
            {
                fix = "Change '#step' to take exactly one argument, for example #step(10).";
            }
        }
        else
        {
            fix = "Adjust the annotation arguments to the expected count.";
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0009_WrongNumberOfAnnotationArguments,
            source,
            location,
            fix);
        diagnostic.addPrimaryLabel(location, "Annotation '#" + annotationName + "' expects 1 argument, but got " + std::to_string(actualCount) + ".");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnexpectedTopLevelTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind)
    {
        auto fix = std::string{};
        if (actualKind == TokenKind::CloseBracket)
        {
            fix = "Remove the stray '}' or move it inside a declaration body.";
        }
        else
        {
            fix = "Remove the unexpected top-level token or start a declaration with 'def', 'enum', or 'type'.";
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0010_UnexpectedTopLevelToken,
            source,
            location,
            fix);
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
            "Remove " + ToTokenSource(actualKind) + " or rewrite the statement so it starts with a valid statement token.");
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
            "Remove " + ToTokenSource(actualKind) + " or replace it with a valid expression.");
        diagnostic.addPrimaryLabel(location, "Unexpected token " + ToTokenSource(actualKind) + " in expression");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnknownNameError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0013_UnknownName,
            source,
            location,
            "Declare '" + name + "' before using it, or rename it to an existing symbol.");
        diagnostic.addPrimaryLabel(location, "Unknown name '" + name + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnknownFunctionError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0014_UnknownFunction,
            source,
            location,
            "Declare '" + functionName + "' before calling it, or rename the call to an existing function.");
        diagnostic.addPrimaryLabel(location, "Unknown function '" + functionName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnknownTypeError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& typeName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0015_UnknownType,
            source,
            location,
            "Declare type '" + typeName + "' before using it, or rename it to an existing type.");
        diagnostic.addPrimaryLabel(location, "Unknown type '" + typeName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnknownMethodError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& receiverTypeName,
        const std::string& methodName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0016_UnknownMethod,
            source,
            location,
            "Add method '" + methodName + "' to type '" + receiverTypeName + "', or rename the call to an existing method.");
        diagnostic.addPrimaryLabel(location, "Type '" + receiverTypeName + "' has no method '" + methodName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnknownFieldError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& receiverTypeName,
        const std::string& fieldName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0017_UnknownField,
            source,
            location,
            "Add field '" + fieldName + "' to type '" + receiverTypeName + "', or rename the access to an existing field.");
        diagnostic.addPrimaryLabel(location, "Type '" + receiverTypeName + "' has no field '" + fieldName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddArgumentCountMismatchError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& functionName,
        i32 expectedCount,
        i32 actualCount,
        bool isVariadic)
    {
        auto fix = std::string{};
        if (isVariadic)
        {
            fix = "Pass at least " + std::to_string(expectedCount) + " argument(s) to '" + functionName + "'.";
        }
        else
        {
            fix = "Pass exactly " + std::to_string(expectedCount) + " argument(s) to '" + functionName + "'.";
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0018_ArgumentCountMismatch,
            source,
            location,
            fix);

        if (isVariadic)
        {
            diagnostic.addPrimaryLabel(location, "Function '" + functionName + "' expects at least " + std::to_string(expectedCount) + " argument(s), but got " + std::to_string(actualCount));
        }
        else
        {
            diagnostic.addPrimaryLabel(location, "Function '" + functionName + "' expects " + std::to_string(expectedCount) + " argument(s), but got " + std::to_string(actualCount));
        }

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddArgumentTypeMismatchError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& functionName,
        const std::vector<ArgumentTypeMismatchInfo>& mismatches)
    {
        auto fix = std::string{};
        if (mismatches.size() == 1)
        {
            fix = "Change the mismatched argument in '" + functionName + "' to the expected parameter type.";
        }
        else
        {
            fix = "Change the mismatched arguments in '" + functionName + "' to the expected parameter types.";
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0019_ArgumentTypeMismatch,
            source,
            location,
            fix);

        for (const auto& mismatch : mismatches)
        {
            diagnostic.addPrimaryLabel(
                mismatch.location,
                "Argument " + std::to_string(mismatch.argumentIndex) + " of '" + functionName + "' expects '" + mismatch.expectedTypeName + "', but got '" + mismatch.actualTypeName + "'");
        }

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddInvalidVariadicArgumentTypeError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& functionName,
        i32 argumentIndex,
        const std::string& actualTypeName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::_0020_InvalidVariadicArgumentType,
            source,
            location,
            "Pass a non-void value as variadic argument " + std::to_string(argumentIndex) + " in '" + functionName + "'.");
        diagnostic.addPrimaryLabel(location, "Variadic argument " + std::to_string(argumentIndex) + " of '" + functionName + "' cannot have type '" + actualTypeName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    const std::vector<Diagnostic>& DiagnosticsBag::Diagnostics() const
    {
        return diagnostics;
    }
}
