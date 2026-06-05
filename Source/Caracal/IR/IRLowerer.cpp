#include <Caracal/IR/IRLowerer.h>

#include <Caracal/IR/AddInstruction.h>
#include <Caracal/IR/AddressOfInstruction.h>
#include <Caracal/IR/AllocateLocalInstruction.h>
#include <Caracal/IR/CallInstruction.h>
#include <Caracal/IR/CallVoidInstruction.h>
#include <Caracal/IR/ConstantValue.h>
#include <Caracal/IR/ConstantInstruction.h>
#include <Caracal/IR/DivideInstruction.h>
#include <Caracal/IR/EqualInstruction.h>
#include <Caracal/IR/FieldAddressInstruction.h>
#include <Caracal/IR/GreaterOrEqualInstruction.h>
#include <Caracal/IR/GreaterThanInstruction.h>
#include <Caracal/IR/BranchIfTerminator.h>
#include <Caracal/IR/JumpTerminator.h>
#include <Caracal/IR/LessOrEqualInstruction.h>
#include <Caracal/IR/LessThanInstruction.h>
#include <Caracal/IR/LoadValueInstruction.h>
#include <Caracal/IR/LogicalNegationInstruction.h>
#include <Caracal/IR/MultiplyInstruction.h>
#include <Caracal/IR/NotEqualInstruction.h>
#include <Caracal/IR/ParameterInstruction.h>
#include <Caracal/IR/PhiInstruction.h>
#include <Caracal/IR/ReturnTerminator.h>
#include <Caracal/IR/ReturnValueTerminator.h>
#include <Caracal/IR/StoreValueInstruction.h>
#include <Caracal/IR/SubtractInstruction.h>
#include <Caracal/IR/ValueNegationInstruction.h>
#include <Caracal/Semantic/FunctionDefinition.h>
#include <Caracal/Syntax/AssignmentStatement.h>
#include <Caracal/Syntax/BinaryExpression.h>
#include <Caracal/Syntax/BlockNode.h>
#include <Caracal/Syntax/BoolLiteral.h>
#include <Caracal/Syntax/ConstantDeclaration.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/GroupingExpression.h>
#include <Caracal/Syntax/IfStatement.h>
#include <Caracal/Syntax/MemberAccessExpression.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/NodeKind.h>
#include <Caracal/Syntax/NumberLiteral.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Syntax/ReturnStatement.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/StringLiteral.h>
#include <Caracal/Syntax/UnaryExpression.h>
#include <Caracal/Syntax/VariableDeclaration.h>
#include <Caracal/Syntax/WhileStatement.h>

#include <algorithm>
#include <optional>
#include <variant>

namespace Caracal
{
    template <typename TResult, typename TVisitor>
    static std::optional<TResult> TryVisitLiteralData(const ConstantValue& value, TVisitor&& visitor) noexcept
    {
        const auto* literalData = value.tryGetLiteralData();
        if (literalData == nullptr)
            return std::nullopt;

        return std::visit(
            [&](const auto& payload) -> std::optional<TResult>
            {
                return visitor(payload);
            },
            *literalData);
    }

    template <typename TValue>
    static std::optional<ConstantValue> ConstantFold(BinaryOperatorKind operation, TValue lhs, TValue rhs) noexcept
    {
        switch (operation)
        {
            case BinaryOperatorKind::Addition:
                return ConstantValue::FromLiteralData(ConstantValue::LiteralData{ static_cast<TValue>(lhs + rhs) });
            case BinaryOperatorKind::Subtraction:
                return ConstantValue::FromLiteralData(ConstantValue::LiteralData{ static_cast<TValue>(lhs - rhs) });
            case BinaryOperatorKind::Multiplication:
                return ConstantValue::FromLiteralData(ConstantValue::LiteralData{ static_cast<TValue>(lhs * rhs) });
            case BinaryOperatorKind::Division:
                if (rhs == static_cast<TValue>(0))
                    return std::nullopt;

                return ConstantValue::FromLiteralData(ConstantValue::LiteralData{ static_cast<TValue>(lhs / rhs) });
            default:
                return std::nullopt;
        }
    }

    static BasicBlock* TryGetCurrentBlock(Function& function, const std::optional<BlockId>& blockId) noexcept
    {
        if (!blockId.has_value())
            return nullptr;

        return function.tryGetBlock(blockId.value());
    }

    static std::optional<ConstantValue> CreateConstantValue(const NumberLiteral& literal) noexcept
    {
        if (!literal.hasParsedValue())
            return std::nullopt;

        const auto baseType = literal.type().toBaseType();
        const auto& parsedValue = literal.parsedValue().value();

        if (baseType == Type::U8())
            return ConstantValue::FromU8(std::get<u8>(parsedValue));

        if (baseType == Type::I32())
            return ConstantValue::FromI32(std::get<i32>(parsedValue));

        if (baseType == Type::F32())
            return ConstantValue::FromF32(std::get<f32>(parsedValue));

        return std::nullopt;
    }

    static std::optional<ConstantValue> ConstantFoldUnary(UnaryOperatorKind operation, const ConstantValue& value) noexcept
    {
        return TryVisitLiteralData<ConstantValue>(
            value,
            [operation](const auto& payload) -> std::optional<ConstantValue>
            {
                using Payload = std::decay_t<decltype(payload)>;

                switch (operation)
                {
                    case UnaryOperatorKind::ValueNegation:
                        if constexpr (std::is_same_v<Payload, i32> || std::is_same_v<Payload, float>)
                            return ConstantValue::FromLiteralData(ConstantValue::LiteralData{ -payload });
                        break;
                    case UnaryOperatorKind::LogicalNegation:
                        if constexpr (std::is_same_v<Payload, bool>)
                            return ConstantValue::FromBool(!payload);
                        break;
                    default:
                        break;
                }

                return std::nullopt;
            });
    }

