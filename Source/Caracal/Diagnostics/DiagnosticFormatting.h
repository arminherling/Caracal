#pragma once

#include <Caracal/Semantic/SemanticContext.h>
#include <Caracal/Syntax/BinaryExpression.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/TokenBuffer.h>

#include <optional>
#include <string>

namespace Caracal
{
    [[nodiscard]] std::string formatAnnotationArgumentValue(const Expression* expression, const TokenBuffer& tokens);
    [[nodiscard]] std::optional<SourceLocation> getTypeFieldLocation(const TypeDefinition& typeDefinition, const FieldDefinition& fieldDefinition, const TokenBuffer& tokens);
    [[nodiscard]] std::string formatTypeName(SemanticContext& module, Type type);
    [[nodiscard]] std::string formatBinaryOperator(BinaryOperatorKind binaryOperator);
}
