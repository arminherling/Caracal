#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>
#include <Caracal/Semantic/Type.h>

#include <memory>

namespace Caracal
{
    using TemporaryId = i32;
    using LocalSlotId = i32;
    using BlockId = i32;
    using FunctionId = i32;
    using EnumId = i32;

    enum class InstructionKind
    {
        Parameter,
        Constant,
        AllocateLocal,
        AddressOf,
        AddressOfGlobal,
        FieldAddress,
        LoadValue,
        StoreValue,
        ValueNegation,
        LogicalNegation,
        Call,
        CallVoid,
        Add,
        Subtract,
        Multiply,
        Divide,
        Equal,
        NotEqual,
        LessThan,
        LessOrEqual,
        GreaterThan,
        GreaterOrEqual,
        LogicalAnd,
        LogicalOr,
        Phi,
    };

    enum class TerminatorKind
    {
        Jump,
        Branch,
        Return,
        ReturnValue,
        Unreachable,
    };

    class CARACAL_API Instruction
    {
    public:
        explicit Instruction(InstructionKind kind) noexcept;
        virtual ~Instruction() = default;

        [[nodiscard]] InstructionKind kind() const noexcept { return m_kind; }

    private:
        InstructionKind m_kind;
    };

    using InstructionUPtr = std::unique_ptr<Instruction>;
}