    static std::optional<ConstantValue> ConstantFoldBinary(
        BinaryOperatorKind operation,
        const ConstantValue& left,
        const ConstantValue& right) noexcept
    {
        const auto* leftData = left.tryGetLiteralData();
        const auto* rightData = right.tryGetLiteralData();
        if (leftData == nullptr || rightData == nullptr)
            return std::nullopt;

        return std::visit(
            [operation](const auto& lhs, const auto& rhs) -> std::optional<ConstantValue>
            {
                using Left = std::decay_t<decltype(lhs)>;
                using Right = std::decay_t<decltype(rhs)>;

                if constexpr (!std::is_same_v<Left, Right>)
                {
                    return std::nullopt;
                }
                else if constexpr (std::is_same_v<Left, u8> || std::is_same_v<Left, i32> || std::is_same_v<Left, float>)
                {
                    return ConstantFold(operation, lhs, rhs);
                }
                else
                {
                    return std::nullopt;
                }
            },
            *leftData,
            *rightData);
    }

    static std::optional<ConstantValue> CreateEnumConstantValue(Type baseType, i32 value) noexcept
    {
        const auto normalizedBaseType = baseType.toBaseType();
        if (normalizedBaseType == Type::Bool())
            return ConstantValue::FromBool(value != 0);

        if (normalizedBaseType == Type::U8())
            return ConstantValue::FromU8(static_cast<u8>(value));

        if (normalizedBaseType == Type::I32())
            return ConstantValue::FromI32(value);

        if (normalizedBaseType == Type::F32())
            return ConstantValue::FromF32(static_cast<f32>(value));

        return std::nullopt;
    }

    IRLowerer::IRLowerer(SemanticContext& semanticModule)
        : m_semanticModule{ semanticModule }
    {
    }

    bool IRLowerer::lower(Module& module) noexcept
    {
        resetState();

        for (const auto& enumDefinition : m_semanticModule.enumDefinitions())
        {
            if (enumDefinition.statement() == nullptr)
                continue;

            if (!lowerEnumDefinition(enumDefinition, module))
                return false;
        }

        for (const auto& typeDefinition : m_semanticModule.typeDefinitions())
        {
            if (typeDefinition.statement() == nullptr)
                continue;

            if (!lowerTypeDefinition(typeDefinition, module))
                return false;
        }

        for (const auto& functionDefinition : m_semanticModule.functionDefinitions())
        {
            if (functionDefinition.functionType() == FunctionType::SynthesizedConstructor)
            {
                if (!lowerSynthesizedConstructorDefinition(functionDefinition, module))
                    return false;

                continue;
            }

            const auto* statement = functionDefinition.statement();
            switch (statement->kind())
            {
                case NodeKind::FunctionDefinitionStatement:
                {
                    const auto* functionStatement = static_cast<const FunctionDefinitionStatement*>(statement);
                    if (!lowerFunctionDefinition(functionDefinition, functionStatement->bodyNode().get(), functionStatement->isExtern(), module))
                        return false;

                    break;
                }
                case NodeKind::MethodDefinitionStatement:
                {
                    const auto* methodStatement = static_cast<const MethodDefinitionStatement*>(statement);
                    if (!lowerFunctionDefinition(functionDefinition, methodStatement->bodyNode().get(), false, module))
                        return false;

                    break;
                }
                default:
                    break;
            }
        }

        return true;
    }

    bool IRLowerer::lowerSynthesizedConstructorDefinition(const FunctionDefinition& definition, Module& module) noexcept
    {
        resetState();

        std::vector<IRParameter> parameters;
        for (const auto& parameter : definition.parameters())
        {
            parameters.emplace_back(parameter.name(), parameter.type());
        }

        const auto functionId = definition.type().id();
        auto* function = module.addFunction(Function{ functionId, definition.fullName(), parameters, Type::Void() });
        auto blockId = m_nextBlockId++;
        auto entryBlock = BasicBlock{ blockId, "entry", std::make_unique<ReturnTerminator>() };
        lowerParameters(definition, entryBlock);

        const auto thisResult = m_localValues.find("this");
        if (thisResult == m_localValues.end() || thisResult->second.storageKind != LocalStorageKind::Address)
            return false;

        const auto& typeDefinition = m_semanticModule.getTypeDefinition(definition.parentType());
        for (const auto& fieldDefinition : typeDefinition.fields())
        {
            if (fieldDefinition.expression() == nullptr)
                continue;

            const auto loweredValue = lowerValueExpression(fieldDefinition.expression(), entryBlock);
            if (!loweredValue.has_value())
                continue;

            const auto addressId = m_nextTemporaryId++;
            entryBlock.addInstruction(std::make_unique<FieldAddressInstruction>(
                addressId,
                thisResult->second.value,
                definition.parentType(),
                fieldDefinition.name(),
                fieldDefinition.index(),
                fieldDefinition.type().toReference()));
            entryBlock.addInstruction(std::make_unique<StoreValueInstruction>(
                loweredValue.value(),
                ValueRef{ addressId },
                fieldDefinition.type()));
        }

        function->addBlock(std::move(entryBlock));
        return true;
    }

    bool IRLowerer::lowerEnumDefinition(const EnumDefinition& definition, Module& module) noexcept
    {
        const auto enumType = definition.type();
        if (enumType == Type::Undefined())
            return false;

        auto enumDeclaration = EnumDeclaration{ definition.name(), enumType, definition.baseType() };
        for (const auto& field : definition.fields())
        {
            auto loweredFieldValue = tryLowerEnumFieldValue(enumType, field.name());
            if (!loweredFieldValue.has_value())
                return false;

            enumDeclaration.addField(field.name(), loweredFieldValue.value());
        }

        module.addEnum(std::move(enumDeclaration));
        return true;
    }

    bool IRLowerer::lowerTypeDefinition(const TypeDefinition& definition, Module& module) noexcept
    {
        const auto typeType = definition.type();
        if (typeType == Type::Undefined())
            return false;

        TypeDeclaration typeDeclaration{ definition.name(), typeType };
        for (const auto& fieldDefinition : definition.fields())
        {
            typeDeclaration.addField(fieldDefinition.name(), fieldDefinition.type(), fieldDefinition.isConstant());
        }

        module.addType(std::move(typeDeclaration));
        return true;
    }

