#include <Caracal/IR/IRLowerer.h>

#include <Caracal/IR/ConstantValue.h>
#include <Caracal/IR/ConstantInstruction.h>
#include <Caracal/IR/ReturnTerminator.h>
#include <Caracal/IR/ReturnValueTerminator.h>
#include <Caracal/Semantic/FunctionDefinition.h>
#include <Caracal/Syntax/BlockNode.h>
#include <Caracal/Syntax/BoolLiteral.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/FunctionDefinitionStatement.h>
#include <Caracal/Syntax/NodeKind.h>
#include <Caracal/Syntax/NumberLiteral.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Syntax/ReturnStatement.h>
#include <Caracal/Syntax/Statement.h>

#include <optional>
#include <variant>

namespace Caracal
{
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
        auto entryBlock = BasicBlock{ m_nextBlockId++, "entry", std::make_unique<ReturnTerminator>() };
        function.addBlock(std::move(entryBlock));

        if (!lowerBlock(statement->bodyNode().get(), function))
            return false;

        module.addFunction(std::move(function));
        return true;
    }

    bool IRLowerer::lowerBlock(const BlockNode* block, Function& function) noexcept
    {
        if (!function.hasBlocks())
            return false;

        auto& entryBlock = function.firstBlock();

        for (const auto& statement : block->statements())
        {
            if (statement->kind() != NodeKind::ReturnStatement)
                return false;

            if (!lowerReturnStatement(static_cast<const ReturnStatement*>(statement.get()), entryBlock))
                return false;
        }

        return true;
    }

    bool IRLowerer::lowerReturnStatement(const ReturnStatement* statement, BasicBlock& block) noexcept
    {
        if (!statement->expression().has_value())
            return true;

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
            default:
                return std::nullopt;
        }
    }
}
