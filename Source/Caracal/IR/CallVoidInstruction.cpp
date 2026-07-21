#include <Caracal/IR/CallVoidInstruction.h>

namespace Caracal
{
    CallVoidInstruction::CallVoidInstruction(FunctionId functionId, std::vector<ValueRef> arguments) noexcept
        : Instruction{ InstructionKind::CallVoid }
        , m_functionId{ functionId }
        , m_arguments{ std::move(arguments) }
    {
    }

    void CallVoidInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        for (auto& argument : m_arguments)
        {
            argument = ValueRef{ remapTemporaryId(remap, argument.id()) };
        }
    }
}