    bool IRLowerer::lowerFunctionDefinition(const FunctionDefinition& definition, const BlockNode* bodyNode, bool isExtern, Module& module) noexcept
    {
        resetState();

        auto returnType = Type::Void();
        if (!definition.returnTypes().empty())
        {
            returnType = definition.returnTypes().front();
        }

        std::vector<IRParameter> parameters;
        for (const auto& parameter : definition.parameters())
        {
            parameters.emplace_back(parameter.name(), parameter.type());
        }

        const auto& functionName = definition.fullName();
        const auto functionId = definition.type().id();
        if (isExtern)
        {
            module.addExternFunction(ExternFunction{ functionId, functionName, parameters, returnType });
            return true;
        }

        auto* function = module.addFunction(Function{ functionId, functionName, parameters, returnType });
        auto blockId = m_nextBlockId++;
        auto entryBlock = BasicBlock{ blockId, "entry", std::make_unique<ReturnTerminator>() };
        lowerParameters(definition, entryBlock);
        function->addBlock(std::move(entryBlock));

        std::optional<BlockId> entryBlockId = blockId;
        if (!lowerBlock(bodyNode, *function, entryBlockId))
            return false;

        return true;
    }

    void IRLowerer::lowerParameters(const FunctionDefinition& definition, BasicBlock& block) noexcept
    {
        const auto& parameters = definition.parameters();
        for (size_t index = 0; index < parameters.size(); ++index)
        {
            const auto parameterId = m_nextTemporaryId++;
            const auto parameterType = parameters[index].type();
            block.addInstruction(std::make_unique<ParameterInstruction>(
                parameterId,
                static_cast<i32>(index),
                IRParameter{ parameters[index].name(), parameterType}));

            const auto& parameterName = parameters[index].name();
            const auto storageKind = parameterType.isReference() ? LocalStorageKind::Address : LocalStorageKind::Value;
            m_localValues.emplace(
                parameterName,
                LocalState{ ValueRef{ parameterId }, parameterType, storageKind });
        }
    }

    bool IRLowerer::lowerStatement(const Statement* statement, Function& function, std::optional<BlockId>& currentBlockId) noexcept
    {
        if (!currentBlockId.has_value())
            return true;

        auto* currentBlock = function.tryGetBlock(currentBlockId.value());
        if (currentBlock == nullptr)
            return false;

        switch (statement->kind())
        {
            case NodeKind::BlockNode:
            {
                const auto* blockNode = static_cast<const BlockNode*>(statement);
                return lowerBlock(blockNode, function, currentBlockId);
            }
            case NodeKind::VariableDeclaration:
            {
                const auto* variableDeclaration = static_cast<const VariableDeclaration*>(statement);
                return lowerLocalDeclaration(
                    variableDeclaration->leftExpression().get(),
                    variableDeclaration->rightExpression().get(),
                    *currentBlock);
            }
            case NodeKind::ConstantDeclaration:
            {
                const auto* constantDeclaration = static_cast<const ConstantDeclaration*>(statement);
                return lowerLocalDeclaration(
                    constantDeclaration->leftExpression().get(),
                    constantDeclaration->rightExpression().get(),
                    *currentBlock);
            }
            case NodeKind::AssignmentStatement:
            {
                const auto* assignmentStatement = static_cast<const AssignmentStatement*>(statement);
                return lowerAssignmentStatement(
                    assignmentStatement->leftExpression().get(),
                    assignmentStatement->rightExpression().get(),
                    *currentBlock);
            }
            case NodeKind::ExpressionStatement:
            {
                const auto* expressionStatement = static_cast<const ExpressionStatement*>(statement);
                return lowerExpressionStatement(expressionStatement, *currentBlock);
            }
            case NodeKind::IfStatement:
            {
                const auto* ifStatement = static_cast<const IfStatement*>(statement);
                return lowerIfStatement(ifStatement, function, currentBlockId);
            }
            case NodeKind::WhileStatement:
            {
                const auto* whileStatement = static_cast<const WhileStatement*>(statement);
                return lowerWhileStatement(whileStatement, function, currentBlockId);
            }
            case NodeKind::BreakStatement:
            {
                return lowerBreakStatement(*currentBlock, currentBlockId);
            }
            case NodeKind::SkipStatement:
            {
                return lowerSkipStatement(*currentBlock, currentBlockId);
            }
            case NodeKind::ReturnStatement:
            {
                const auto* returnStatement = static_cast<const ReturnStatement*>(statement);
                return lowerReturnStatement(returnStatement, *currentBlock);
            }
            default:
            {
                return false;
            }
        }
    }

    bool IRLowerer::lowerBlock(const BlockNode* block, Function& function, std::optional<BlockId>& currentBlockId) noexcept
    {
        for (const auto& statement : block->statements())
        {
            if (!lowerStatement(statement.get(), function, currentBlockId))
                return false;
        }

        return true;
    }

