#pragma once

#include <Caracal/API.h>
#include <Caracal/IR/BasicBlock.h>
#include <Caracal/IR/Function.h>
#include <Caracal/IR/Module.h>
#include <Caracal/IR/ValueRef.h>
#include <Caracal/Semantic/SemanticContext.h>
#include <Caracal/Syntax/BlockNode.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/FunctionDefinitionStatement.h>
#include <Caracal/Syntax/IfStatement.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Syntax/ReturnStatement.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/WhileStatement.h>

#include <optional>
#include <string>
#include <unordered_map>

namespace Caracal
{
    class CARACAL_API IRLowerer
    {
    public:
        explicit IRLowerer(SemanticContext& semanticModule);

        CARACAL_DELETE_COPY_DELETE_MOVE(IRLowerer)

        [[nodiscard]] bool lower(const ParseTree& parseTree, Module& module) noexcept;

    private:
        [[nodiscard]] bool lowerStatement(const Statement* statement, Module& module) noexcept;
        [[nodiscard]] bool lowerStatement(const Statement* statement, Function& function, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerFunctionDefinition(const FunctionDefinitionStatement* statement, Module& module) noexcept;
        [[nodiscard]] bool lowerBlock(const BlockNode* block, Function& function, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerIfStatement(const IfStatement* statement, Function& function, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerWhileStatement(const WhileStatement* statement, Function& function, std::optional<BlockId>& currentBlockId) noexcept;
        void lowerParameters(const FunctionDefinitionStatement* statement, BasicBlock& block) noexcept;
        [[nodiscard]] bool lowerLocalDeclaration(const Expression* leftExpression, const Expression* rightExpression, BasicBlock& block) noexcept;
        [[nodiscard]] bool lowerAssignmentStatement(const Expression* leftExpression, const Expression* rightExpression, BasicBlock& block) noexcept;
        [[nodiscard]] bool lowerReturnStatement(const ReturnStatement* statement, BasicBlock& block) noexcept;
        [[nodiscard]] std::optional<ValueRef> lowerValueExpression(const Expression* expression, BasicBlock& block) noexcept;

    private:
        SemanticContext& m_semanticModule;
        std::unordered_map<std::string, ValueRef> m_localValues;
        TemporaryId m_nextTemporaryId{ 0 };
        BlockId m_nextBlockId{ 0 };
    };
}
