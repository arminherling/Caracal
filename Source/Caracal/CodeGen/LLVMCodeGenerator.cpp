#include <Caracal/CodeGen/LLVMCodeGenerator.h>

#include <Caracal/IR/AddInstruction.h>
#include <Caracal/IR/AddressOfGlobalInstruction.h>
#include <Caracal/IR/AddressOfInstruction.h>
#include <Caracal/IR/AllocateLocalInstruction.h>
#include <Caracal/IR/BasicBlock.h>
#include <Caracal/IR/BranchIfTerminator.h>
#include <Caracal/IR/ConstantInstruction.h>
#include <Caracal/IR/GlobalConstantDeclaration.h>
#include <Caracal/IR/GlobalReferenceDeclaration.h>
#include <Caracal/IR/DivideInstruction.h>
#include <Caracal/IR/EqualInstruction.h>
#include <Caracal/IR/Function.h>
#include <Caracal/IR/GreaterOrEqualInstruction.h>
#include <Caracal/IR/GreaterThanInstruction.h>
#include <Caracal/IR/JumpTerminator.h>
#include <Caracal/IR/LessOrEqualInstruction.h>
#include <Caracal/IR/LessThanInstruction.h>
#include <Caracal/IR/LoadValueInstruction.h>
#include <Caracal/IR/LogicalAndInstruction.h>
#include <Caracal/IR/LogicalNegationInstruction.h>
#include <Caracal/IR/LogicalOrInstruction.h>
#include <Caracal/IR/MultiplyInstruction.h>
#include <Caracal/IR/NotEqualInstruction.h>
#include <Caracal/IR/ParameterInstruction.h>
#include <Caracal/IR/PhiInstruction.h>
#include <Caracal/IR/ReturnValueTerminator.h>
#include <Caracal/IR/StoreValueInstruction.h>
#include <Caracal/IR/SubtractInstruction.h>
#include <Caracal/IR/Terminator.h>
#include <Caracal/IR/ValueNegationInstruction.h>

#include <llvm/ADT/APFloat.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <cstdint>
#include <type_traits>
#include <variant>
#include <vector>

namespace Caracal
{
    LLVMCodeGenerator::LLVMCodeGenerator(const Module& irModule, llvm::Module& llvmModule)
        : m_irModule{ irModule }
        , m_llvmModule{ llvmModule }
        , m_irBuilder{ std::make_unique<llvm::IRBuilder<>>(llvmModule.getContext()) }
        , m_currentFunction{ nullptr }
    {
    }

    // defaulted here so the unique_ptr<llvm::IRBuilderBase> member is
    // destroyed where the type is complete
    LLVMCodeGenerator::~LLVMCodeGenerator() = default;

    bool LLVMCodeGenerator::generate()
    {
        for (const auto& globalConstant : m_irModule.globalConstants())
        {
            if (!lowerGlobalConstant(globalConstant))
                return false;
        }

        for (const auto& globalReference : m_irModule.globalReferences())
        {
            if (!lowerGlobalReference(globalReference))
                return false;
        }

        for (const auto& function : m_irModule.functions())
        {
            if (!lowerFunction(function))
                return false;
        }

        return true;
    }

    bool LLVMCodeGenerator::lowerGlobalConstant(const GlobalConstantDeclaration& globalConstant) noexcept
    {
        auto* initializer = llvm::dyn_cast_or_null<llvm::Constant>(lowerConstant(globalConstant.value()));
        if (initializer == nullptr)
            return false;

        new llvm::GlobalVariable(
            m_llvmModule,
            initializer->getType(),
            true, // is constant
            llvm::GlobalValue::ExternalLinkage,
            initializer,
            globalConstant.name());
        return true;
    }

    bool LLVMCodeGenerator::lowerGlobalReference(const GlobalReferenceDeclaration& globalReference) noexcept
    {
        auto* target = m_llvmModule.getNamedGlobal(globalReference.targetName());
        if (target == nullptr)
            return false;

        // a global reference is the constant address of another global
        new llvm::GlobalVariable(
            m_llvmModule,
            target->getType(),
            true, // is constant
            llvm::GlobalValue::ExternalLinkage,
            target,
            globalReference.name());
        return true;
    }