    bool IRLowerer::lowerIfStatement(const IfStatement* statement, Function& function, std::optional<BlockId>& currentBlockId) noexcept
    {
        auto* currentBlock = TryGetCurrentBlock(function, currentBlockId);
        if (currentBlock == nullptr)
            return false;

        // copy the locals before branching so each path can be lowered from the same state
        const auto preBranchValues = m_localValues;

        const auto conditionValue = lowerValueExpression(statement->condition().get(), *currentBlock);
        if (!conditionValue.has_value())
            return false;

        const auto trueId = m_nextBlockId++;
        function.addBlock(BasicBlock{ trueId, "if.true", nullptr });

        if (!statement->hasFalseBlock())
        {
            const auto continuationId = m_nextBlockId++;
            currentBlock->setTerminator(std::make_unique<BranchIfTerminator>(conditionValue.value(), trueId, continuationId));

            restoreLocalValues(preBranchValues);
            std::optional<BlockId> trueBlockId = trueId;
            if (!lowerStatement(statement->trueStatement().get(), function, trueBlockId))
                return false;
            const auto trueExitValues = m_localValues;

            function.addBlock(BasicBlock{ continuationId, "if.continuation", nullptr });
            auto* continuationBlock = function.tryGetBlock(continuationId);
            if (continuationBlock == nullptr)
                return false;

            auto* trueBlock = TryGetCurrentBlock(function, trueBlockId);
            std::vector<IncomingLocalValues> continuationInputs;
            continuationInputs.push_back(IncomingLocalValues{ currentBlock->id(), preBranchValues });
            const auto needsContinuationJump = trueBlock != nullptr && !trueBlock->hasTerminator();
            if (needsContinuationJump)
            {
                trueBlock->setTerminator(std::make_unique<JumpTerminator>(continuationId));
                continuationInputs.push_back(IncomingLocalValues{ trueBlock->id(), trueExitValues });
            }

            mergeLocalValues(*continuationBlock, continuationInputs);

            currentBlockId = continuationId;
            return true;
        }

        const auto falseId = m_nextBlockId++;
        currentBlock->setTerminator(std::make_unique<BranchIfTerminator>(conditionValue.value(), trueId, falseId));

        // restpre values so later phi decisions are comparable
        restoreLocalValues(preBranchValues);
        std::optional<BlockId> trueBlockId = trueId;
        if (!lowerStatement(statement->trueStatement().get(), function, trueBlockId))
            return false;
        const auto trueExitValues = m_localValues;

        function.addBlock(BasicBlock{ falseId, "if.false", nullptr });

        restoreLocalValues(preBranchValues);
        std::optional<BlockId> falseBlockId = falseId;
        if (!lowerStatement(statement->falseStatement().value().get(), function, falseBlockId))
            return false;
        const auto falseExitValues = m_localValues;

        auto* trueBlock = TryGetCurrentBlock(function, trueBlockId);
        auto* falseBlock = TryGetCurrentBlock(function, falseBlockId);
        const auto trueFallsThrough = trueBlock != nullptr && !trueBlock->hasTerminator();
        const auto falseFallsThrough = falseBlock != nullptr && !falseBlock->hasTerminator();
        if (!trueFallsThrough && !falseFallsThrough)
        {
            currentBlockId.reset();
            return true;
        }

        const auto continuationId = m_nextBlockId++;
        function.addBlock(BasicBlock{ continuationId, "if.continuation", nullptr });
        auto* continuationBlock = function.tryGetBlock(continuationId);
        if (continuationBlock == nullptr)
            return false;

        std::vector<IncomingLocalValues> continuationInputs;
        if (trueFallsThrough)
        {
            trueBlock->setTerminator(std::make_unique<JumpTerminator>(continuationId));
            continuationInputs.push_back(IncomingLocalValues{ trueBlock->id(), trueExitValues });
        }
        if (falseFallsThrough)
        {
            falseBlock->setTerminator(std::make_unique<JumpTerminator>(continuationId));
            continuationInputs.push_back(IncomingLocalValues{ falseBlock->id(), falseExitValues });
        }

        mergeLocalValues(*continuationBlock, continuationInputs);

        currentBlockId = continuationId;
        return true;
    }

    bool IRLowerer::lowerWhileStatement(const WhileStatement* statement, Function& function, std::optional<BlockId>& currentBlockId) noexcept
    {
        auto* currentBlock = TryGetCurrentBlock(function, currentBlockId);
        if (currentBlock == nullptr)
            return false;

        // copy the locals before branching so loop condition can be lowered from the same state as the body
        const auto preLoopValues = m_localValues;
        const auto conditionId = m_nextBlockId++;
        const auto loopId = m_nextBlockId++;
        const auto continuationId = m_nextBlockId++;

        currentBlock->setTerminator(std::make_unique<JumpTerminator>(conditionId));

        function.addBlock(BasicBlock{ conditionId, "while.condition", nullptr });
        auto* conditionBlock = function.tryGetBlock(conditionId);
        if (conditionBlock == nullptr)
            return false;

        std::vector<std::string> headerNames;
        headerNames.reserve(preLoopValues.size());
        for (const auto& [name, localState] : preLoopValues)
        {
            if (localState.type == Type::Undefined())
                continue;

            headerNames.push_back(name);
        }
        std::sort(headerNames.begin(), headerNames.end());

        LocalStateMap loopHeaderValues;
        loopHeaderValues.reserve(headerNames.size());
        std::vector<LoopHeaderPhi> loopHeaderPhis;
        loopHeaderPhis.reserve(headerNames.size());
        for (const auto& name : headerNames)
        {
            const auto& localState = preLoopValues.at(name);
            const auto phiId = m_nextTemporaryId++;

            std::vector<PhiInput> phiInputs;
            phiInputs.emplace_back(currentBlock->id(), localState.value);

            auto phiInstruction = std::make_unique<PhiInstruction>(phiId, std::move(phiInputs), localState.type);
            auto* phiInstructionPtr = phiInstruction.get();
            conditionBlock->addInstruction(std::move(phiInstruction));

            loopHeaderValues.emplace(name, LocalState{ ValueRef{ phiId }, localState.type });
            loopHeaderPhis.push_back(LoopHeaderPhi{ name, phiInstructionPtr, localState.type });
        }

        restoreLocalValues(loopHeaderValues);

        const auto conditionValue = lowerValueExpression(statement->condition().get(), *conditionBlock);
        if (!conditionValue.has_value())
            return false;

        conditionBlock->setTerminator(std::make_unique<BranchIfTerminator>(conditionValue.value(), loopId, continuationId));

        function.addBlock(BasicBlock{ loopId, "while.body", nullptr });

        // add loop exit targets so break statements can contribute values for continuation phis
        m_loopContexts.push_back(LoopContext{ conditionId, continuationId, {}, {} });
        restoreLocalValues(loopHeaderValues);

        std::optional<BlockId> loopBlockId = loopId;
        const auto loweredBody = lowerStatement(statement->trueStatement().get(), function, loopBlockId);

        // copy the values added by any breaks before dropping the loop context
        auto conditionInputs = m_loopContexts.back().conditionInputs;
        auto continuationInputs = m_loopContexts.back().continuationInputs;
        m_loopContexts.pop_back();
        if (!loweredBody)
            return false;

        auto* loopBlock = TryGetCurrentBlock(function, loopBlockId);
        if (loopBlock != nullptr && !loopBlock->hasTerminator())
        {
            conditionInputs.push_back(IncomingLocalValues{ loopBlock->id(), m_localValues });
            loopBlock->setTerminator(std::make_unique<JumpTerminator>(conditionId));
        }

        for (const auto& loopHeaderPhi : loopHeaderPhis)
        {
            std::vector<PhiInput> phiInputs;
            phiInputs.reserve(1 + conditionInputs.size());
            phiInputs.emplace_back(currentBlock->id(), preLoopValues.at(loopHeaderPhi.name).value);
            for (const auto& conditionInput : conditionInputs)
            {
                phiInputs.emplace_back(conditionInput.predecessorBlockId, conditionInput.values.at(loopHeaderPhi.name).value);
            }

            loopHeaderPhi.instruction->setInputs(std::move(phiInputs));
        }

        function.addBlock(BasicBlock{ continuationId, "while.continuation", nullptr });
        auto* continuationBlock = function.tryGetBlock(continuationId);
        if (continuationBlock == nullptr)
            return false;

        // condition-false edge reaches the continuation with the bindings visible at the loop header
        continuationInputs.insert(continuationInputs.begin(), IncomingLocalValues{ conditionId, loopHeaderValues });
        mergeLocalValues(*continuationBlock, continuationInputs);
        currentBlockId = continuationId;

        return true;
    }

