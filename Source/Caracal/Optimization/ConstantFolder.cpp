#include <Caracal/Optimization/ConstantFolder.h>

#include <Caracal/Syntax/AssignmentStatement.h>
#include <Caracal/Syntax/BinaryExpression.h>
#include <Caracal/Syntax/BlockNode.h>
#include <Caracal/Syntax/BoolLiteral.h>
#include <Caracal/Syntax/ConstantDeclaration.h>
#include <Caracal/Syntax/EnumDefinitionStatement.h>
#include <Caracal/Syntax/ExpressionStatement.h>
#include <Caracal/Syntax/FunctionCallExpression.h>
#include <Caracal/Syntax/FunctionDefinitionStatement.h>
#include <Caracal/Syntax/GroupingExpression.h>
#include <Caracal/Syntax/IfStatement.h>
#include <Caracal/Syntax/MemberAccessExpression.h>
#include <Caracal/Syntax/MethodDefinitionStatement.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/NumberLiteral.h>
#include <Caracal/Syntax/ParametersNode.h>
#include <Caracal/Syntax/ReturnStatement.h>
#include <Caracal/Syntax/TypeDefinitionStatement.h>
#include <Caracal/Syntax/TypeFieldDeclaration.h>
#include <Caracal/Syntax/UnaryExpression.h>
#include <Caracal/Syntax/VariableDeclaration.h>
#include <Caracal/Syntax/WhileStatement.h>

#include <string>
#include <unordered_map>

namespace Caracal
{
    namespace
    {
        static std::string FormatTypeName(const SemanticContext& module, Type type)
        {
            return std::string(module.getNameByType(type));
        }

        class ConstantFolder
        {
        public:
            ConstantFolder(const SemanticContext& module, DiagnosticsBag& diagnostics)
                : m_module{ module }
                , m_diagnostics{ diagnostics }
            {
                m_constantScopes.emplace_back();
            }

            void foldTree(const ParseTree& parseTree)
            {
                for (const auto& statement : parseTree.statements())
                {
                    foldStatement(statement.get(), parseTree.tokens());
                }
            }