    bool LLVMCodeGenerator::lowerFunction(const Function& function) noexcept
    {
        m_values.clear();
        m_slots.clear();
        m_blocks.clear();
        m_pendingPhis.clear();

        auto* returnType = lowerType(function.returnType());
        if (returnType == nullptr)
            return false;

        std::vector<llvm::Type*> parameterTypes;
        for (const auto& parameter : function.parameters())
        {
            auto* parameterType = lowerType(parameter.type());
            if (parameterType == nullptr)
                return false;

            parameterTypes.push_back(parameterType);
        }

        auto* functionType = llvm::FunctionType::get(returnType, parameterTypes, false);
        m_currentFunction = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, function.name(), m_llvmModule);

        if (!function.hasBlocks())
            return false;

        // pre-create every block (the first is the entry) so jumps, branches and phis can reference them
        auto& context = m_llvmModule.getContext();
        for (const auto& block : function.blocks())
            m_blocks[block->id()] = llvm::BasicBlock::Create(context, block->label(), m_currentFunction);

        for (const auto& block : function.blocks())
        {
            m_irBuilder->SetInsertPoint(m_blocks[block->id()]);

            for (const auto& instruction : block->instructions())
            {
                if (!lowerInstruction(*instruction))
                    return false;
            }

            if (!block->hasTerminator())
                return false;

            if (!lowerTerminator(*block->terminator()))
                return false;
        }

        // wire up phi incomings now that every block is built and every value is defined
        for (const auto& [phi, node] : m_pendingPhis)
        {
            for (const auto& input : phi->inputs())
            {
                auto* incomingValue = tryResolve(input.value());
                auto* incomingBlock = tryGetBlock(input.blockId());
                if (incomingValue == nullptr || incomingBlock == nullptr)
                    return false;

                node->addIncoming(incomingValue, incomingBlock);
            }
        }

