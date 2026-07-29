#pragma once

#include <Caracal/IR/Instruction.h>
#include <Caracal/IR/ValueRef.h>

namespace Caracal
{
    class ArrayCopyInstruction final : public Instruction
    {
    public:
        ArrayCopyInstruction(TemporaryId resultId, ValueRef sourceAddress, Type arrayType, FunctionId callocFunctionId, FunctionId memmoveFunctionId) noexcept;

        [[nodiscard]] TemporaryId resultId() const noexcept { return m_resultId; }
        [[nodiscard]] ValueRef sourceAddress() const noexcept { return m_sourceAddress; }
        [[nodiscard]] Type arrayType() const noexcept { return m_arrayType; }
        [[nodiscard]] FunctionId callocFunctionId() const noexcept { return m_callocFunctionId; }
        [[nodiscard]] FunctionId memmoveFunctionId() const noexcept { return m_memmoveFunctionId; }
        void remapValueIds(const ValueIdMap& remap) noexcept override;

    private:
        TemporaryId m_resultId;
        ValueRef m_sourceAddress;
        Type m_arrayType;
        FunctionId m_callocFunctionId;
        FunctionId m_memmoveFunctionId;
    };
}