        private:
            void foldStatement(Statement* statement, const TokenBuffer& tokens)
            {
                switch (statement->kind())
                {
                    case NodeKind::ConstantDeclaration:
                    {
                        auto* declaration = static_cast<ConstantDeclaration*>(statement);
                        foldExpression(declaration->rightExpression().get(), tokens);

                        const auto* foldedInitializer = tryGetFoldedOperand(declaration->rightExpression().get());
                        if (!declaration->isInit()
                            && declaration->leftExpression()->kind() == NodeKind::NameExpression
                            && foldedInitializer != nullptr)
                        {
                            const auto& name = static_cast<const NameExpression*>(declaration->leftExpression().get())->name();
                            m_constantScopes.back().try_emplace(name, *foldedInitializer);
                        }
                        break;
                    }
                    case NodeKind::VariableDeclaration:
                    {
                        auto* declaration = static_cast<VariableDeclaration*>(statement);
                        foldExpression(declaration->rightExpression().get(), tokens);
                        break;
                    }
                    case NodeKind::ExpressionStatement:
                    {
                        auto* expressionStatement = static_cast<ExpressionStatement*>(statement);
                        foldExpression(expressionStatement->expression().get(), tokens);
                        break;
                    }
                    case NodeKind::AssignmentStatement:
                    {
                        auto* assignment = static_cast<AssignmentStatement*>(statement);
                        foldExpression(assignment->leftExpression().get(), tokens);
                        foldExpression(assignment->rightExpression().get(), tokens);
                        break;
                    }
                    case NodeKind::ReturnStatement:
                    {
                        auto* returnStatement = static_cast<ReturnStatement*>(statement);
                        if (returnStatement->expression().has_value())
                        {
                            foldExpression(returnStatement->expression().value().get(), tokens);
                        }
                        break;
                    }
                    case NodeKind::IfStatement:
                    {
                        auto* ifStatement = static_cast<IfStatement*>(statement);
                        foldExpression(ifStatement->condition().get(), tokens);
                        foldStatement(ifStatement->trueStatement().get(), tokens);
                        if (ifStatement->hasFalseBlock())
                        {
                            foldStatement(ifStatement->falseStatement().value().get(), tokens);
                        }
                        break;
                    }
                    case NodeKind::WhileStatement:
                    {
                        auto* whileStatement = static_cast<WhileStatement*>(statement);
                        foldExpression(whileStatement->condition().get(), tokens);
                        foldStatement(whileStatement->trueStatement().get(), tokens);
                        break;
                    }
                    case NodeKind::BlockNode:
                    {
                        auto* block = static_cast<BlockNode*>(statement);
                        m_constantScopes.emplace_back();
                        for (const auto& blockStatement : block->statements())
                        {
                            foldStatement(blockStatement.get(), tokens);
                        }
                        m_constantScopes.pop_back();
                        break;
                    }
                    case NodeKind::FunctionDefinitionStatement:
                    {
                        auto* function = static_cast<FunctionDefinitionStatement*>(statement);
                        foldParameters(function->parametersNode().get(), tokens);
                        foldStatement(function->bodyNode().get(), tokens);
                        break;
                    }
                    case NodeKind::MethodDefinitionStatement:
                    {
                        auto* method = static_cast<MethodDefinitionStatement*>(statement);
                        foldParameters(method->parametersNode().get(), tokens);
                        foldStatement(method->bodyNode().get(), tokens);
                        break;
                    }
                    case NodeKind::TypeDefinitionStatement:
                    {
                        auto* typeDefinition = static_cast<TypeDefinitionStatement*>(statement);
                        if (typeDefinition->isBuiltin())
                        {
                            break;
                        }

                        if (typeDefinition->constructorParameters().has_value())
                        {
                            foldParameters(typeDefinition->constructorParameters().value().get(), tokens);
                        }
                        foldStatement(typeDefinition->bodyNode().get(), tokens);
                        break;
                    }
                    case NodeKind::TypeFieldDeclaration:
                    {
                        auto* field = static_cast<TypeFieldDeclaration*>(statement);
                        foldExpression(field->rightExpression().get(), tokens);
                        break;
                    }
                    case NodeKind::EnumDefinitionStatement:
                    {
                        auto* enumDefinition = static_cast<EnumDefinitionStatement*>(statement);
                        for (const auto& field : enumDefinition->fieldNodes())
                        {
                            if (field->valueExpression().has_value())
                            {
                                foldExpression(field->valueExpression().value().get(), tokens);
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void foldParameters(ParametersNode* parametersNode, const TokenBuffer& tokens)
            {
                for (const auto& parameter : parametersNode->parameters())
                {
                    if (parameter->hasDefault())
                    {
                        foldExpression(parameter->defaultValue().get(), tokens);
                    }
                }
            }

            void foldExpression(Expression* expression, const TokenBuffer& tokens)
            {
                if (expression == nullptr)
                {
                    return;
                }

                switch (expression->kind())
                {
                    case NodeKind::NumberLiteral:
                    {
                        auto* literal = static_cast<NumberLiteral*>(expression);
                        if (!literal->hasParsedValue())
                        {
                            break;
                        }

                        const auto baseType = literal->type().toBaseType();
                        const auto& parsedValue = literal->parsedValue().value();
                        if (baseType == Type::I32())
                        {
                            literal->setFoldedValue(FoldValue{ std::get<i32>(parsedValue) });
                        }
                        else if (baseType == Type::U8())
                        {
                            literal->setFoldedValue(FoldValue{ std::get<u8>(parsedValue) });
                        }
                        else if (baseType == Type::F32())
                        {
                            literal->setFoldedValue(FoldValue{ std::get<f32>(parsedValue) });
                        }
                        break;
                    }
                    case NodeKind::BoolLiteral:
                    {
                        auto* literal = static_cast<BoolLiteral*>(expression);
                        literal->setFoldedValue(FoldValue{ literal->value() });
                        break;
                    }
                    case NodeKind::GroupingExpression:
                    {
                        auto* grouping = static_cast<GroupingExpression*>(expression);
                        foldExpression(grouping->expression().get(), tokens);
                        if (grouping->expression()->foldedValue().has_value())
                        {
                            grouping->setFoldedValue(grouping->expression()->foldedValue().value());
                        }
                        break;
                    }
                    case NodeKind::UnaryExpression:
                    {
                        foldUnaryExpression(static_cast<UnaryExpression*>(expression), tokens);
                        break;
                    }
                    case NodeKind::BinaryExpression:
                    {
                        foldBinaryExpression(static_cast<BinaryExpression*>(expression), tokens);
                        break;
                    }
                    case NodeKind::FunctionCallExpression:
                    {
                        auto* call = static_cast<FunctionCallExpression*>(expression);
                        for (const auto& argument : call->arguments())
                        {
                            foldExpression(argument.value().get(), tokens);
                        }
                        break;
                    }
                    case NodeKind::MemberAccessExpression:
                    {
                        auto* memberAccess = static_cast<MemberAccessExpression*>(expression);
                        foldExpression(memberAccess->expression().get(), tokens);
                        break;
                    }
                    default:
                        break;
                }
            }

            void foldUnaryExpression(UnaryExpression* expression, const TokenBuffer& tokens)
            {
                auto* operand = expression->expression().get();
                foldExpression(operand, tokens);

                if (expression->unaryOperator() != UnaryOperatorKind::ValueNegation
                    && expression->unaryOperator() != UnaryOperatorKind::LogicalNegation)
                {
                    return;
                }

                const auto* operandValue = tryGetFoldedOperand(operand);
                if (operandValue == nullptr)
                {
                    return;
                }

                // the literal already carries the sign, negating again would undo it
                if (expression->unaryOperator() == UnaryOperatorKind::ValueNegation && expression->signFolded())
                {
                    expression->setFoldedValue(*operandValue);
                    return;
                }

                const auto operandType = expression->type().toValue();
                const auto* signature = m_module.tryGetOperatorSignature(operandType, expression->unaryOperator());
                if (signature == nullptr || signature->unaryFold == nullptr)
                {
                    return;
                }

                const auto folded = signature->unaryFold(*operandValue);
                if (folded.kind == FoldResultKind::Value)
                {
                    expression->setFoldedValue(folded.value);
                }
                else if (folded.kind == FoldResultKind::Overflow 
                    && (operandType == Type::I32() || operandType == Type::U8()))
                {
                    m_diagnostics.addConstantOverflowError(
                        tokens.source(),
                        expression->sourceLocation(tokens),
                        FormatTypeName(m_module, operandType));
                }
            }

            void foldBinaryExpression(BinaryExpression* expression, const TokenBuffer& tokens)
            {
                auto* left = expression->leftExpression().get();
                auto* right = expression->rightExpression().get();
                foldExpression(left, tokens);
                foldExpression(right, tokens);

                const auto* leftValue = tryGetFoldedOperand(left);
                const auto* rightValue = tryGetFoldedOperand(right);
                if (leftValue == nullptr || rightValue == nullptr)
                {
                    return;
                }

                const auto operandType = left->type().toValue();
                const auto* signature = m_module.tryGetOperatorSignature(operandType, expression->binaryOperator());
                if (signature == nullptr || signature->binaryFold == nullptr)
                {
                    return;
                }

                const auto folded = signature->binaryFold(*leftValue, *rightValue);
                if (folded.kind == FoldResultKind::Value)
                {
                    expression->setFoldedValue(folded.value);
                }
                else if (folded.kind == FoldResultKind::DivideByZero
                    && (operandType == Type::I32() || operandType == Type::U8()))
                {
                    m_diagnostics.addDivisionByZeroError(
                        tokens.source(),
                        right->sourceLocation(tokens));
                }
                else if (folded.kind == FoldResultKind::Overflow
                    && (operandType == Type::I32() || operandType == Type::U8()))
                {
                    m_diagnostics.addConstantOverflowError(
                        tokens.source(),
                        expression->sourceLocation(tokens),
                        FormatTypeName(m_module, operandType));
                }
            }

            [[nodiscard]] const FoldValue* tryGetFoldedOperand(const Expression* expression) const
            {
                while (expression != nullptr && expression->kind() == NodeKind::GroupingExpression)
                {
                    expression = static_cast<const GroupingExpression*>(expression)->expression().get();
                }

                if (expression == nullptr)
                {
                    return nullptr;
                }

                if (expression->foldedValue().has_value())
                {
                    return &expression->foldedValue().value();
                }

                if (expression->kind() == NodeKind::NameExpression)
                {
                    return tryFindConstant(static_cast<const NameExpression*>(expression)->name());
                }

                return nullptr;
            }

            [[nodiscard]] const FoldValue* tryFindConstant(const std::string& name) const
            {
                for (auto scope = m_constantScopes.rbegin(); scope != m_constantScopes.rend(); ++scope)
                {
                    const auto found = scope->find(name);
                    if (found != scope->end())
                    {
                        return &found->second;
                    }
                }

                return nullptr;
            }

            const SemanticContext& m_module;
            DiagnosticsBag& m_diagnostics;
            std::vector<std::unordered_map<std::string, FoldValue>> m_constantScopes;
        };
    }

    bool foldConstants(
        const std::vector<ParseTreeUPtr>& parseTrees,
        const SemanticContext& module,
        DiagnosticsBag& diagnostics) noexcept
    {
        const auto diagnosticCountBefore = diagnostics.diagnostics().size();

        ConstantFolder folder{ module, diagnostics };
        for (const auto& parseTree : parseTrees)
        {
            folder.foldTree(*parseTree);
        }

        return diagnostics.diagnostics().size() == diagnosticCountBefore;
    }
}
