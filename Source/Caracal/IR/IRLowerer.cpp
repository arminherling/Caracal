#include <Caracal/IR/IRLowerer.h>

#include <Caracal/IR/AddInstruction.h>
#include <Caracal/IR/ConstantValue.h>
#include <Caracal/IR/ConstantInstruction.h>
#include <Caracal/IR/DivideInstruction.h>
#include <Caracal/IR/BranchIfTerminator.h>
#include <Caracal/IR/JumpTerminator.h>
#include <Caracal/IR/MultiplyInstruction.h>
#include <Caracal/IR/ParameterInstruction.h>
#include <Caracal/IR/ReturnTerminator.h>
#include <Caracal/IR/ReturnValueTerminator.h>
#include <Caracal/IR/SubtractInstruction.h>
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
#include <Caracal/Syntax/VariableDeclaration.h>

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
            block.addInstruction(std::make_unique<ParameterInstruction>(
                parameterId,
                static_cast<i32>(index),
                parameterNodes[index]->type()));
            m_localValues.emplace(parameterNodes[index]->name(), ValueRef{ parameterId });
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

        const auto conditionValue = lowerValueExpression(statement->condition().get(), *currentBlock);
        if (!conditionValue.has_value())
            return false;

        const auto trueId = m_nextBlockId++;
        function.addBlock(BasicBlock{ trueId, "if.true", nullptr });

        if (!statement->hasFalseBlock())
        {
            const auto continuationId = m_nextBlockId++;
            currentBlock->setTerminator(std::make_unique<BranchIfTerminator>(conditionValue.value(), trueId, continuationId));

            std::optional<BlockId> trueBlockId = trueId;
            if (!lowerStatement(statement->trueStatement().get(), function, trueBlockId))
                return false;

            function.addBlock(BasicBlock{ continuationId, "if.continuation", nullptr });

            auto* trueBlock = TryGetCurrentBlock(function, trueBlockId);
            const auto needsContinuationJump = trueBlock != nullptr && !trueBlock->hasTerminator();
            if (needsContinuationJump)
            {
                trueBlock->setTerminator(std::make_unique<JumpTerminator>(continuationId));
            }

            currentBlockId = continuationId;
            return true;
        }

        const auto falseId = m_nextBlockId++;
        currentBlock->setTerminator(std::make_unique<BranchIfTerminator>(conditionValue.value(), trueId, falseId));

        std::optional<BlockId> trueBlockId = trueId;
        if (!lowerStatement(statement->trueStatement().get(), function, trueBlockId))
            return false;

        function.addBlock(BasicBlock{ falseId, "if.false", nullptr });

        std::optional<BlockId> falseBlockId = falseId;
        if (!lowerStatement(statement->falseStatement().value().get(), function, falseBlockId))
            return false;

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

        if (trueFallsThrough)
        {
            trueBlock->setTerminator(std::make_unique<JumpTerminator>(continuationId));
        }

        if (falseFallsThrough)
        {
            falseBlock->setTerminator(std::make_unique<JumpTerminator>(continuationId));
        }

        currentBlockId = continuationId;
        return true;
    }

    bool IRLowerer::lowerLocalDeclaration(const Expression* leftExpression, const Expression* rightExpression, BasicBlock& block) noexcept
    {
        if (leftExpression->kind() != NodeKind::NameExpression)
            return false;

        const auto loweredValue = lowerValueExpression(rightExpression, block);
        if (!loweredValue.has_value())
            return false;

        const auto* nameExpression = static_cast<const NameExpression*>(leftExpression);
        m_localValues[nameExpression->name()] = loweredValue.value();

        return true;
    }

    bool IRLowerer::lowerAssignmentStatement(const Expression* leftExpression, const Expression* rightExpression, BasicBlock& block) noexcept
    {
        if (leftExpression->kind() != NodeKind::NameExpression)
            return false;

        const auto* nameExpression = static_cast<const NameExpression*>(leftExpression);
        if (!m_localValues.contains(nameExpression->name()))
            return false;

        const auto loweredValue = lowerValueExpression(rightExpression, block);
        if (!loweredValue.has_value())
            return false;

        m_localValues[nameExpression->name()] = loweredValue.value();
        return true;
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

                return result->second;
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
