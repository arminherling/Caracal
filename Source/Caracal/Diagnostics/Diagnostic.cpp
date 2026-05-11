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
            case DiagnosticKind::T0001_UnknownAnnotation:
                return "T0001";
            case DiagnosticKind::T0002_UnexpectedAnnotationTarget:
                return "T0002";
            case DiagnosticKind::T0003_MissingAnnotationArguments:
                return "T0003";
            case DiagnosticKind::T0004_WrongNumberOfAnnotationArguments:
                return "T0004";
            case DiagnosticKind::T0005_AnnotationArgumentTypeMismatch:
                return "T0005";
            case DiagnosticKind::T0006_ConflictingEnumAnnotations:
                return "T0006";

            // Unknown symbol and member access diagnostics
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
            case DiagnosticKind::T0012_UnknownEnumField:
                return "T0012";
            case DiagnosticKind::T0013_InvalidEnumMemberAccess:
                return "T0013";
            case DiagnosticKind::T0014_InvalidMemberAccessReceiver:
                return "T0014";

            // Call diagnostics
            case DiagnosticKind::T0015_ArgumentCountMismatch:
                return "T0015";
            case DiagnosticKind::T0016_ArgumentTypeMismatch:
                return "T0016";
            case DiagnosticKind::T0017_InvalidVariadicArgumentType:
                return "T0017";

            // Control flow diagnostics
            case DiagnosticKind::T0018_NonBoolIfCondition:
                return "T0018";
            case DiagnosticKind::T0019_NonBoolWhileCondition:
                return "T0019";

            // Type mismatch diagnostics
            case DiagnosticKind::T0020_ReturnTypeMismatch:
                return "T0020";
            case DiagnosticKind::T0021_AssignmentTypeMismatch:
                return "T0021";
            case DiagnosticKind::T0022_ExplicitConstantTypeMismatch:
                return "T0022";
            case DiagnosticKind::T0023_ExplicitVariableTypeMismatch:
                return "T0023";
            case DiagnosticKind::T0024_TypeFieldInitializerMismatch:
                return "T0024";
            case DiagnosticKind::T0025_ArithmeticOperandTypeMismatch:
                return "T0025";
            case DiagnosticKind::T0026_ComparisonOperandTypeMismatch:
                return "T0026";
            case DiagnosticKind::T0027_EnumFieldValueTypeMismatch:
                return "T0027";

            // Declaration-shape and reference diagnostics
            case DiagnosticKind::T0028_NonExternVariadicFunction:
                return "T0028";
            case DiagnosticKind::T0029_FlagEnumExplicitValue:
                return "T0029";
            case DiagnosticKind::T0030_ExplicitConstructorDeclaration:
                return "T0030";
            case DiagnosticKind::T0031_ReferenceReturnType:
                return "T0031";
            case DiagnosticKind::T0032_AlreadyReference:
                return "T0032";

            // Duplicate declaration diagnostics
            case DiagnosticKind::T0033_DuplicateDeclaration:
                return "T0033";
            case DiagnosticKind::T0034_DuplicateConstantDeclaration:
                return "T0034";
            case DiagnosticKind::T0035_DuplicateVariableDeclaration:
                return "T0035";
            case DiagnosticKind::T0036_DuplicateParameterDeclaration:
                return "T0036";
            case DiagnosticKind::T0037_DuplicateEnumFieldDeclaration:
                return "T0037";
            case DiagnosticKind::T0038_DuplicateTypeFieldDeclaration:
                return "T0038";
            case DiagnosticKind::T0039_DuplicateFunctionDeclaration:
                return "T0039";
            case DiagnosticKind::T0040_DuplicateTypeDeclaration:
                return "T0040";
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
            case DiagnosticKind::T0001_UnknownAnnotation:
                return "Unknown annotation";
            case DiagnosticKind::T0002_UnexpectedAnnotationTarget:
                return "Unexpected annotation target";
            case DiagnosticKind::T0003_MissingAnnotationArguments:
                return "Missing annotation arguments";
            case DiagnosticKind::T0004_WrongNumberOfAnnotationArguments:
                return "Wrong number of annotation arguments";
            case DiagnosticKind::T0005_AnnotationArgumentTypeMismatch:
                return "Annotation argument type mismatch";
            case DiagnosticKind::T0006_ConflictingEnumAnnotations:
                return "Conflicting enum annotations";

            // Unknown symbol and member access diagnostics
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
            case DiagnosticKind::T0012_UnknownEnumField:
                return "Unknown enum field";
            case DiagnosticKind::T0013_InvalidEnumMemberAccess:
                return "Invalid enum member access";
            case DiagnosticKind::T0014_InvalidMemberAccessReceiver:
                return "Invalid member access receiver";

            // Call diagnostics
            case DiagnosticKind::T0015_ArgumentCountMismatch:
                return "Argument count mismatch";
            case DiagnosticKind::T0016_ArgumentTypeMismatch:
                return "Argument type mismatch";
            case DiagnosticKind::T0017_InvalidVariadicArgumentType:
                return "Invalid variadic argument type";

            // Control flow diagnostics
            case DiagnosticKind::T0018_NonBoolIfCondition:
                return "Non-bool if condition";
            case DiagnosticKind::T0019_NonBoolWhileCondition:
                return "Non-bool while condition";

            // Type mismatch diagnostics
            case DiagnosticKind::T0020_ReturnTypeMismatch:
                return "Return type mismatch";
            case DiagnosticKind::T0021_AssignmentTypeMismatch:
                return "Assignment type mismatch";
            case DiagnosticKind::T0022_ExplicitConstantTypeMismatch:
                return "Explicit constant type mismatch";
            case DiagnosticKind::T0023_ExplicitVariableTypeMismatch:
                return "Explicit variable type mismatch";
            case DiagnosticKind::T0024_TypeFieldInitializerMismatch:
                return "Type field initializer mismatch";
            case DiagnosticKind::T0025_ArithmeticOperandTypeMismatch:
                return "Arithmetic operand type mismatch";
            case DiagnosticKind::T0026_ComparisonOperandTypeMismatch:
                return "Comparison operand type mismatch";
            case DiagnosticKind::T0027_EnumFieldValueTypeMismatch:
                return "Enum field value type mismatch";

            // Declaration-shape and reference diagnostics
            case DiagnosticKind::T0028_NonExternVariadicFunction:
                return "Non-extern variadic function";
            case DiagnosticKind::T0029_FlagEnumExplicitValue:
                return "Flag enum explicit value";
            case DiagnosticKind::T0030_ExplicitConstructorDeclaration:
                return "Explicit constructor declaration";
            case DiagnosticKind::T0031_ReferenceReturnType:
                return "Reference return type";
            case DiagnosticKind::T0032_AlreadyReference:
                return "Already a reference";

            // Duplicate declaration diagnostics
            case DiagnosticKind::T0033_DuplicateDeclaration:
                return "Duplicate declaration";
            case DiagnosticKind::T0034_DuplicateConstantDeclaration:
                return "Duplicate constant declaration";
            case DiagnosticKind::T0035_DuplicateVariableDeclaration:
                return "Duplicate variable declaration";
            case DiagnosticKind::T0036_DuplicateParameterDeclaration:
                return "Duplicate parameter declaration";
            case DiagnosticKind::T0037_DuplicateEnumFieldDeclaration:
                return "Duplicate enum field declaration";
            case DiagnosticKind::T0038_DuplicateTypeFieldDeclaration:
                return "Duplicate type field declaration";
            case DiagnosticKind::T0039_DuplicateFunctionDeclaration:
                return "Duplicate function declaration";
            case DiagnosticKind::T0040_DuplicateTypeDeclaration:
                return "Duplicate type declaration";
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