    bool IRLowerer::lowerBreakStatement(BasicBlock& block, std::optional<BlockId>& currentBlockId) noexcept
    {
        if (m_loopContexts.empty())
            return false;

        auto& loopContext = m_loopContexts.back();
        loopContext.continuationInputs.push_back(IncomingLocalValues{ block.id(), m_localValues });
        block.setTerminator(std::make_unique<JumpTerminator>(loopContext.continuationBlockId));
        currentBlockId.reset();

        return true;
    }

    bool IRLowerer::lowerSkipStatement(BasicBlock& block, std::optional<BlockId>& currentBlockId) noexcept
    {
        if (m_loopContexts.empty())
            return false;

        auto& loopContext = m_loopContexts.back();
        loopContext.conditionInputs.push_back(IncomingLocalValues{ block.id(), m_localValues });
        block.setTerminator(std::make_unique<JumpTerminator>(loopContext.conditionBlockId));
        currentBlockId.reset();

        return true;
    }

    bool IRLowerer::lowerExpressionStatement(const ExpressionStatement* statement, BasicBlock& block) noexcept
    {
        return lowerExpression(statement->expression().get(), block);
    }

    bool IRLowerer::lowerLocalDeclaration(const Expression* leftExpression, const Expression* rightExpression, BasicBlock& block) noexcept
    {
        if (leftExpression->kind() == NodeKind::DiscardLiteral)
        {
            return lowerExpression(rightExpression, block);
        }

        if (leftExpression->kind() != NodeKind::NameExpression)
            return false;

        const auto* nameExpression = static_cast<const NameExpression*>(leftExpression);
        const auto isExplicitReferenceBinding =
            (rightExpression->kind() == NodeKind::UnaryExpression
            && static_cast<const UnaryExpression*>(rightExpression)->unaryOperator() == UnaryOperatorKind::ReferenceOf);

        if (nameExpression->type().isReference() && isExplicitReferenceBinding)
        {
            const auto loweredAddress = lowerAddressExpression(rightExpression, block);
            if (!loweredAddress.has_value())
                return false;

            m_localValues.insert_or_assign(nameExpression->name(), LocalState{ loweredAddress.value(), nameExpression->type(), LocalStorageKind::Address });
            return true;
        }

        const auto loweredValue = lowerValueExpression(rightExpression, block);
        if (!loweredValue.has_value())
            return false;

        auto localType = nameExpression->type();
        if (localType.isReference())
            localType = localType.toValue();

        m_localValues.insert_or_assign(nameExpression->name(), LocalState{ loweredValue.value(), localType, LocalStorageKind::Value });

        return true;
    }

    bool IRLowerer::lowerAssignmentStatement(const Expression* leftExpression, const Expression* rightExpression, BasicBlock& block) noexcept
    {
        if (leftExpression->kind() == NodeKind::DiscardLiteral)
        {
            return lowerExpression(rightExpression, block);
        }

        const auto loweredValue = lowerValueExpression(rightExpression, block);
        if (!loweredValue.has_value())
            return false;

        if (leftExpression->kind() == NodeKind::MemberAccessExpression)
        {
            const auto loweredAddress = lowerAddressExpression(leftExpression, block);
            if (!loweredAddress.has_value())
                return false;

            block.addInstruction(std::make_unique<StoreValueInstruction>(
                loweredValue.value(),
                loweredAddress.value(),
                leftExpression->type().toValue()));
            return true;
        }

        if (leftExpression->kind() != NodeKind::NameExpression)
            return false;

        const auto* nameExpression = static_cast<const NameExpression*>(leftExpression);
        if (!m_localValues.contains(nameExpression->name()))
            return false;

        auto& localState = m_localValues.at(nameExpression->name());
        if (localState.storageKind == LocalStorageKind::Address)
        {
            block.addInstruction(std::make_unique<StoreValueInstruction>(loweredValue.value(), localState.value, localState.type.toValue()));
            return true;
        }

        localState.value = loweredValue.value();

        return true;
    }

    std::optional<ValueRef> IRLowerer::ensureAddressableLocal(const NameExpression* expression, BasicBlock& block) noexcept
    {
        const auto result = m_localValues.find(expression->name());
        if (result == m_localValues.end())
            return std::nullopt;

        auto& localState = result->second;
        if (localState.storageKind == LocalStorageKind::Address)
            return localState.value;

        const auto localId = m_nextLocalSlotId++;
        block.addPrologueInstruction(std::make_unique<AllocateLocalInstruction>(localId, expression->name(), expression->type().toValue()));

        const auto addressId = m_nextTemporaryId++;
        block.addInstruction(std::make_unique<AddressOfInstruction>(addressId, LocalSlotRef{ localId }, expression->type().toReference()));
        block.addInstruction(std::make_unique<StoreValueInstruction>(localState.value, ValueRef{ addressId }, localState.type.toValue()));

        localState.value = ValueRef{ addressId };
        localState.storageKind = LocalStorageKind::Address;
        return localState.value;
    }

