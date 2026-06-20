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
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/GroupingExpression.h>
#include <Caracal/Syntax/IfStatement.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Syntax/ReturnStatement.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/WhileStatement.h>
#include <Caracal/Syntax/ExpressionStatement.h>
#include <Caracal/Syntax/FunctionCallExpression.h>
#include <Caracal/Syntax/TypeDefinitionStatement.h>

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
        };

    public:
        explicit IRLowerer(SemanticContext& semanticContext);

        CARACAL_DELETE_COPY_DELETE_MOVE(IRLowerer)

        [[nodiscard]] bool lower(Module& module) noexcept;

    private:
        [[nodiscard]] bool lowerStatement(const Statement* statement, Function& function, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerEnumDefinition(const EnumDefinition& definition, Module& module) noexcept;
        [[nodiscard]] bool lowerTypeDefinition(const TypeDefinition& definition, Module& module) noexcept;
        [[nodiscard]] bool lowerFunctionDefinition(const FunctionDefinition& definition, const BlockNode* bodyNode, bool isExtern, Module& module) noexcept;
        [[nodiscard]] bool lowerSynthesizedConstructorDefinition(const FunctionDefinition& definition, Module& module) noexcept;
        [[nodiscard]] bool ensureExitTerminator(Function& function, std::optional<BlockId> currentBlockId, Type returnType) noexcept;
        void collectAddressTakenLocals(const Statement* statement) noexcept;
        void collectAddressTakenLocals(const Expression* expression) noexcept;
        void collectAddressTakenLocals(const std::vector<std::unique_ptr<Statement>>& statements) noexcept;
        void collectAddressTakenLocals(const std::vector<std::unique_ptr<Expression>>& expressions) noexcept;
        [[nodiscard]] bool lowerBlock(const BlockNode* block, Function& function, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerIfStatement(const IfStatement* statement, Function& function, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerWhileStatement(const WhileStatement* statement, Function& function, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerBreakStatement(BasicBlock& block, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerSkipStatement(BasicBlock& block, std::optional<BlockId>& currentBlockId) noexcept;
        [[nodiscard]] bool lowerExpressionStatement(const ExpressionStatement* statement, BasicBlock& block) noexcept;
        [[nodiscard]] bool lowerParameters(const FunctionDefinition& definition, BasicBlock& block) noexcept;
        [[nodiscard]] bool lowerLocalDeclaration(const Expression* leftExpression, const Expression* rightExpression, BasicBlock& block) noexcept;
        [[nodiscard]] bool lowerAssignmentStatement(const Expression* leftExpression, const Expression* rightExpression, BasicBlock& block) noexcept;
        [[nodiscard]] bool lowerReturnStatement(const ReturnStatement* statement, BasicBlock& block) noexcept;
        [[nodiscard]] std::optional<ConstantValue> tryLowerConstantExpression(const Expression* expression) noexcept;
        [[nodiscard]] std::optional<ConstantValue> tryLowerEnumFieldValue(Type enumType, const std::string& fieldName) noexcept;
        [[nodiscard]] std::optional<ConstantValue> tryLowerEnumMemberConstant(const BinaryExpression* expression) noexcept;
        [[nodiscard]] std::optional<ValueRef> allocateLocalSlot(std::string localName, Type valueType, BasicBlock& block, std::optional<ValueRef> initialValue = std::nullopt) noexcept;
        [[nodiscard]] ValueRef emitFieldAddress(ValueRef objectAddress, Type objectType, const std::string& fieldName, i32 fieldIndex, Type fieldType, BasicBlock& block) noexcept;
        [[nodiscard]] ValueRef emitLoad(ValueRef address, Type valueType, BasicBlock& block) noexcept;
        void setLocalValue(std::string localName, ValueRef value, Type type) noexcept;
        void setAddressBackedLocal(std::string localName, ValueRef address, Type valueType) noexcept;
        [[nodiscard]] std::vector<std::string> sortedDefinedLocalNames(const LocalStateMap& localValues) const noexcept;
        [[nodiscard]] std::optional<ValueRef> tryGetAddressBackedLocal(const std::string& localName) const noexcept;
        [[nodiscard]] bool isLocalDefinedOnAllEdges(const std::vector<IncomingLocalValues>& incomingValues, const std::string& name) const noexcept;
        [[nodiscard]] bool localNeedsPhi(const std::vector<IncomingLocalValues>& incomingValues, const std::string& name, const LocalState& firstState) const noexcept;
        [[nodiscard]] bool tryLowerConstructorCallIntoAddress(const Expression* expression, ValueRef destinationAddress, BasicBlock& block) noexcept;
        [[nodiscard]] std::optional<ValueRef> allocateSlotFromExpression(std::string localName, const Expression* expression, Type valueType, BasicBlock& block) noexcept;
        [[nodiscard]] std::optional<ValueRef> spillValueToTempSlot(const Expression* expression, ValueRef value, BasicBlock& block) noexcept;
        [[nodiscard]] std::optional<ValueRef> lowerMethodReceiverAddress(const Expression* receiverExpression, BasicBlock& block) noexcept;
        [[nodiscard]] std::optional<ValueRef> lowerCallWithReceiver(const FunctionCallExpression* expression, BasicBlock& block, const Expression* receiverExpression = nullptr) noexcept;
        [[nodiscard]] std::optional<ValueRef> lowerMemberFieldAddress(const Expression* receiverExpression, const NameExpression* fieldNameExpression, BasicBlock& block) noexcept;
        [[nodiscard]] std::optional<ValueRef> lowerAddressExpression(const Expression* expression, BasicBlock& block) noexcept;
        [[nodiscard]] std::optional<ValueRef> lowerValueExpression(const Expression* expression, BasicBlock& block) noexcept;
        [[nodiscard]] bool lowerExpressionForEffect(const Expression* expression, BasicBlock& block) noexcept;
        [[nodiscard]] std::optional<ValueRef> emitCall(const FunctionCallExpression* expression, BasicBlock& block, std::optional<ValueRef> implicitArgument = std::nullopt) noexcept;
        
        void resetState();
        void restoreLocalValues(const LocalStateMap& values) noexcept;
        void mergeLocalValues(BasicBlock& block, const std::vector<IncomingLocalValues>& incomingValues) noexcept;

        SemanticContext& m_semanticContext;
        LocalStateMap m_locals;
        std::unordered_set<std::string> m_addressTakenLocals;
        std::vector<LoopContext> m_loopContexts;
        TemporaryId m_nextTemporaryId{ 0 };
        LocalSlotId m_nextLocalSlotId{ 0 };
        BlockId m_nextBlockId{ 0 };
    };
}
