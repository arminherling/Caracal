#include <Caracal/IR/IRLowerer.h>

#include <Caracal/IR/AddInstruction.h>
#include <Caracal/IR/ConstantValue.h>
#include <Caracal/IR/ConstantInstruction.h>
#include <Caracal/IR/DivideInstruction.h>
#include <Caracal/IR/EqualInstruction.h>
#include <Caracal/IR/GreaterOrEqualInstruction.h>
#include <Caracal/IR/GreaterThanInstruction.h>
#include <Caracal/IR/BranchIfTerminator.h>
#include <Caracal/IR/JumpTerminator.h>
#include <Caracal/IR/LessOrEqualInstruction.h>
#include <Caracal/IR/LessThanInstruction.h>
#include <Caracal/IR/LogicalNegationInstruction.h>
#include <Caracal/IR/MultiplyInstruction.h>
#include <Caracal/IR/NotEqualInstruction.h>
#include <Caracal/IR/ParameterInstruction.h>
#include <Caracal/IR/PhiInstruction.h>
#include <Caracal/IR/ReturnTerminator.h>
#include <Caracal/IR/ReturnValueTerminator.h>
#include <Caracal/IR/SubtractInstruction.h>
#include <Caracal/IR/ValueNegationInstruction.h>
#include <Caracal/Syntax/AssignmentStatement.h>
#include <Caracal/Semantic/FunctionDefinition.h>
#include <Caracal/Syntax/BinaryExpression.h>
#include <Caracal/Syntax/BlockNode.h>
#include <Caracal/Syntax/BoolLiteral.h>
#include <Caracal/Syntax/ConstantDeclaration.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/FunctionDefinitionStatement.h>
#include <Caracal/Syntax/IfStatement.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/NodeKind.h>
#include <Caracal/Syntax/NumberLiteral.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Syntax/ReturnStatement.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/UnaryExpression.h>
#include <Caracal/Syntax/VariableDeclaration.h>
#include <Caracal/Syntax/WhileStatement.h>

#include <algorithm>
#include <optional>
#include <variant>

namespace Caracal
{
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

    IRLowerer::IRLowerer(SemanticContext& semanticModule)
        : m_semanticModule{ semanticModule }
    {
    }

    bool IRLowerer::lower(const ParseTree& parseTree, Module& module) noexcept
    {
        m_nextTemporaryId = 0;
        m_nextBlockId = 0;
        m_loopContexts.clear();

        for (const auto& statement : parseTree.statements())
        {
            if (!lowerStatement(statement.get(), module))
                return false;
        }

        return true;
    }

    bool IRLowerer::lowerStatement(const Statement* statement, Module& module) noexcept
    {
        if (statement->kind() == NodeKind::FunctionDefinitionStatement)
            return lowerFunctionDefinition(static_cast<const FunctionDefinitionStatement*>(statement), module);

        return false;
    }

    bool IRLowerer::lowerFunctionDefinition(const FunctionDefinitionStatement* statement, Module& module) noexcept
    {
        m_localValues.clear();

        const auto functionType = m_semanticModule.tryGetFunctionTypeByName(statement->name());
        if (functionType == Type::Undefined())
            return false;

        const auto& functionDefinition = m_semanticModule.getFunctionDefinition(functionType);
        auto returnType = Type::Void();
        if (!functionDefinition.returnTypes().empty())
        {
            returnType = functionDefinition.returnTypes().front();
        }

        std::vector<Type> parameterTypes;
        for (const auto& parameter : functionDefinition.parameters())
        {
            parameterTypes.push_back(parameter.type());
        }

        if (statement->isExtern())
        {
            auto function = ExternFunction{ statement->name(), parameterTypes, returnType };
            module.addExternFunction(std::move(function));
            return true;
        }

        auto function = Function{ statement->name(), parameterTypes, returnType };
        auto blockId = m_nextBlockId++;
        auto entryBlock = BasicBlock{ blockId, "entry", std::make_unique<ReturnTerminator>() };
        lowerParameters(statement, entryBlock);
        function.addBlock(std::move(entryBlock));

        std::optional<BlockId> entryBlockId = blockId;
        if (!lowerBlock(statement->bodyNode().get(), function, entryBlockId))
            return false;

        module.addFunction(std::move(function));
        return true;
    }

    void IRLowerer::lowerParameters(const FunctionDefinitionStatement* statement, BasicBlock& block) noexcept
    {
        const auto& parameterNodes = statement->parametersNode()->parameters();
        for (size_t index = 0; index < parameterNodes.size(); ++index)
        {
            const auto parameterId = m_nextTemporaryId++;
            const auto parameterName = parameterNodes[index]->name();
            const auto parameterType = parameterNodes[index]->type();
            block.addInstruction(std::make_unique<ParameterInstruction>(
                parameterId,
                static_cast<i32>(index),
                parameterType));
            m_localValues.emplace(parameterName, LocalState{ ValueRef{ parameterId }, parameterType });
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

    bool IRLowerer::lowerLocalDeclaration(const Expression* leftExpression, const Expression* rightExpression, BasicBlock& block) noexcept
    {
        if (leftExpression->kind() == NodeKind::DiscardLiteral)
        {
            return lowerValueExpression(rightExpression, block).has_value();
        }

        if (leftExpression->kind() != NodeKind::NameExpression)
            return false;

        const auto loweredValue = lowerValueExpression(rightExpression, block);
        if (!loweredValue.has_value())
            return false;

        const auto* nameExpression = static_cast<const NameExpression*>(leftExpression);
        m_localValues.insert_or_assign(nameExpression->name(), LocalState{ loweredValue.value(), nameExpression->type() });

        return true;
    }

    bool IRLowerer::lowerAssignmentStatement(const Expression* leftExpression, const Expression* rightExpression, BasicBlock& block) noexcept
    {
        if (leftExpression->kind() == NodeKind::DiscardLiteral)
        {
            return lowerValueExpression(rightExpression, block).has_value();
        }

        if (leftExpression->kind() != NodeKind::NameExpression)
            return false;

        const auto* nameExpression = static_cast<const NameExpression*>(leftExpression);
        if (!m_localValues.contains(nameExpression->name()))
            return false;

        const auto loweredValue = lowerValueExpression(rightExpression, block);
        if (!loweredValue.has_value())
            return false;

        auto& localState = m_localValues.at(nameExpression->name());
        localState.value = loweredValue.value();

        return true;
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
            case NodeKind::NameExpression:
            {
                const auto* nameExpression = static_cast<const NameExpression*>(expression);
                const auto result = m_localValues.find(nameExpression->name());
                if (result == m_localValues.end())
                    return std::nullopt;

                return result->second.value;
            }
            case NodeKind::UnaryExpression:
            {
                const auto* unaryExpression = static_cast<const UnaryExpression*>(expression);
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
}
