#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>

#include <optional>
#include <variant>

namespace Caracal
{
    using FoldValue = std::variant<bool, u8, u16, u32, u64, i8, i16, i32, i64, float, f64>;

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
    [[nodiscard]] CARACAL_API FoldResult FoldEqual(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldNotEqual(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldLessThan(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldLessOrEqual(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldGreaterThan(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldGreaterOrEqual(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldLogicalAnd(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldLogicalOr(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldValueNegation(const FoldValue& value);
    [[nodiscard]] CARACAL_API FoldResult FoldLogicalNegation(const FoldValue& value);
    [[nodiscard]] CARACAL_API FoldResult FoldBitAnd(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldBitOr(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldBitXor(const FoldValue& lhs, const FoldValue& rhs);
    [[nodiscard]] CARACAL_API FoldResult FoldBitNot(const FoldValue& value);
    [[nodiscard]] CARACAL_API FoldResult FoldShiftLeft(const FoldValue& value, const FoldValue& amount);
    [[nodiscard]] CARACAL_API FoldResult FoldShiftRight(const FoldValue& value, const FoldValue& amount);
}
