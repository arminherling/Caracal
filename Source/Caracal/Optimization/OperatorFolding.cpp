#include <Caracal/Optimization/OperatorFolding.h>

#include <cstdint>
#include <limits>
#include <type_traits>

namespace Caracal
{
    namespace
    {
        template <typename TInteger>
        FoldResult MakeIntegerResult(std::int64_t result)
        {
            if (result >= static_cast<std::int64_t>(std::numeric_limits<TInteger>::min())
                && result <= static_cast<std::int64_t>(std::numeric_limits<TInteger>::max()))
            {
                return FoldResult{ FoldResultKind::Value, FoldValue{ static_cast<TInteger>(result) } };
            }

            return FoldResult{ FoldResultKind::Overflow, FoldValue{ static_cast<TInteger>(result) } };
        }

        enum class ArithmeticOperation
        {
            Addition,
            Subtraction,
            Multiplication,
        };

        FoldResult FoldArithmetic(ArithmeticOperation operation, const FoldValue& lhs, const FoldValue& rhs)
        {
            return std::visit(
                [operation](const auto& left, const auto& right) -> FoldResult
                {
                    using Left = std::decay_t<decltype(left)>;
                    using Right = std::decay_t<decltype(right)>;

                    if constexpr (!std::is_same_v<Left, Right>)
                    {
                        return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                    }
                    else if constexpr (std::is_same_v<Left, float>)
                    {
                        auto result = 0.0f;
                        switch (operation)
                        {
                            case ArithmeticOperation::Addition:
                            {
                                result = left + right;
                                break;
                            }
                            case ArithmeticOperation::Subtraction:
                            {
                                result = left - right;
                                break;
                            }
                            case ArithmeticOperation::Multiplication:
                            {
                                result = left * right;
                                break;
                            }
                        }

                        return FoldResult{ FoldResultKind::Value, FoldValue{ result } };
                    }
                    else if constexpr (std::is_same_v<Left, u8> || std::is_same_v<Left, i32>)
                    {
                        const auto leftValue = static_cast<std::int64_t>(left);
                        const auto rightValue = static_cast<std::int64_t>(right);
                        std::int64_t result = 0;
                        switch (operation)
                        {
                            case ArithmeticOperation::Addition:
                            {
                                result = leftValue + rightValue;
                                break;
                            }
                            case ArithmeticOperation::Subtraction:
                            {
                                result = leftValue - rightValue;
                                break;
                            }
                            case ArithmeticOperation::Multiplication:
                            {
                                result = leftValue * rightValue;
                                break;
                            }
                        }

                        return MakeIntegerResult<Left>(result);
                    }
                    else
                    {
                        return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                    }
                },
                lhs,
                rhs);
        }

        enum class ComparisonOperation
        {
            Equal,
            NotEqual,
            LessThan,
            LessOrEqual,
            GreaterThan,
            GreaterOrEqual,
        };

        // IEEE float uses ordered semantics for comparisons, which means NaN compares are false
        // except for not equals but we dont define those in the prelude for floats
        FoldResult FoldComparison(ComparisonOperation operation, const FoldValue& lhs, const FoldValue& rhs)
        {
            return std::visit(
                [operation](const auto& left, const auto& right) -> FoldResult
                {
                    using Left = std::decay_t<decltype(left)>;
                    using Right = std::decay_t<decltype(right)>;

                    if constexpr (!std::is_same_v<Left, Right>)
                    {
                        return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                    }
                    else if constexpr (std::is_same_v<Left, bool>)
                    {
                        switch (operation)
                        {
                            case ComparisonOperation::Equal:
                            {
                                return FoldResult{ FoldResultKind::Value, FoldValue{ left == right } };
                            }
                            case ComparisonOperation::NotEqual:
                            {
                                return FoldResult{ FoldResultKind::Value, FoldValue{ left != right } };
                            }
                            default:
                            {
                                return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                            }
                        }
                    }
                    else
                    {
                        auto result = false;
                        switch (operation)
                        {
                            case ComparisonOperation::Equal:
                            {
                                result = left == right;
                                break;
                            }
                            case ComparisonOperation::NotEqual:
                            {
                                result = left != right;
                                break;
                            }
                            case ComparisonOperation::LessThan:
                            {
                                result = left < right;
                                break;
                            }
                            case ComparisonOperation::LessOrEqual:
                            {
                                result = left <= right;
                                break;
                            }
                            case ComparisonOperation::GreaterThan:
                            {
                                result = left > right;
                                break;
                            }
                            case ComparisonOperation::GreaterOrEqual:
                            {
                                result = left >= right;
                                break;
                            }
                        }

                        return FoldResult{ FoldResultKind::Value, FoldValue{ result } };
                    }
                },
                lhs,
                rhs);
        }

