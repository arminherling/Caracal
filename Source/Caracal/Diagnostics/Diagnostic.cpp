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
            case DiagnosticKind::P0003_UnexpectedTopLevelToken:
                return "P0003";
            case DiagnosticKind::P0004_InvalidEnumField:
                return "P0004";
            case DiagnosticKind::P0005_InvalidStatement:
                return "P0005";
            case DiagnosticKind::P0006_InvalidExpression:
                return "P0006";
            case DiagnosticKind::P0007_DanglingAnnotation:
                return "P0007";

            // Annotation diagnostics
            case DiagnosticKind::T0001_MissingAnnotationArguments:
                return "T0001";
            case DiagnosticKind::T0002_WrongNumberOfAnnotationArguments:
                return "T0002";
            case DiagnosticKind::T0003_AnnotationArgumentTypeMismatch:
                return "T0003";
            case DiagnosticKind::T0004_UnexpectedAnnotationTarget:
                return "T0004";
            case DiagnosticKind::T0005_ConflictingEnumAnnotations:
                return "T0005";
            case DiagnosticKind::T0006_UnknownAnnotation:
                return "T0006";

            // Unknown symbol diagnostics
            case DiagnosticKind::T0007_UnknownName:
                return "T0007";
            case DiagnosticKind::T0008_UnknownFunction:
                return "T0008";
            case DiagnosticKind::T0009_UnknownType:
                return "T0009";
            case DiagnosticKind::T0010_UnknownMethod:
                return "T0010";
            case DiagnosticKind::T0011_UnknownField:
                return "T0011";

            // Call diagnostics
            case DiagnosticKind::T0012_ArgumentCountMismatch:
                return "T0012";
            case DiagnosticKind::T0013_ArgumentTypeMismatch:
                return "T0013";
            case DiagnosticKind::T0014_InvalidVariadicArgumentType:
                return "T0014";

            // Control flow and declaration-shape diagnostics
            case DiagnosticKind::T0015_NonBoolIfCondition:
                return "T0015";
            case DiagnosticKind::T0016_NonBoolWhileCondition:
                return "T0016";
            case DiagnosticKind::T0017_NonExternVariadicFunction:
                return "T0017";

            // Type mismatch diagnostics
            case DiagnosticKind::T0018_ReturnTypeMismatch:
                return "T0018";
            case DiagnosticKind::T0019_AssignmentTypeMismatch:
                return "T0019";
            case DiagnosticKind::T0020_ExplicitConstantTypeMismatch:
                return "T0020";
            case DiagnosticKind::T0021_ExplicitVariableTypeMismatch:
                return "T0021";
            case DiagnosticKind::T0022_TypeFieldInitializerMismatch:
                return "T0022";
            case DiagnosticKind::T0023_ArithmeticOperandTypeMismatch:
                return "T0023";
            case DiagnosticKind::T0024_ComparisonOperandTypeMismatch:
                return "T0024";
            case DiagnosticKind::T0025_EnumFieldValueTypeMismatch:
                return "T0025";
            case DiagnosticKind::T0026_DuplicateDeclaration:
                return "T0026";
            case DiagnosticKind::T0027_FlagEnumExplicitValue:
                return "T0027";
            case DiagnosticKind::T0028_ReferenceReturnType:
                return "T0028";
            case DiagnosticKind::T0029_ExplicitConstructorDeclaration:
                return "T0029";
            case DiagnosticKind::T0030_AlreadyReference:
                return "T0030";
            case DiagnosticKind::T0031_DuplicateTypeDeclaration:
                return "T0031";
            case DiagnosticKind::T0032_DuplicateFunctionDeclaration:
                return "T0032";
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
            case DiagnosticKind::P0003_UnexpectedTopLevelToken:
                return "Unexpected top-level token";
            case DiagnosticKind::P0004_InvalidEnumField:
                return "Invalid enum field";
            case DiagnosticKind::P0005_InvalidStatement:
                return "Invalid statement";
            case DiagnosticKind::P0006_InvalidExpression:
                return "Invalid expression";
            case DiagnosticKind::P0007_DanglingAnnotation:
                return "Dangling annotation";

            // Annotation diagnostics
            case DiagnosticKind::T0001_MissingAnnotationArguments:
                return "Missing annotation arguments";
            case DiagnosticKind::T0002_WrongNumberOfAnnotationArguments:
                return "Wrong number of annotation arguments";
            case DiagnosticKind::T0003_AnnotationArgumentTypeMismatch:
                return "Annotation argument type mismatch";
            case DiagnosticKind::T0004_UnexpectedAnnotationTarget:
                return "Unexpected annotation target";
            case DiagnosticKind::T0005_ConflictingEnumAnnotations:
                return "Conflicting enum annotations";
            case DiagnosticKind::T0006_UnknownAnnotation:
                return "Unknown annotation";

            // Unknown symbol diagnostics
            case DiagnosticKind::T0007_UnknownName:
                return "Unknown name";
            case DiagnosticKind::T0008_UnknownFunction:
                return "Unknown function";
            case DiagnosticKind::T0009_UnknownType:
                return "Unknown type";
            case DiagnosticKind::T0010_UnknownMethod:
                return "Unknown method";
            case DiagnosticKind::T0011_UnknownField:
                return "Unknown field";

            // Call diagnostics
            case DiagnosticKind::T0012_ArgumentCountMismatch:
                return "Argument count mismatch";
            case DiagnosticKind::T0013_ArgumentTypeMismatch:
                return "Argument type mismatch";
            case DiagnosticKind::T0014_InvalidVariadicArgumentType:
                return "Invalid variadic argument type";

            // Control flow and declaration-shape diagnostics
            case DiagnosticKind::T0015_NonBoolIfCondition:
                return "Non-bool if condition";
            case DiagnosticKind::T0016_NonBoolWhileCondition:
                return "Non-bool while condition";
            case DiagnosticKind::T0017_NonExternVariadicFunction:
                return "Non-extern variadic function";

            // Type mismatch diagnostics
            case DiagnosticKind::T0018_ReturnTypeMismatch:
                return "Return type mismatch";
            case DiagnosticKind::T0019_AssignmentTypeMismatch:
                return "Assignment type mismatch";
            case DiagnosticKind::T0020_ExplicitConstantTypeMismatch:
                return "Explicit constant type mismatch";
            case DiagnosticKind::T0021_ExplicitVariableTypeMismatch:
                return "Explicit variable type mismatch";
            case DiagnosticKind::T0022_TypeFieldInitializerMismatch:
                return "Type field initializer mismatch";
            case DiagnosticKind::T0023_ArithmeticOperandTypeMismatch:
                return "Arithmetic operand type mismatch";
            case DiagnosticKind::T0024_ComparisonOperandTypeMismatch:
                return "Comparison operand type mismatch";
            case DiagnosticKind::T0025_EnumFieldValueTypeMismatch:
                return "Enum field value type mismatch";
            case DiagnosticKind::T0026_DuplicateDeclaration:
                return "Duplicate declaration";
            case DiagnosticKind::T0027_FlagEnumExplicitValue:
                return "Flag enum explicit value";
            case DiagnosticKind::T0028_ReferenceReturnType:
                return "Reference return type";
            case DiagnosticKind::T0029_ExplicitConstructorDeclaration:
                return "Explicit constructor declaration";
            case DiagnosticKind::T0030_AlreadyReference:
                return "Already a reference";
            case DiagnosticKind::T0031_DuplicateTypeDeclaration:
                return "Duplicate type declaration";
            case DiagnosticKind::T0032_DuplicateFunctionDeclaration:
                return "Duplicate function declaration";
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

    void Diagnostic::addSecondaryLabel(const SourceLocation& location, std::string text)
    {
        m_labels.push_back(Label{
            .source = m_source,
            .location = location,
            .text = std::move(text),
            .isPrimary = false
            });
    }

    void Diagnostic::addRelatedPrimaryLabel(
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        std::string message,
        std::string text)
    {
        auto relatedSource = source;
        if (!relatedSource)
        {
            relatedSource = m_source;
        }

        auto related = RelatedReport{
            .source = relatedSource,
            .message = std::move(message),
            .labels = {}
        };
        related.labels.push_back(Label{
            .source = relatedSource,
            .location = location,
            .text = std::move(text),
            .isPrimary = true,
        });
        m_related.push_back(std::move(related));
    }
}