    std::optional<ValueRef> IRLowerer::lowerAddressExpression(const Expression* expression, BasicBlock& block) noexcept
    {
        switch (expression->kind())
        {
            case NodeKind::GroupingExpression:
            {
                const auto* groupingExpression = static_cast<const GroupingExpression*>(expression);
                return lowerAddressExpression(groupingExpression->expression().get(), block);
            }
            case NodeKind::NameExpression:
            {
                const auto* nameExpression = static_cast<const NameExpression*>(expression);
                const auto result = m_localValues.find(nameExpression->name());
                if (result == m_localValues.end())
                    return std::nullopt;

                if (nameExpression->type().isReference())
                {
                    if (result->second.storageKind != LocalStorageKind::Address)
                        return std::nullopt;

                    return result->second.value;
                }

                return ensureAddressableLocal(nameExpression, block);
            }
            case NodeKind::UnaryExpression:
            {
                const auto* unaryExpression = static_cast<const UnaryExpression*>(expression);
                if (unaryExpression->unaryOperator() != UnaryOperatorKind::ReferenceOf)
                    return std::nullopt;

                if (unaryExpression->expression()->kind() != NodeKind::NameExpression)
                    return std::nullopt;

                return ensureAddressableLocal(static_cast<const NameExpression*>(unaryExpression->expression().get()), block);
            }
            case NodeKind::MemberAccessExpression:
            {
                const auto* memberAccessExpression = static_cast<const MemberAccessExpression*>(expression);
                if (memberAccessExpression->expression()->kind() != NodeKind::NameExpression)
                    return std::nullopt;

                const auto thisResult = m_localValues.find("this");
                if (thisResult == m_localValues.end() || thisResult->second.storageKind != LocalStorageKind::Address)
                    return std::nullopt;

                const auto* fieldNameExpression = static_cast<const NameExpression*>(memberAccessExpression->expression().get());
                const auto objectType = thisResult->second.type;
                const auto& typeDefinition = m_semanticModule.getTypeDefinition(objectType);
                const auto& fieldDefinition = typeDefinition.tryGetFieldByName(fieldNameExpression->name());
                if (fieldDefinition.type() == Type::Undefined())
                    return std::nullopt;

                const auto temporaryId = m_nextTemporaryId++;
                block.addInstruction(std::make_unique<FieldAddressInstruction>(
                    temporaryId,
                    thisResult->second.value,
                    objectType,
                    fieldNameExpression->name(),
                    fieldDefinition.index(),
                    expression->type().toReference()));
                return ValueRef{ temporaryId };
            }
            default:
                return std::nullopt;
        }
    }

    bool IRLowerer::lowerReturnStatement(const ReturnStatement* statement, BasicBlock& block) noexcept
    {
        if (!statement->expression().has_value())
        {
            block.setTerminator(std::make_unique<ReturnTerminator>());
            return true;
        }

        const auto loweredValue = lowerValueExpression(statement->expression().value().get(), block);
        if (!loweredValue.has_value())
            return false;

        block.setTerminator(std::make_unique<ReturnValueTerminator>(loweredValue.value()));
        return true;
    }

    std::optional<ConstantValue> IRLowerer::tryLowerConstantExpression(const Expression* expression) noexcept
    {
        switch (expression->kind())
        {
            case NodeKind::NumberLiteral:
            {
                return CreateConstantValue(*static_cast<const NumberLiteral*>(expression));
            }
            case NodeKind::BoolLiteral:
            {
                const auto* literal = static_cast<const BoolLiteral*>(expression);
                return ConstantValue::FromBool(literal->value());
            }
            case NodeKind::StringLiteral:
            {
                const auto* literal = static_cast<const StringLiteral*>(expression);
                return ConstantValue::FromString(literal->escapedContent());
            }
            case NodeKind::GroupingExpression:
            {
                const auto* groupingExpression = static_cast<const GroupingExpression*>(expression);
                return tryLowerConstantExpression(groupingExpression->expression().get());
            }
            case NodeKind::UnaryExpression:
            {
                const auto* unaryExpression = static_cast<const UnaryExpression*>(expression);
                const auto operandValue = tryLowerConstantExpression(unaryExpression->expression().get());
                if (!operandValue.has_value())
                    return std::nullopt;

                switch (unaryExpression->unaryOperator())
                {
                    case UnaryOperatorKind::ValueNegation:
                    case UnaryOperatorKind::LogicalNegation:
                        return ConstantFoldUnary(unaryExpression->unaryOperator(), operandValue.value());
                    default:
                        return std::nullopt;
                }
            }
            case NodeKind::BinaryExpression:
            {
                const auto* binaryExpression = static_cast<const BinaryExpression*>(expression);
                if (binaryExpression->binaryOperator() == BinaryOperatorKind::MemberAccess)
                {
                    const auto enumConstant = tryLowerEnumMemberConstant(binaryExpression);
                    if (!enumConstant.has_value())
                        return std::nullopt;

                    if (const auto* enumValue = enumConstant->tryGetEnumConstant())
                        return ConstantValue::FromLiteralData(enumValue->underlyingValue);

                    return std::nullopt;
                }

                const auto leftValue = tryLowerConstantExpression(binaryExpression->leftExpression().get());
                if (!leftValue.has_value())
                    return std::nullopt;

                const auto rightValue = tryLowerConstantExpression(binaryExpression->rightExpression().get());
                if (!rightValue.has_value())
                    return std::nullopt;

                return ConstantFoldBinary(binaryExpression->binaryOperator(), leftValue.value(), rightValue.value());
            }
            default:
                return std::nullopt;
        }
    }

    std::optional<ConstantValue> IRLowerer::tryLowerEnumFieldValue(Type enumType, const std::string& fieldName) noexcept
    {
        auto& enumDefinition = m_semanticModule.getEnumDefinition(enumType);
        if (!enumDefinition.hasField(fieldName))
            return std::nullopt;

        const auto& enumField = enumDefinition.getFieldByName(fieldName);
        if (enumField.expression() != nullptr)
            return tryLowerConstantExpression(enumField.expression());

        return CreateEnumConstantValue(enumDefinition.baseType(), enumField.value());
    }

    std::optional<ConstantValue> IRLowerer::tryLowerEnumMemberConstant(const BinaryExpression* expression) noexcept
    {
        const auto enumType = expression->leftExpression()->type();
        if (enumType.kind() != TypeKind::Enum)
            return std::nullopt;

        if (expression->rightExpression()->kind() != NodeKind::NameExpression)
            return std::nullopt;

        const auto* fieldNameExpression = static_cast<const NameExpression*>(expression->rightExpression().get());
        auto underlyingValue = tryLowerEnumFieldValue(enumType, fieldNameExpression->name());
        if (!underlyingValue.has_value())
            return std::nullopt;

        const auto* literalData = underlyingValue->tryGetLiteralData();
        if (literalData == nullptr)
            return std::nullopt;

        auto& enumDefinition = m_semanticModule.getEnumDefinition(enumType);
        return ConstantValue::FromEnum(
            enumType,
            enumDefinition.name(),
            fieldNameExpression->name(),
            *literalData);
    }

