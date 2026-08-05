#pragma once

#include <Caracal/Semantic/SemanticContext.h>
#include <Caracal/Syntax/NumberLiteral.h>

#include <optional>
#include <string_view>

namespace Caracal
{
    [[nodiscard]] std::optional<i32> tryParseI32Literal(std::string_view lexeme);
    [[nodiscard]] std::optional<i32> tryParseNegatedI32Literal(std::string_view lexeme);
    [[nodiscard]] std::optional<u8> tryParseU8Literal(std::string_view lexeme);
    [[nodiscard]] std::optional<f64> tryParseF64Literal(std::string_view lexeme);
    [[nodiscard]] std::optional<f32> tryParseF32Literal(std::string_view lexeme);
    [[nodiscard]] std::optional<NumberLiteral::ParsedValue> tryParseNumberLiteralValue(std::string_view lexeme, Type type, const SemanticContext& module);
    [[nodiscard]] std::optional<i32> tryConvertEnumFieldLiteralValue(const NumberLiteral& literal);
}
