#include <Caracal/Diagnostics/Diagnostic.h>

namespace Caracal
{
    static std::string_view DiagnosticCode(DiagnosticKind kind)
    {
        switch (kind)
        {
            // Lexer diagnostics
            case DiagnosticKind::L0001_IllegalCharacter:
                return "L0001";
            case DiagnosticKind::L0002_UnterminatedString:
                return "L0002";

            // Parser syntax diagnostics
            case DiagnosticKind::P0001_UnexpectedToken:
                return "P0001";
            case DiagnosticKind::P0002_UnexpectedTrailingTokens:
                return "P0002";
            case DiagnosticKind::P0003_InvalidEnumField:
                return "P0003";
            case DiagnosticKind::P0006_UnexpectedTopLevelToken:
                return "P0006";
            case DiagnosticKind::P0007_InvalidStatement:
                return "P0007";
            case DiagnosticKind::P0008_InvalidExpression:
                return "P0008";

            // Annotation diagnostics
            case DiagnosticKind::P0004_UnexpectedAnnotation:
                return "P0004";
            case DiagnosticKind::P0005_UnknownAnnotation:
                return "P0005";
            case DiagnosticKind::T0001_MissingAnnotationArguments:
                return "T0001";
            case DiagnosticKind::T0002_WrongNumberOfAnnotationArguments:
                return "T0002";

            // Unknown symbol diagnostics
            case DiagnosticKind::T0003_UnknownName:
                return "T0003";
            case DiagnosticKind::T0004_UnknownFunction:
                return "T0004";
            case DiagnosticKind::T0005_UnknownType:
                return "T0005";
            case DiagnosticKind::T0006_UnknownMethod:
                return "T0006";
            case DiagnosticKind::T0007_UnknownField:
                return "T0007";

            // Call diagnostics
            case DiagnosticKind::T0008_ArgumentCountMismatch:
                return "T0008";
            case DiagnosticKind::T0009_ArgumentTypeMismatch:
                return "T0009";
            case DiagnosticKind::T0010_InvalidVariadicArgumentType:
                return "T0010";

            // Control flow and declaration-shape diagnostics
            case DiagnosticKind::T0011_NonBoolIfCondition:
                return "T0011";
            case DiagnosticKind::T0012_NonBoolWhileCondition:
                return "T0012";
            case DiagnosticKind::T0013_NonExternVariadicFunction:
                return "T0013";

            // Type mismatch diagnostics
            case DiagnosticKind::T0014_ReturnTypeMismatch:
                return "T0014";
            case DiagnosticKind::T0015_AssignmentTypeMismatch:
                return "T0015";
            case DiagnosticKind::T0016_ExplicitConstantTypeMismatch:
                return "T0016";
            case DiagnosticKind::T0017_ExplicitVariableTypeMismatch:
                return "T0017";
            case DiagnosticKind::T0018_TypeFieldInitializerMismatch:
                return "T0018";
            case DiagnosticKind::T0019_ArithmeticOperandTypeMismatch:
                return "T0019";
            case DiagnosticKind::T0020_ComparisonOperandTypeMismatch:
                return "T0020";
            case DiagnosticKind::T0021_EnumFieldValueTypeMismatch:
                return "T0021";
            case DiagnosticKind::Unknown:
            default:
                return "?????";
        }
    }

    static std::string DiagnosticMessage(DiagnosticKind kind)
    {
        switch (kind)
        {
            // Lexer diagnostics
            case DiagnosticKind::L0001_IllegalCharacter:
                return "Illegal character";
            case DiagnosticKind::L0002_UnterminatedString:
                return "Unterminated string";

            // Parser syntax diagnostics
            case DiagnosticKind::P0001_UnexpectedToken:
                return "Unexpected token";
            case DiagnosticKind::P0002_UnexpectedTrailingTokens:
                return "Unexpected trailing tokens";
            case DiagnosticKind::P0003_InvalidEnumField:
                return "Invalid enum field";
            case DiagnosticKind::P0006_UnexpectedTopLevelToken:
                return "Unexpected top-level token";
            case DiagnosticKind::P0007_InvalidStatement:
                return "Invalid statement";
            case DiagnosticKind::P0008_InvalidExpression:
                return "Invalid expression";

            // Annotation diagnostics
            case DiagnosticKind::P0004_UnexpectedAnnotation:
                return "Unexpected annotation";
            case DiagnosticKind::P0005_UnknownAnnotation:
                return "Unknown annotation";
            case DiagnosticKind::T0001_MissingAnnotationArguments:
                return "Missing annotation arguments";
            case DiagnosticKind::T0002_WrongNumberOfAnnotationArguments:
                return "Wrong number of annotation arguments";

            // Unknown symbol diagnostics
            case DiagnosticKind::T0003_UnknownName:
                return "Unknown name";
            case DiagnosticKind::T0004_UnknownFunction:
                return "Unknown function";
            case DiagnosticKind::T0005_UnknownType:
                return "Unknown type";
            case DiagnosticKind::T0006_UnknownMethod:
                return "Unknown method";
            case DiagnosticKind::T0007_UnknownField:
                return "Unknown field";

            // Call diagnostics
            case DiagnosticKind::T0008_ArgumentCountMismatch:
                return "Argument count mismatch";
            case DiagnosticKind::T0009_ArgumentTypeMismatch:
                return "Argument type mismatch";
            case DiagnosticKind::T0010_InvalidVariadicArgumentType:
                return "Invalid variadic argument type";

            // Control flow and declaration-shape diagnostics
            case DiagnosticKind::T0011_NonBoolIfCondition:
                return "Non-bool if condition";
            case DiagnosticKind::T0012_NonBoolWhileCondition:
                return "Non-bool while condition";
            case DiagnosticKind::T0013_NonExternVariadicFunction:
                return "Non-extern variadic function";

            // Type mismatch diagnostics
            case DiagnosticKind::T0014_ReturnTypeMismatch:
                return "Return type mismatch";
            case DiagnosticKind::T0015_AssignmentTypeMismatch:
                return "Assignment type mismatch";
            case DiagnosticKind::T0016_ExplicitConstantTypeMismatch:
                return "Explicit constant type mismatch";
            case DiagnosticKind::T0017_ExplicitVariableTypeMismatch:
                return "Explicit variable type mismatch";
            case DiagnosticKind::T0018_TypeFieldInitializerMismatch:
                return "Type field initializer mismatch";
            case DiagnosticKind::T0019_ArithmeticOperandTypeMismatch:
                return "Arithmetic operand type mismatch";
            case DiagnosticKind::T0020_ComparisonOperandTypeMismatch:
                return "Comparison operand type mismatch";
            case DiagnosticKind::T0021_EnumFieldValueTypeMismatch:
                return "Enum field value type mismatch";
            case DiagnosticKind::Unknown:
            default:
                return "Unknown diagnostic";
        }
    }

    Diagnostic::Diagnostic(
        DiagnosticLevel level,
        DiagnosticKind kind,
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        std::optional<std::string> fix)
        : m_level(level)
        , m_kind(kind)
        , m_message(DiagnosticMessage(kind))
        , m_source(source)
        , m_location(location)
        , m_fix(std::move(fix))
    {
    }

    std::string_view Diagnostic::code() const noexcept
    {
        return DiagnosticCode(m_kind);
    }

    void Diagnostic::addPrimaryLabel(const SourceLocation& location, std::string text)
    {
        m_labels.push_back(Label{
            .source = m_source,
            .location = location,
            .text = std::move(text),
            .isPrimary = true
            });
    }
}
