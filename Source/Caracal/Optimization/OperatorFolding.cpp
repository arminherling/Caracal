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
}