        FoldResult FoldLogical(bool isAnd, const FoldValue& lhs, const FoldValue& rhs)
        {
            return std::visit(
                [isAnd](const auto& left, const auto& right) -> FoldResult
                {
                    using Left = std::decay_t<decltype(left)>;
                    using Right = std::decay_t<decltype(right)>;

                    if constexpr (std::is_same_v<Left, bool> && std::is_same_v<Right, bool>)
                    {
                        if (isAnd)
                        {
                            return FoldResult{ FoldResultKind::Value, FoldValue{ left && right } };
                        }

                        return FoldResult{ FoldResultKind::Value, FoldValue{ left || right } };
                    }
                    else
                    {
                        return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                    }
                },
                lhs,
                rhs);
        }
    }

    FoldResult FoldAddition(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldArithmetic(ArithmeticOperation::Addition, lhs, rhs);
    }

    FoldResult FoldSubtraction(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldArithmetic(ArithmeticOperation::Subtraction, lhs, rhs);
    }

    FoldResult FoldMultiplication(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldArithmetic(ArithmeticOperation::Multiplication, lhs, rhs);
    }

    FoldResult FoldDivision(const FoldValue& lhs, const FoldValue& rhs)
    {
        return std::visit(
            [](const auto& left, const auto& right) -> FoldResult
            {
                using Left = std::decay_t<decltype(left)>;
                using Right = std::decay_t<decltype(right)>;

                if constexpr (!std::is_same_v<Left, Right>)
                {
                    return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                }
                else if constexpr (std::is_same_v<Left, float>)
                {
                    if (right == 0.0f)
                    {
                        return FoldResult{ FoldResultKind::DivideByZero, FoldValue{} };
                    }

                    return FoldResult{ FoldResultKind::Value, FoldValue{ left / right } };
                }
                else if constexpr (std::is_same_v<Left, u8> || std::is_same_v<Left, i32>)
                {
                    if (right == 0)
                    {
                        return FoldResult{ FoldResultKind::DivideByZero, FoldValue{} };
                    }

                    return FoldResult{ FoldResultKind::Value, FoldValue{ static_cast<float>(left) / static_cast<float>(right) } };
                }
                else
                {
                    return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                }
            },
            lhs,
            rhs);
    }

    FoldResult FoldEqual(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldComparison(ComparisonOperation::Equal, lhs, rhs);
    }

    FoldResult FoldNotEqual(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldComparison(ComparisonOperation::NotEqual, lhs, rhs);
    }

    FoldResult FoldLessThan(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldComparison(ComparisonOperation::LessThan, lhs, rhs);
    }

    FoldResult FoldLessOrEqual(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldComparison(ComparisonOperation::LessOrEqual, lhs, rhs);
    }

    FoldResult FoldGreaterThan(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldComparison(ComparisonOperation::GreaterThan, lhs, rhs);
    }

    FoldResult FoldGreaterOrEqual(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldComparison(ComparisonOperation::GreaterOrEqual, lhs, rhs);
    }

    FoldResult FoldLogicalAnd(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldLogical(true, lhs, rhs);
    }

    FoldResult FoldLogicalOr(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldLogical(false, lhs, rhs);
    }

    FoldResult FoldValueNegation(const FoldValue& value)
    {
        return std::visit(
            [](const auto& payload) -> FoldResult
            {
                using Payload = std::decay_t<decltype(payload)>;

                if constexpr (std::is_same_v<Payload, float>)
                {
                    return FoldResult{ FoldResultKind::Value, FoldValue{ -payload } };
                }
                else if constexpr (std::is_same_v<Payload, u8> || std::is_same_v<Payload, i32>)
                {
                    return MakeIntegerResult<Payload>(-static_cast<std::int64_t>(payload));
                }
                else
                {
                    return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                }
            },
            value);
    }

