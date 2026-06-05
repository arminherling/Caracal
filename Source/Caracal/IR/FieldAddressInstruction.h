#pragma once

#include <Caracal/IR/Instruction.h>
#include <Caracal/IR/ValueRef.h>

#include <string>

namespace Caracal
{
    class FieldAddressInstruction final : public Instruction
    {
    public:
        FieldAddressInstruction(
            TemporaryId resultId, 
            ValueRef objectAddress,
            Type objectType,
            std::string fieldName, 
            i32 fieldIndex,
            Type type) noexcept;

        [[nodiscard]] TemporaryId resultId() const noexcept { return m_resultId; }
        [[nodiscard]] ValueRef objectAddress() const noexcept { return m_objectAddress; }
        [[nodiscard]] Type objectType() const noexcept { return m_objectType; }
        [[nodiscard]] const std::string& fieldName() const noexcept { return m_fieldName; }
        [[nodiscard]] i32 fieldIndex() const noexcept { return m_fieldIndex; }
        [[nodiscard]] Type type() const noexcept { return m_type; }

    private:
        TemporaryId m_resultId;
        ValueRef m_objectAddress;
        Type m_objectType;
        std::string m_fieldName;
        i32 m_fieldIndex;
        Type m_type;
    };
}
