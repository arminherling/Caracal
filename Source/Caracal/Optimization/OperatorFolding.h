#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>

#include <optional>
#include <variant>

namespace Caracal
{
    using FoldValue = std::variant<bool, u8, i32, float>;

    enum class FoldResultKind
    {
        Value,
        Overflow,
        DivideByZero,
        NotFoldable,
    };

    struct FoldResult
    {
        FoldResultKind kind;
        FoldValue value;
    };

    using BinaryFoldFunction = FoldResult (*)(const FoldValue& lhs, const FoldValue& rhs);
    using UnaryFoldFunction = FoldResult (*)(const FoldValue& value);

    [[nodiscard]] CARACAL_API FoldResult FoldAddition(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldSubtraction(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldMultiplication(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldDivision(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldValueNegation(const FoldValue& value);
    [[nodiscard]] CARACAL_API FoldResult FoldLogicalNegation(const FoldValue& value);
}