    FoldResult FoldLogicalNegation(const FoldValue& value)
    {
        return std::visit(
            [](const auto& payload) -> FoldResult
            {
                using Payload = std::decay_t<decltype(payload)>;

                if constexpr (std::is_same_v<Payload, bool>)
                {
                    return FoldResult{ FoldResultKind::Value, FoldValue{ !payload } };
                }
                else
                {
                    return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                }
            },
            value);
    }

    template <typename TOperation>
    [[nodiscard]] static FoldResult FoldBitwiseBinary(const FoldValue& lhs, const FoldValue& rhs, TOperation operation)
    {
        return std::visit(
            [&operation](const auto& left, const auto& right) -> FoldResult
            {
                using Left = std::decay_t<decltype(left)>;
                using Right = std::decay_t<decltype(right)>;

                if constexpr (!std::is_same_v<Left, Right>)
                {
                    return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                }
                else if constexpr (std::is_same_v<Left, u8> || std::is_same_v<Left, i32>)
                {
                    return FoldResult{ FoldResultKind::Value, FoldValue{ static_cast<Left>(operation(left, right)) } };
                }
                else
                {
                    return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                }
            },
            lhs,
            rhs);
    }

    FoldResult FoldBitAnd(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldBitwiseBinary(lhs, rhs, [](auto left, auto right) { return left & right; });
    }

    FoldResult FoldBitOr(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldBitwiseBinary(lhs, rhs, [](auto left, auto right) { return left | right; });
    }

    FoldResult FoldBitXor(const FoldValue& lhs, const FoldValue& rhs)
    {
        return FoldBitwiseBinary(lhs, rhs, [](auto left, auto right) { return left ^ right; });
    }

    FoldResult FoldBitNot(const FoldValue& value)
    {
        return std::visit(
            [](const auto& payload) -> FoldResult
            {
                using Payload = std::decay_t<decltype(payload)>;

                if constexpr (std::is_same_v<Payload, u8> || std::is_same_v<Payload, i32>)
                {
                    return FoldResult{ FoldResultKind::Value, FoldValue{ static_cast<Payload>(~payload) } };
                }
                else
                {
                    return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                }
            },
            value);
    }

    template <typename TOperation>
    [[nodiscard]] static FoldResult FoldShift(const FoldValue& value, const FoldValue& amount, TOperation operation)
    {
        return std::visit(
            [&operation](const auto& left, const auto& right) -> FoldResult
            {
                using Left = std::decay_t<decltype(left)>;
                using Right = std::decay_t<decltype(right)>;

                // the amount is always an i32, masked to the operand width exactly like the runtime
                if constexpr (!std::is_same_v<Right, i32>)
                {
                    return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                }
                else if constexpr (std::is_same_v<Left, u8>)
                {
                    return FoldResult{ FoldResultKind::Value, FoldValue{ static_cast<u8>(operation(left, right & 7)) } };
                }
                else if constexpr (std::is_same_v<Left, i32>)
                {
                    return FoldResult{ FoldResultKind::Value, FoldValue{ static_cast<i32>(operation(left, right & 31)) } };
                }
                else
                {
                    return FoldResult{ FoldResultKind::NotFoldable, FoldValue{} };
                }
            },
            value,
            amount);
    }

    FoldResult FoldShiftLeft(const FoldValue& value, const FoldValue& amount)
    {
        // shifting in unsigned space matches the runtime wraparound for signed values
        return FoldShift(value, amount, [](auto left, i32 maskedAmount) { return static_cast<std::uint32_t>(left) << maskedAmount; });
    }

    FoldResult FoldShiftRight(const FoldValue& value, const FoldValue& amount)
    {
        // signed values shift arithmetic and unsigned values shift logical, matching the emitted instructions
        return FoldShift(value, amount, [](auto left, i32 maskedAmount) { return left >> maskedAmount; });
    }
}
