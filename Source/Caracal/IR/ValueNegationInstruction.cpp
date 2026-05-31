#include <Caracal/IR/ValueNegationInstruction.h>

namespace Caracal
{
    ValueNegationInstruction::ValueNegationInstruction(TemporaryId resultId, ValueRef operandValue, Type type) noexcept
        : Instruction{ InstructionKind::ValueNegation }
        , m_resultId{ resultId }
        , m_operandValue{ operandValue }
        , m_type{ type }
    {
    }
}
