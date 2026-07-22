#pragma once

#include <Caracal/API.h>
#include <Caracal/Optimization/OperatorFolding.h>
#include <Caracal/Syntax/Node.h>
#include <Caracal/Syntax/TokenBuffer.h>

#include <optional>

namespace Caracal
{
    class CARACAL_API Expression : public Node
    {
    public:
        Expression(NodeKind kind, const Type& type);
        virtual ~Expression() = default;

        [[nodiscard]] bool isLiteral() const noexcept;
        [[nodiscard]] virtual SourceLocation sourceLocation(const TokenBuffer& tokens) const = 0;
        void setFoldedValue(const FoldValue& value) noexcept { m_foldedValue = value; }
        [[nodiscard]] const std::optional<FoldValue>& foldedValue() const noexcept { return m_foldedValue; }

    private:
        std::optional<FoldValue> m_foldedValue{};
    };

    using ExpressionUPtr = std::unique_ptr<Expression>;
}