        return true;
    }

    bool LLVMCodeGenerator::lowerInstruction(const Instruction& instruction) noexcept
    {
        switch (instruction.kind())
        {
            case InstructionKind::Parameter:
            {
                const auto& parameter = static_cast<const ParameterInstruction&>(instruction);
                defineValue(parameter.resultId(), m_currentFunction->getArg(static_cast<unsigned>(parameter.parameterIndex())));
                return true;
            }
            case InstructionKind::Constant:
            {
                const auto& constant = static_cast<const ConstantInstruction&>(instruction);
                auto* value = lowerConstant(constant.value());
                if (value == nullptr)
                    return false;

                defineValue(constant.resultId(), value);
                return true;
            }
            case InstructionKind::AddressOfGlobal:
            {
                const auto& addressOfGlobal = static_cast<const AddressOfGlobalInstruction&>(instruction);
                auto* global = m_llvmModule.getNamedGlobal(addressOfGlobal.name());
                if (global == nullptr)
                    return false;

                defineValue(addressOfGlobal.resultId(), global);
                return true;
            }
            case InstructionKind::AllocateLocal:
            {
                const auto& allocate = static_cast<const AllocateLocalInstruction&>(instruction);
                auto* slotType = lowerType(allocate.type());
                if (slotType == nullptr)
                    return false;

                m_slots[allocate.resultId()] = m_irBuilder->CreateAlloca(slotType, nullptr, allocate.localName());
                return true;
            }
            case InstructionKind::AddressOf:
            {
                // the address of a slot is just the slot's alloca pointer, no instruction is emitted
                const auto& addressOf = static_cast<const AddressOfInstruction&>(instruction);
                const auto slot = m_slots.find(addressOf.local().id());
                if (slot == m_slots.end())
                    return false;

                defineValue(addressOf.resultId(), slot->second);
                return true;
            }
            case InstructionKind::LoadValue:
            {
                const auto& load = static_cast<const LoadValueInstruction&>(instruction);
                auto* address = tryResolve(load.address());
                auto* valueType = lowerType(load.type());
                if (address == nullptr || valueType == nullptr)
                    return false;

                defineValue(load.resultId(), m_irBuilder->CreateLoad(valueType, address, "load"));
                return true;
            }
            case InstructionKind::StoreValue:
            {
                const auto& store = static_cast<const StoreValueInstruction&>(instruction);
                auto* value = tryResolve(store.value());
                auto* address = tryResolve(store.address());
                if (value == nullptr || address == nullptr)
                    return false;

                m_irBuilder->CreateStore(value, address);
                return true;
            }
            case InstructionKind::Phi:
            {
                // create the empty node now; its incomings are wired up in lowerFunction once all blocks exist
                const auto& phi = static_cast<const PhiInstruction&>(instruction);
                auto* phiType = lowerType(phi.type());
                if (phiType == nullptr)
                    return false;

                auto* node = m_irBuilder->CreatePHI(phiType, static_cast<unsigned>(phi.inputs().size()), "phi");
                defineValue(phi.resultId(), node);
                m_pendingPhis.emplace_back(&phi, node);
                return true;
            }
            case InstructionKind::Add:
            {
                const auto& binary = static_cast<const AddInstruction&>(instruction);
                return emitBinary(binary.resultId(), binary.leftValue(), binary.rightValue(), instruction.kind());
            }
            case InstructionKind::Subtract:
            {
                const auto& binary = static_cast<const SubtractInstruction&>(instruction);
                return emitBinary(binary.resultId(), binary.leftValue(), binary.rightValue(), instruction.kind());
            }
            case InstructionKind::Multiply:
            {
                const auto& binary = static_cast<const MultiplyInstruction&>(instruction);
                return emitBinary(binary.resultId(), binary.leftValue(), binary.rightValue(), instruction.kind());
            }
            case InstructionKind::Divide:
            {
                const auto& binary = static_cast<const DivideInstruction&>(instruction);
                return emitBinary(binary.resultId(), binary.leftValue(), binary.rightValue(), instruction.kind());
            }
            case InstructionKind::Equal:
            {
                const auto& binary = static_cast<const EqualInstruction&>(instruction);
                return emitBinary(binary.resultId(), binary.leftValue(), binary.rightValue(), instruction.kind());
            }
            case InstructionKind::NotEqual:
            {
                const auto& binary = static_cast<const NotEqualInstruction&>(instruction);
                return emitBinary(binary.resultId(), binary.leftValue(), binary.rightValue(), instruction.kind());
            }
            case InstructionKind::LessThan:
            {
                const auto& binary = static_cast<const LessThanInstruction&>(instruction);
                return emitBinary(binary.resultId(), binary.leftValue(), binary.rightValue(), instruction.kind());
            }
            case InstructionKind::LessOrEqual:
            {
                const auto& binary = static_cast<const LessOrEqualInstruction&>(instruction);
                return emitBinary(binary.resultId(), binary.leftValue(), binary.rightValue(), instruction.kind());
            }
            case InstructionKind::GreaterThan:
            {
                const auto& binary = static_cast<const GreaterThanInstruction&>(instruction);
                return emitBinary(binary.resultId(), binary.leftValue(), binary.rightValue(), instruction.kind());
            }
            case InstructionKind::GreaterOrEqual:
            {
                const auto& binary = static_cast<const GreaterOrEqualInstruction&>(instruction);
                return emitBinary(binary.resultId(), binary.leftValue(), binary.rightValue(), instruction.kind());
            }
            case InstructionKind::LogicalAnd:
            {
                const auto& binary = static_cast<const LogicalAndInstruction&>(instruction);
                return emitBinary(binary.resultId(), binary.leftValue(), binary.rightValue(), instruction.kind());
            }
            case InstructionKind::LogicalOr:
            {
                const auto& binary = static_cast<const LogicalOrInstruction&>(instruction);
                return emitBinary(binary.resultId(), binary.leftValue(), binary.rightValue(), instruction.kind());
            }
            case InstructionKind::ValueNegation:
            {
                const auto& negation = static_cast<const ValueNegationInstruction&>(instruction);
                auto* operand = tryResolve(negation.operandValue());
                if (operand == nullptr)
                    return false;

                llvm::Value* result = nullptr;
                if (operand->getType()->isFloatingPointTy())
                    result = m_irBuilder->CreateFNeg(operand);
                else
                    result = m_irBuilder->CreateNeg(operand);

                defineValue(negation.resultId(), result);
                return true;
            }
            case InstructionKind::LogicalNegation:
            {
                const auto& negation = static_cast<const LogicalNegationInstruction&>(instruction);
                auto* operand = tryResolve(negation.operandValue());
                if (operand == nullptr)
                    return false;

                defineValue(negation.resultId(), m_irBuilder->CreateNot(operand));
                return true;
            }
            default:
            {
                // todo other stuff
                return false;
            }
        }
    }

    bool LLVMCodeGenerator::emitBinary(TemporaryId resultId, ValueRef leftRef, ValueRef rightRef, InstructionKind kind) noexcept
    {
        auto* lhs = tryResolve(leftRef);
        auto* rhs = tryResolve(rightRef);
        if (lhs == nullptr || rhs == nullptr)
            return false;


        // string/pointer equality needs a strcmp (lands in 3.5); never silently emit a pointer compare
        if ((kind == InstructionKind::Equal || kind == InstructionKind::NotEqual) && lhs->getType()->isPointerTy())
            return false;

        const auto isFloat = lhs->getType()->isFloatingPointTy();
        if (isFloat)
        {
            switch (kind)
            {
                case InstructionKind::Add:
                {
                    defineValue(resultId, m_irBuilder->CreateFAdd(lhs, rhs, "add"));
                    return true;
                }
                case InstructionKind::Subtract:
                {
                    defineValue(resultId, m_irBuilder->CreateFSub(lhs, rhs, "subtract"));
                    return true;
                }
                case InstructionKind::Multiply:
                {
                    defineValue(resultId, m_irBuilder->CreateFMul(lhs, rhs, "multiply"));
                    return true;
                }
                case InstructionKind::Divide:
                {
                    defineValue(resultId, m_irBuilder->CreateFDiv(lhs, rhs, "divide"));
                    return true;
                }
                case InstructionKind::Equal:
                {
                    defineValue(resultId, m_irBuilder->CreateFCmpUEQ(lhs, rhs, "equal"));
                    return true;
                }
                case InstructionKind::NotEqual:
                {
                    defineValue(resultId, m_irBuilder->CreateFCmpUNE(lhs, rhs, "not_equal"));
                    return true;
                }
                case InstructionKind::LessThan:
                {
                    defineValue(resultId, m_irBuilder->CreateFCmpULT(lhs, rhs, "less_than"));
                    return true;
                }
                case InstructionKind::LessOrEqual:
                {
                    defineValue(resultId, m_irBuilder->CreateFCmpULE(lhs, rhs, "less_or_equal"));
                    return true;
                }
                case InstructionKind::GreaterThan:
                {
                    defineValue(resultId, m_irBuilder->CreateFCmpUGT(lhs, rhs, "greater_than"));
                    return true;
                }
                case InstructionKind::GreaterOrEqual:
                {
                    defineValue(resultId, m_irBuilder->CreateFCmpUGE(lhs, rhs, "greater_or_equal"));
                    return true;
                }
                default:
                {
                    return false;
                }
            }
        }
        else
        {
            switch (kind)
            {
                case InstructionKind::Add:
                {
                    defineValue(resultId, m_irBuilder->CreateAdd(lhs, rhs, "add"));
                    return true;
                }
                case InstructionKind::Subtract:
                {
                    defineValue(resultId, m_irBuilder->CreateSub(lhs, rhs, "subtract"));
                    return true;
                }
                case InstructionKind::Multiply:
                {
                    defineValue(resultId, m_irBuilder->CreateMul(lhs, rhs, "multiply"));
                    return true;
                }
                case InstructionKind::Divide:
                {
                    defineValue(resultId, m_irBuilder->CreateSDiv(lhs, rhs, "divide"));
                    return true;
                }
                case InstructionKind::Equal:
                {
                    defineValue(resultId, m_irBuilder->CreateICmpEQ(lhs, rhs, "equal"));
                    return true;
                }
                case InstructionKind::NotEqual:
                {
                    defineValue(resultId, m_irBuilder->CreateICmpNE(lhs, rhs, "not_equal"));
                    return true;
                }
                case InstructionKind::LessThan:
                {
                    defineValue(resultId, m_irBuilder->CreateICmpSLT(lhs, rhs, "less_than"));
                    return true;
                }
                case InstructionKind::LessOrEqual:
                {
                    defineValue(resultId, m_irBuilder->CreateICmpSLE(lhs, rhs, "less_or_equal"));
                    return true;
                }
                case InstructionKind::GreaterThan:
                {
                    defineValue(resultId, m_irBuilder->CreateICmpSGT(lhs, rhs, "greater_than"));
                    return true;
                }
                case InstructionKind::GreaterOrEqual:
                {
                    defineValue(resultId, m_irBuilder->CreateICmpSGE(lhs, rhs, "greater_or_equal"));
                    return true;
                }
                case InstructionKind::LogicalAnd:
                {
                    defineValue(resultId, m_irBuilder->CreateAnd(lhs, rhs, "logical_and"));
                    return true;
                }
                case InstructionKind::LogicalOr:
                {
                    defineValue(resultId, m_irBuilder->CreateOr(lhs, rhs, "logical_or"));
                    return true;
                }
                default:
                {
                    return false;
                }
            }
        }
    }

    bool LLVMCodeGenerator::lowerTerminator(const Terminator& terminator) noexcept
    {
        switch (terminator.kind())
        {
            case TerminatorKind::Return:
            {
                m_irBuilder->CreateRetVoid();
                return true;
            }
            case TerminatorKind::ReturnValue:
            {
                const auto& returnValue = static_cast<const ReturnValueTerminator&>(terminator);
                auto* value = tryResolve(returnValue.value());
                if (value == nullptr)
                    return false;

                m_irBuilder->CreateRet(value);
                return true;
            }
            case TerminatorKind::Jump:
            {
                const auto& jump = static_cast<const JumpTerminator&>(terminator);
                auto* target = tryGetBlock(jump.targetBlockId());
                if (target == nullptr)
                    return false;

                m_irBuilder->CreateBr(target);
                return true;
            }
            case TerminatorKind::Branch:
            {
                const auto& branch = static_cast<const BranchIfTerminator&>(terminator);
                auto* condition = tryResolve(branch.condition());
                auto* trueBlock = tryGetBlock(branch.trueBlockId());
                auto* falseBlock = tryGetBlock(branch.falseBlockId());
                if (condition == nullptr || trueBlock == nullptr || falseBlock == nullptr)
                    return false;

                m_irBuilder->CreateCondBr(condition, trueBlock, falseBlock);
                return true;
            }
            default:
            {
                return false;
            }
        }
    }

    llvm::Type* LLVMCodeGenerator::lowerType(Type type) const noexcept
    {
        auto& context = m_llvmModule.getContext();

        if (type.isReference())
            return llvm::PointerType::getUnqual(context);
        else if (type == Type::Void())
            return llvm::Type::getVoidTy(context);
        else if (type == Type::Bool())
            return llvm::Type::getInt1Ty(context);
        else if (type == Type::U8())
            return llvm::Type::getInt8Ty(context);
        else if (type == Type::I32())
            return llvm::Type::getInt32Ty(context);
        else if (type == Type::F32())
            return llvm::Type::getFloatTy(context);
        else if (type == Type::String())
            return llvm::PointerType::getUnqual(context);

        return nullptr;
    }

    llvm::Value* LLVMCodeGenerator::lowerConstant(const ConstantValue& value) noexcept
    {
        // the llvm type follows the literal payload (which is the base type for enum constants),
        // not the declared type, so enum constants lower to their underlying integer type
        const auto* literalData = value.tryGetLiteralData();
        ConstantValue::LiteralData enumUnderlying;
        if (literalData == nullptr)
        {
            if (const auto* enumConstant = value.tryGetEnumConstant())
            {
                enumUnderlying = enumConstant->underlyingValue;
                literalData = &enumUnderlying;
            }
        }

        if (literalData == nullptr)
            return nullptr;

        auto& context = m_llvmModule.getContext();
        return std::visit(
            [&](const auto& payload) -> llvm::Value*
            {
                using Payload = std::decay_t<decltype(payload)>;

                if constexpr (std::is_same_v<Payload, bool>)
                {
                    if (payload)
                        return llvm::ConstantInt::getTrue(context);

                    return llvm::ConstantInt::getFalse(context);
                }
                else if constexpr (std::is_same_v<Payload, u8>)
                {
                    return llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), payload);
                }
                else if constexpr (std::is_same_v<Payload, i32>)
                {
                    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), static_cast<std::uint64_t>(payload), true);
                }
                else if constexpr (std::is_same_v<Payload, float>)
                {
                    return llvm::ConstantFP::get(context, llvm::APFloat(payload));
                }
                else if constexpr (std::is_same_v<Payload, std::string>)
                {
                    return m_irBuilder->CreateGlobalString(payload, "", 0, &m_llvmModule);
                }
                else
                {
                    return nullptr;
                }
            },
            *literalData);
    }

    llvm::Value* LLVMCodeGenerator::tryResolve(ValueRef value) const noexcept
    {
        const auto result = m_values.find(value.id());
        if (result == m_values.end())
            return nullptr;

        return result->second;
    }

    llvm::BasicBlock* LLVMCodeGenerator::tryGetBlock(BlockId id) const noexcept
    {
        const auto result = m_blocks.find(id);
        if (result == m_blocks.end())
            return nullptr;

        return result->second;
    }

    void LLVMCodeGenerator::defineValue(TemporaryId id, llvm::Value* value) noexcept
    {
        m_values.insert_or_assign(id, value);
    }
}
