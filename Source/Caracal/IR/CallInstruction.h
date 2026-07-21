#pragma once

#include <Caracal/IR/Instruction.h>
#include <Caracal/IR/ValueRef.h>

#include <vector>

namespace Caracal
{
    class CallInstruction final : public Instruction
    {
    public:
        CallInstruction(TemporaryId resultId, FunctionId functionId, std::vector<ValueRef> arguments, Type type) noexcept;

        [[nodiscard]] TemporaryId resultId() const noexcept { return m_resultId; }
        [[nodiscard]] FunctionId functionId() const noexcept { return m_functionId; }
        [[nodiscard]] const std::vector<ValueRef>& arguments() const noexcept { return m_arguments; }
        [[nodiscard]] Type type() const noexcept { return m_type; }
        void remapValueIds(const ValueIdMap& remap) noexcept override;

    private:
        TemporaryId m_resultId;
        FunctionId m_functionId;
        std::vector<ValueRef> m_arguments;
        Type m_type;
    };
}
