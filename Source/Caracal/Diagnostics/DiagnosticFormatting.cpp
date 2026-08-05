#include "DiagnosticFormatting.h"

#include <Caracal/Syntax/TypeFieldDeclaration.h>
#include <Caracal/Syntax/TypeDefinitionStatement.h>

namespace Caracal
{
    std::string formatAnnotationArgumentValue(const Expression* expression, const TokenBuffer& tokens)
    {
        const auto location = expression->sourceLocation(tokens);
        const auto& text = tokens.source()->text;
        if (location.startIndex < 0 || location.endIndex > static_cast<i32>(text.size()) || location.endIndex <= location.startIndex)
        {
            return {};
        }

        return text.substr(location.startIndex, location.endIndex - location.startIndex);
    }

    std::optional<SourceLocation> getTypeFieldLocation(const TypeDefinition& typeDefinition, const FieldDefinition& fieldDefinition, const TokenBuffer& tokens)
    {
        const auto fieldIndex = static_cast<size_t>(fieldDefinition.index());
        const auto& statements = typeDefinition.statement()->bodyNode()->statements();
        if (fieldIndex >= statements.size())
        {
            return std::nullopt;
        }

        auto* fieldStatement = static_cast<TypeFieldDeclaration*>(statements[fieldIndex].get());
        return fieldStatement->nameExpression()->sourceLocation(tokens);
    }

    std::string formatTypeName(SemanticContext& module, Type type)
    {
        if (type == Type::Undefined())
        {
            return "undefined";
        }

        if (type == Type::Void())
        {
            return "void";
        }

        auto name = std::string(module.getNameByType(type));
        if (type.isReference())
        {
            return "ref " + name;
        }

        return name;
    }

    std::string formatBinaryOperator(BinaryOperatorKind binaryOperator)
    {
        switch (binaryOperator)
        {
            case BinaryOperatorKind::AdditionWrapping:
                return "%+";
            case BinaryOperatorKind::SubtractionWrapping:
                return "%-";
            case BinaryOperatorKind::MultiplicationWrapping:
                return "%*";
            case BinaryOperatorKind::Division:
                return "/";
            case BinaryOperatorKind::Equal:
                return "==";
            case BinaryOperatorKind::NotEqual:
                return "!=";
            case BinaryOperatorKind::LessThan:
                return "<";
            case BinaryOperatorKind::LessOrEqual:
                return "<=";
            case BinaryOperatorKind::GreaterThan:
                return ">";
            case BinaryOperatorKind::GreaterOrEqual:
                return ">=";
            case BinaryOperatorKind::LogicalAnd:
                return "and";
            case BinaryOperatorKind::LogicalOr:
                return "or";
            default:
                return stringify(binaryOperator);
        }
    }

}
