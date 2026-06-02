#pragma once

#include <Caracal/API.h>
#include <Caracal/IR/BasicBlock.h>
#include <Caracal/IR/ConstantValue.h>
#include <Caracal/IR/Function.h>
#include <Caracal/IR/Module.h>
#include <Caracal/IR/ValueRef.h>
#include <Caracal/IR/PhiInstruction.h>
#include <Caracal/Semantic/SemanticContext.h>
#include <Caracal/Syntax/BinaryExpression.h>
#include <Caracal/Syntax/BlockNode.h>
#include <Caracal/Syntax/EnumDefinitionStatement.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/FunctionDefinitionStatement.h>
#include <Caracal/Syntax/GroupingExpression.h>
#include <Caracal/Syntax/IfStatement.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Syntax/ReturnStatement.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/WhileStatement.h>
#include <Caracal/Syntax/ExpressionStatement.h>
#include <Caracal/Syntax/FunctionCallExpression.h>

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Caracal
{
    class CARACAL_API IRLowerer
    {
    private:
        enum class LocalStorageKind
        {
            Value,
            Address,
        };

        struct LocalState final
        {
            ValueRef value;
            Type type;
            LocalStorageKind storageKind{ LocalStorageKind::Value };
        };
        using LocalStateMap = std::unordered_map<std::string, LocalState>;

        struct IncomingLocalValues final
        {
            BlockId predecessorBlockId;
            LocalStateMap values;
        };

        struct LoopContext final
        {
            BlockId conditionBlockId;
            BlockId continuationBlockId;
            std::vector<IncomingLocalValues> conditionInputs;
            std::vector<IncomingLocalValues> continuationInputs;
        };

        struct LoopHeaderPhi final
        {
            std::string name;
            PhiInstruction* instruction;
            Type type;
        };

    public:
        explicit IRLowerer(SemanticContext& semanticModule);

        CARACAL_DELETE_COPY_DELETE_MOVE(IRLowerer)

        [[nodiscard]] bool lower(const ParseTree& parseTree, Module& module) noexcept;

    private:
        [[nodiscard]] bool lowerStatement(const Statement* statement, Module& module) noexcept;
        [[nodiscard]] bool lowerStatement(const Statement* statement, Function& function, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerEnumDefinition(const EnumDefinitionStatement* statement, Module& module) noexcept;
        [[nodiscard]] bool lowerFunctionDefinition(const FunctionDefinitionStatement* statement, Module& module) noexcept;
        [[nodiscard]] bool lowerBlock(const BlockNode* block, Function& function, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerIfStatement(const IfStatement* statement, Function& function, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerWhileStatement(const WhileStatement* statement, Function& function, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerBreakStatement(BasicBlock& block, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerSkipStatement(BasicBlock& block, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerExpressionStatement(const ExpressionStatement* statement, BasicBlock& block) noexcept;
        void lowerParameters(const FunctionDefinitionStatement* statement, BasicBlock& block) noexcept;
        [[nodiscard]] bool lowerLocalDeclaration(const Expression* leftExpression, const Expression* rightExpression, BasicBlock& block) noexcept;
        [[nodiscard]] bool lowerAssignmentStatement(const Expression* leftExpression, const Expression* rightExpression, BasicBlock& block) noexcept;
        [[nodiscard]] bool lowerReturnStatement(const ReturnStatement* statement, BasicBlock& block) noexcept;
        [[nodiscard]] std::optional<ConstantValue> tryLowerConstantExpression(const Expression* expression) noexcept;
        [[nodiscard]] std::optional<ConstantValue> tryLowerEnumFieldValue(Type enumType, const std::string& fieldName) noexcept;
        [[nodiscard]] std::optional<ConstantValue> tryLowerEnumMemberConstant(const BinaryExpression* expression) noexcept;
        [[nodiscard]] std::optional<ValueRef> ensureAddressableLocal(const NameExpression* expression, BasicBlock& block) noexcept;
        [[nodiscard]] std::optional<ValueRef> lowerAddressExpression(const Expression* expression, BasicBlock& block) noexcept;
        [[nodiscard]] std::optional<ValueRef> lowerValueExpression(const Expression* expression, BasicBlock& block) noexcept;
        [[nodiscard]] bool lowerExpression(const Expression* expression, BasicBlock& block) noexcept;
        [[nodiscard]] std::optional<ValueRef> lowerFunctionCall(const FunctionCallExpression* expression, BasicBlock& block) noexcept;
        
        void restoreLocalValues(const LocalStateMap& values) noexcept;
        void mergeLocalValues(BasicBlock& block, const std::vector<IncomingLocalValues>& incomingValues) noexcept;

        SemanticContext& m_semanticModule;
        LocalStateMap m_localValues;
        std::vector<LoopContext> m_loopContexts;
        TemporaryId m_nextTemporaryId{ 0 };
        LocalSlotId m_nextLocalSlotId{ 0 };
        BlockId m_nextBlockId{ 0 };
    };
}
