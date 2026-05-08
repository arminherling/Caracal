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

    void DiagnosticsBag::AddIllegalCharacterError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::L0001_IllegalCharacter,
            source,
            location,
            "Remove the unsupported character.");
        diagnostic.addPrimaryLabel(location, "This character is not part of the Caracal syntax.");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnterminatedStringError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::L0002_UnterminatedString,
            source,
            location,
            "Add a closing quote to terminate the string.");
        diagnostic.addPrimaryLabel(location, "String literal is missing a closing quote.");

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
            DiagnosticKind::P0001_UnexpectedToken,
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
            DiagnosticKind::P0001_UnexpectedToken,
            source,
            location,
            fix);
        diagnostic.addPrimaryLabel(
            location,
            "Expected "
            + ToTokenSource(expectedKind1)
            + " or "
            + ToTokenSource(expectedKind2)
            + " but got "
            + ToTokenSource(actualKind));

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddExtraTokensRemainingError(const SourceTextSharedPtr& source, const SourceLocation& location)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::P0002_UnexpectedTrailingTokens,
            source,
            location);
        diagnostic.addPrimaryLabel(location, "Input should end before these trailing tokens.");

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
            DiagnosticKind::P0003_InvalidEnumField,
            source,
            location,
            fix);
        diagnostic.addPrimaryLabel(location, "Expected an enum field name, but got a " + ToTokenSource(actualKind));

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
            DiagnosticKind::P0006_UnexpectedTopLevelToken,
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
            DiagnosticKind::P0007_InvalidStatement,
            source,
            location,
            "Remove this token or rewrite the statement so it starts with a valid statement token.");
        diagnostic.addPrimaryLabel(location, "Unexpected token " + ToTokenSource(actualKind) + " in statement");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnexpectedExpressionTokenError(const SourceTextSharedPtr& source, const SourceLocation& location, TokenKind actualKind)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::P0008_InvalidExpression,
            source,
            location,
            "Remove this token or replace it with a valid expression.");
        diagnostic.addPrimaryLabel(location, "Unexpected token " + ToTokenSource(actualKind) + " in expression");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnexpectedAnnotationError(const SourceTextSharedPtr& source, const SourceLocation& location)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::P0004_UnexpectedAnnotation,
            source,
            location,
            "Remove the extra annotation or attach it to the next supported declaration.");
        diagnostic.addPrimaryLabel(location, "This annotation is not attached to a supported declaration.");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnknownAnnotationError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& annotationName, TokenKind targetKind)
    {
        auto fix = std::string{};
        switch (targetKind)
        {
            case TokenKind::DefKeyword:
                fix = "Rename this annotation to #extern, or remove it from the function.";
                break;
            case TokenKind::EnumKeyword:
                fix = "Rename this annotation to #flag or #step, or remove it from the enum.";
                break;
            default:
                fix = "Rename this annotation to a supported one, or remove it.";
                break;
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::P0005_UnknownAnnotation,
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
            fix = "Add the required annotation argument, for example (10).";
        }
        else
        {
            fix = "Add the required annotation argument.";
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0001_MissingAnnotationArguments,
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
                fix = "Add the missing annotation argument, for example (10).";
            }
            else
            {
                fix = "Reduce the annotation to one argument, for example (10).";
            }
        }
        else
        {
            fix = "Adjust the annotation argument count.";
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0002_WrongNumberOfAnnotationArguments,
            source,
            location,
            fix);
        diagnostic.addPrimaryLabel(location, "Annotation '#" + annotationName + "' expects 1 argument, but got " + std::to_string(actualCount) + ".");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnknownNameError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& name)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0003_UnknownName,
            source,
            location,
            "Declare this name before using it, or rename it to an existing symbol.");
        diagnostic.addPrimaryLabel(location, "Unknown name '" + name + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnknownFunctionError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& functionName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0004_UnknownFunction,
            source,
            location,
            "Declare this function before calling it, or rename the call to an existing function.");
        diagnostic.addPrimaryLabel(location, "Unknown function '" + functionName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddUnknownTypeError(const SourceTextSharedPtr& source, const SourceLocation& location, const std::string& typeName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0005_UnknownType,
            source,
            location,
            "Declare this type before using it, or rename it to an existing type.");
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
            DiagnosticKind::T0006_UnknownMethod,
            source,
            location,
            "Add this method to the type, or rename the call to an existing method.");
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
            DiagnosticKind::T0007_UnknownField,
            source,
            location,
            "Add this field to the type, or rename the access to an existing field.");
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
            fix = "Add the missing arguments to this call.";
        }
        else
        {
            fix = "Adjust this call to match the required argument count.";
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0008_ArgumentCountMismatch,
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
            fix = "Change the mismatched argument to match the parameter type.";
        }
        else
        {
            fix = "Change the mismatched arguments to match the parameter types.";
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0009_ArgumentTypeMismatch,
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
            DiagnosticKind::T0010_InvalidVariadicArgumentType,
            source,
            location,
            "Pass a non-void value as this variadic argument.");
        diagnostic.addPrimaryLabel(location, "Variadic argument " + std::to_string(argumentIndex) + " of '" + functionName + "' cannot have type '" + actualTypeName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddNonBoolIfConditionError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& actualTypeName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0011_NonBoolIfCondition,
            source,
            location,
            "Change the if condition to a bool expression.");
        diagnostic.addPrimaryLabel(location, "If condition must have type 'bool', but got '" + actualTypeName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddNonBoolWhileConditionError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& actualTypeName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0012_NonBoolWhileCondition,
            source,
            location,
            "Change the while condition to a bool expression.");
        diagnostic.addPrimaryLabel(location, "While condition must have type 'bool', but got '" + actualTypeName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddNonExternVariadicFunctionError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& functionName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0013_NonExternVariadicFunction,
            source,
            location,
            "Add #extern to this function, or remove the variadic parameter.");
        diagnostic.addPrimaryLabel(location, "Function '" + functionName + "' uses a variadic parameter but is not marked #extern");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddReturnTypeMismatchError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& expectedTypeName,
        const std::string& actualTypeName)
    {
        auto fix = std::string{};
        if (expectedTypeName == "void")
        {
            fix = "Remove the returned value from this return statement.";
        }
        else
        {
            fix = "Return a value of type '" + expectedTypeName + "' or change the function's return type.";
        }

        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0014_ReturnTypeMismatch,
            source,
            location,
            fix);
        diagnostic.addPrimaryLabel(location, "Cannot return '" + actualTypeName + "' from a function with return type '" + expectedTypeName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddAssignmentTypeMismatchError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& expectedTypeName,
        const std::string& actualTypeName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0015_AssignmentTypeMismatch,
            source,
            location,
            "Assign a value of type '" + expectedTypeName + "' or change the assignment target type.");
        diagnostic.addPrimaryLabel(location, "Cannot assign '" + actualTypeName + "' to a value of type '" + expectedTypeName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddExplicitConstantTypeMismatchError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& expectedTypeName,
        const std::string& actualTypeName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0016_ExplicitConstantTypeMismatch,
            source,
            location,
            "Initialize this constant with a value of type '" + expectedTypeName + "' or change its explicit type.");
        diagnostic.addPrimaryLabel(location, "Constant is declared as '" + expectedTypeName + "', but its initializer has type '" + actualTypeName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddExplicitVariableTypeMismatchError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& expectedTypeName,
        const std::string& actualTypeName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0017_ExplicitVariableTypeMismatch,
            source,
            location,
            "Initialize this variable with a value of type '" + expectedTypeName + "' or change its explicit type.");
        diagnostic.addPrimaryLabel(location, "Variable is declared as '" + expectedTypeName + "', but its initializer has type '" + actualTypeName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddTypeFieldInitializerMismatchError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& expectedTypeName,
        const std::string& actualTypeName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0018_TypeFieldInitializerMismatch,
            source,
            location,
            "Initialize this field with a value of type '" + expectedTypeName + "' or change the field type.");
        diagnostic.addPrimaryLabel(location, "Field is declared as '" + expectedTypeName + "', but its initializer has type '" + actualTypeName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddArithmeticOperandTypeMismatchError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& operatorName,
        const std::string& leftTypeName,
        const std::string& rightTypeName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0019_ArithmeticOperandTypeMismatch,
            source,
            location,
            "Use matching operand types for this arithmetic operation.");
        diagnostic.addPrimaryLabel(location, "Operator '" + operatorName + "' cannot be applied to '" + leftTypeName + "' and '" + rightTypeName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddComparisonOperandTypeMismatchError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& operatorName,
        const std::string& leftTypeName,
        const std::string& rightTypeName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0020_ComparisonOperandTypeMismatch,
            source,
            location,
            "Use comparable operand types for this comparison.");
        diagnostic.addPrimaryLabel(location, "Operator '" + operatorName + "' cannot compare '" + leftTypeName + "' and '" + rightTypeName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    void DiagnosticsBag::AddEnumFieldValueTypeMismatchError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        const std::string& expectedTypeName,
        const std::string& actualTypeName)
    {
        auto diagnostic = Diagnostic(
            DiagnosticLevel::Error,
            DiagnosticKind::T0021_EnumFieldValueTypeMismatch,
            source,
            location,
            "Use a value of the enum base type for this field.");
        diagnostic.addPrimaryLabel(location, "Enum base type is '" + expectedTypeName + "', but this field's initializer has type '" + actualTypeName + "'");

        diagnostics.push_back(std::move(diagnostic));
    }

    const std::vector<Diagnostic>& DiagnosticsBag::Diagnostics() const
    {
        return diagnostics;
    }
}
