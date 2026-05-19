#pragma once

#include <Caracal/IR/Instruction.h>

namespace Caracal
{
    class ParameterInstruction final : public Instruction
    {
    public:
        ParameterInstruction(TemporaryId resultId, i32 parameterIndex, Type type) noexcept;

        [[nodiscard]] TemporaryId resultId() const noexcept { return m_resultId; }
        [[nodiscard]] i32 parameterIndex() const noexcept { return m_parameterIndex; }
        [[nodiscard]] Type type() const noexcept { return m_type; }

    private:
        TemporaryId m_resultId;
        i32 m_parameterIndex;
        Type m_type;
    };
}