    bool IRLowerer::lowerExpression(const Expression* expression, BasicBlock& block) noexcept
    {
        if (expression->kind() == NodeKind::FunctionCallExpression)
            return lowerFunctionCall(static_cast<const FunctionCallExpression*>(expression), block).has_value();

        return lowerValueExpression(expression, block).has_value();
    }

    std::optional<ValueRef> IRLowerer::lowerValueExpression(const Expression* expression, BasicBlock& block) noexcept
    {
        switch (expression->kind())
        {
            case NodeKind::NumberLiteral:
            {
                const auto* literal = static_cast<const NumberLiteral*>(expression);
                const auto constantValue = CreateConstantValue(*literal);
                if (!constantValue.has_value())
                    return std::nullopt;

                const auto temporaryId = m_nextTemporaryId++;
                block.addInstruction(std::make_unique<ConstantInstruction>(
                    temporaryId,
                    constantValue.value(),
                    literal->type()));
                return ValueRef{ temporaryId };
            }
            case NodeKind::BoolLiteral:
            {
                const auto* literal = static_cast<const BoolLiteral*>(expression);
                const auto temporaryId = m_nextTemporaryId++;
                block.addInstruction(std::make_unique<ConstantInstruction>(
                    temporaryId,
                    ConstantValue::FromBool(literal->value()),
                    literal->type()));
                return ValueRef{ temporaryId };
            }
            case NodeKind::StringLiteral:
            {
                const auto* literal = static_cast<const StringLiteral*>(expression);
                const auto temporaryId = m_nextTemporaryId++;
                block.addInstruction(std::make_unique<ConstantInstruction>(
                    temporaryId,
                    ConstantValue::FromString(literal->escapedContent()),
                    literal->type()));
                return ValueRef{ temporaryId };
            }
            case NodeKind::NameExpression:
            {
                const auto* nameExpression = static_cast<const NameExpression*>(expression);
                const auto result = m_localValues.find(nameExpression->name());
                if (result == m_localValues.end())
                    return std::nullopt;

                if (result->second.storageKind == LocalStorageKind::Address)
                {
                    const auto temporaryId = m_nextTemporaryId++;
                    block.addInstruction(std::make_unique<LoadValueInstruction>(temporaryId, result->second.value, result->second.type.toValue()));
                    return ValueRef{ temporaryId };
                }

                return result->second.value;
            }
            case NodeKind::FunctionCallExpression:
                if (expression->type() == Type::Void())
                    return std::nullopt;

                return lowerFunctionCall(static_cast<const FunctionCallExpression*>(expression), block);
            case NodeKind::MemberAccessExpression:
            {
                const auto loweredAddress = lowerAddressExpression(expression, block);
                if (!loweredAddress.has_value())
                    return std::nullopt;

                if (expression->type().isReference())
                    return loweredAddress;

                const auto temporaryId = m_nextTemporaryId++;
                block.addInstruction(std::make_unique<LoadValueInstruction>(
                    temporaryId,
                    loweredAddress.value(),
                    expression->type().toValue()));
                return ValueRef{ temporaryId };
            }
            case NodeKind::UnaryExpression:
            {
                const auto* unaryExpression = static_cast<const UnaryExpression*>(expression);
                if (unaryExpression->unaryOperator() == UnaryOperatorKind::ReferenceOf)
                    return lowerAddressExpression(expression, block);

                const auto operandValue = lowerValueExpression(unaryExpression->expression().get(), block);
                if (!operandValue.has_value())
                    return std::nullopt;

                switch (unaryExpression->unaryOperator())
                {
                    case UnaryOperatorKind::ValueNegation:
                    {
                        const auto temporaryId = m_nextTemporaryId++;
                        block.addInstruction(std::make_unique<ValueNegationInstruction>(temporaryId, operandValue.value(), expression->type()));
                        return ValueRef{ temporaryId };
                    }
                    case UnaryOperatorKind::LogicalNegation:
                    {
                        const auto temporaryId = m_nextTemporaryId++;
                        block.addInstruction(std::make_unique<LogicalNegationInstruction>(temporaryId, operandValue.value(), expression->type()));
                        return ValueRef{ temporaryId };
                    }
                    default:
                        return std::nullopt;
                }
            }
            case NodeKind::BinaryExpression:
            {
                const auto* binaryExpression = static_cast<const BinaryExpression*>(expression);
                if (binaryExpression->binaryOperator() == BinaryOperatorKind::MemberAccess)
                {
                    if (binaryExpression->rightExpression()->kind() == NodeKind::FunctionCallExpression)
                    {
                        if (binaryExpression->type() == Type::Void())
                            return std::nullopt;

                        const auto* functionCallExpression = static_cast<const FunctionCallExpression*>(binaryExpression->rightExpression().get());
                        return lowerFunctionCall(functionCallExpression, block);
                    }

                    const auto enumConstant = tryLowerEnumMemberConstant(binaryExpression);
                    if (!enumConstant.has_value())
                        return std::nullopt;

                    auto loweredType = expression->type();
                    if (const auto* enumValue = enumConstant->tryGetEnumConstant())
                        loweredType = enumValue->enumType;

                    const auto temporaryId = m_nextTemporaryId++;
                    block.addInstruction(std::make_unique<ConstantInstruction>(
                        temporaryId,
                        enumConstant.value(),
                        loweredType));
                    return ValueRef{ temporaryId };
                }

                const auto leftValue = lowerValueExpression(binaryExpression->leftExpression().get(), block);
                if (!leftValue.has_value())
                    return std::nullopt;

                const auto rightValue = lowerValueExpression(binaryExpression->rightExpression().get(), block);
                if (!rightValue.has_value())
                    return std::nullopt;

                const auto temporaryId = m_nextTemporaryId++;
                switch (binaryExpression->binaryOperator())
                {
                    case BinaryOperatorKind::Addition:
                        block.addInstruction(std::make_unique<AddInstruction>(temporaryId, leftValue.value(), rightValue.value(), expression->type()));
                        break;
                    case BinaryOperatorKind::Subtraction:
                        block.addInstruction(std::make_unique<SubtractInstruction>(temporaryId, leftValue.value(), rightValue.value(), expression->type()));
                        break;
                    case BinaryOperatorKind::Multiplication:
                        block.addInstruction(std::make_unique<MultiplyInstruction>(temporaryId, leftValue.value(), rightValue.value(), expression->type()));
                        break;
                    case BinaryOperatorKind::Division:
                        block.addInstruction(std::make_unique<DivideInstruction>(temporaryId, leftValue.value(), rightValue.value(), expression->type()));
                        break;
                    case BinaryOperatorKind::Equal:
                        block.addInstruction(std::make_unique<EqualInstruction>(temporaryId, leftValue.value(), rightValue.value(), expression->type()));
                        break;
                    case BinaryOperatorKind::NotEqual:
                        block.addInstruction(std::make_unique<NotEqualInstruction>(temporaryId, leftValue.value(), rightValue.value(), expression->type()));
                        break;
                    case BinaryOperatorKind::LessThan:
                        block.addInstruction(std::make_unique<LessThanInstruction>(temporaryId, leftValue.value(), rightValue.value(), expression->type()));
                        break;
                    case BinaryOperatorKind::LessOrEqual:
                        block.addInstruction(std::make_unique<LessOrEqualInstruction>(temporaryId, leftValue.value(), rightValue.value(), expression->type()));
                        break;
                    case BinaryOperatorKind::GreaterThan:
                        block.addInstruction(std::make_unique<GreaterThanInstruction>(temporaryId, leftValue.value(), rightValue.value(), expression->type()));
                        break;
                    case BinaryOperatorKind::GreaterOrEqual:
                        block.addInstruction(std::make_unique<GreaterOrEqualInstruction>(temporaryId, leftValue.value(), rightValue.value(), expression->type()));
                        break;
                    default:
                        return std::nullopt;
                }

                return ValueRef{ temporaryId };
            }
            default:
                return std::nullopt;
        }
    }

