#include <Caracal/IR/ArrayAddInstruction.h>

namespace Caracal
{
    ArrayAddInstruction::ArrayAddInstruction(ValueRef descriptorAddress, ValueRef value, Type arrayType, FunctionId reallocFunctionId) noexcept
        : Instruction{ InstructionKind::ArrayAdd }
        , m_descriptorAddress{ descriptorAddress }
        , m_value{ value }
        , m_arrayType{ arrayType }
        , m_reallocFunctionId{ reallocFunctionId }
    {
    }

    void ArrayAddInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_descriptorAddress = ValueRef{ remapTemporaryId(remap, m_descriptorAddress.id()) };
        m_value = ValueRef{ remapTemporaryId(remap, m_value.id()) };
    }
}