    std::optional<ValueRef> IRLowerer::lowerFunctionCall(const FunctionCallExpression* expression, BasicBlock& block) noexcept
    {
        const auto functionType = expression->functionType();
        if (functionType == Type::Undefined())
            return std::nullopt;

        auto& functionDefinition = m_semanticModule.getFunctionDefinition(functionType);
        const auto functionId = functionDefinition.type().id();

        std::vector<ValueRef> loweredArguments;
        const auto& arguments = expression->argumentsNode()->arguments();
        loweredArguments.reserve(arguments.size());
        const auto& parameterTypes = functionDefinition.parameters();
        for (size_t index = 0; index < arguments.size(); ++index)
        {
            const auto& argument = arguments[index];
            std::optional<ValueRef> loweredArgument;
            if (index < parameterTypes.size() && parameterTypes[index].type().isReference())
            {
                loweredArgument = lowerAddressExpression(argument.get(), block);
            }
            else
            {
                loweredArgument = lowerValueExpression(argument.get(), block);
            }

            if (!loweredArgument.has_value())
                return std::nullopt;

            loweredArguments.push_back(loweredArgument.value());
        }

        if (expression->type() == Type::Void())
        {
            block.addInstruction(std::make_unique<CallVoidInstruction>(functionId, std::move(loweredArguments)));
            return ValueRef{};
        }

        const auto temporaryId = m_nextTemporaryId++;
        block.addInstruction(std::make_unique<CallInstruction>(temporaryId, functionId, std::move(loweredArguments), expression->type()));
        return ValueRef{ temporaryId };
    }

    void IRLowerer::resetState()
    {
        m_localValues.clear();
        m_nextTemporaryId = 0;
        m_nextLocalSlotId = 0;
        m_nextBlockId = 0;
    }

    void IRLowerer::restoreLocalValues(const LocalStateMap& values) noexcept
    {
        m_localValues = values;
    }

    void IRLowerer::mergeLocalValues(BasicBlock& block, const std::vector<IncomingLocalValues>& incomingValues) noexcept
    {
        LocalStateMap mergedLocalValues;
        if (incomingValues.empty())
        {
            m_localValues = std::move(mergedLocalValues);
            return;
        }

        const auto& firstIncomingValues = incomingValues.front().values;

        std::vector<std::string> mergeCandidateNames;
        mergeCandidateNames.reserve(firstIncomingValues.size());
        for (const auto& [name, localState] : firstIncomingValues)
        {
            if (localState.type == Type::Undefined())
                continue;

            mergeCandidateNames.push_back(name);
        }

        // sorting for deterministic phi insertion and snapshot output
        std::sort(mergeCandidateNames.begin(), mergeCandidateNames.end());

        for (const auto& name : mergeCandidateNames)
        {
            // only merge locals that are defined along every incoming edge
            const auto isAvailableOnAllPaths = std::all_of(
                incomingValues.begin(),
                incomingValues.end(),
                [&name](const IncomingLocalValues& incomingValue)
                {
                    return incomingValue.values.contains(name);
                });
            if (!isAvailableOnAllPaths)
                continue;

            const auto& firstState = firstIncomingValues.at(name);
            // phi is only needed when at least one predecessor contributes a different SSA value
            const auto requiresPhi = std::any_of(
                incomingValues.begin() + 1,
                incomingValues.end(),
                [&name, &firstState](const IncomingLocalValues& incomingValue)
                {
                    return incomingValue.values.at(name).value.id() != firstState.value.id();
                });
            if (!requiresPhi)
            {
                mergedLocalValues.emplace(name, firstState);
                continue;
            }

            std::vector<PhiInput> phiInputs;
            phiInputs.reserve(incomingValues.size());
            // phi inputs in predecessor order for the merged block
            for (const auto& incomingValue : incomingValues)
            {
                phiInputs.emplace_back(incomingValue.predecessorBlockId, incomingValue.values.at(name).value);
            }

            const auto phiId = m_nextTemporaryId++;
            block.addInstruction(std::make_unique<PhiInstruction>(phiId, std::move(phiInputs), firstState.type));
            mergedLocalValues.emplace(name, LocalState{ ValueRef{ phiId }, firstState.type });
        }

        // replace locals with values after the merge
        m_localValues = std::move(mergedLocalValues);
    }
}
