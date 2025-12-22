#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Syntax/CppBlockStatement.h>
#include <Caracal/Syntax/FunctionDefinitionStatement.h>
#include <Caracal/Syntax/ReturnStatement.h>
#include <Caracal/Syntax/BoolLiteral.h>
#include <Caracal/Syntax/NumberLiteral.h>
#include <Caracal/Syntax/StringLiteral.h>
#include <Caracal/Syntax/ConstantDeclaration.h>
#include <Caracal/Syntax/VariableDeclaration.h>
#include <Caracal/Syntax/AssignmentStatement.h>
#include <Caracal/Syntax/BinaryExpression.h>
#include <Caracal/Syntax/DiscardLiteral.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/UnaryExpression.h>
#include <Caracal/Syntax/GroupingExpression.h>
#include <Caracal/Syntax/FunctionCallExpression.h>
#include <Caracal/Syntax/ExpressionStatement.h>
#include <Caracal/Syntax/EnumDefinitionStatement.h>
#include <Caracal/Syntax/BlockNode.h>
#include <Caracal/Syntax/IfStatement.h>
#include <Caracal/Syntax/WhileStatement.h>
#include <Caracal/Syntax/BreakStatement.h>
#include <Caracal/Syntax/SkipStatement.h>
#include <Caracal/Syntax/TypeDefinitionStatement.h>
#include <Caracal/Syntax/TypeFieldDeclaration.h>
#include <Caracal/Syntax/MethodDefinitionStatement.h>
#include <Caracal/Syntax/MemberAccessExpression.h>
#include <Caracal/Text/StringBuilder.h>

namespace Caracal
{
    class CARACAL_API CppCodeGenerator
    {
    public:
        CppCodeGenerator(const ParseTree& parseTree, i32 indentation = 4);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(CppCodeGenerator)

        [[nodiscard]] std::string generate();

    private:
        enum class Scope
        {
            Global,
            Function
        };

        struct CppTypeDef
        {
            std::string_view name;
            ParametersNode* constructorParameters = nullptr;
            std::vector<TypeFieldDeclaration*> publicFields;
            std::vector<MethodDefinitionStatement*> publicMethods;
            std::vector<MethodDefinitionStatement*> staticMethods;
            std::vector<MethodDefinitionStatement*> privateMethods;
        };

        void generateNode(Node* node)  noexcept;
        void generateConstantDeclaration(ConstantDeclaration* node) noexcept;
        void generateVariableDeclaration(VariableDeclaration* node) noexcept;
        void generateTypeFieldDeclaration(TypeFieldDeclaration* node) noexcept;
        void generateConstructorDeclarationSignature(std::string_view className, ParametersNode* node) noexcept;
        void generateConstructorDefinition(CppTypeDef* cppType) noexcept;
        void generateDestructorDeclarationSignature(std::string_view className) noexcept;
        void generateMethodDeclarationSignature(MethodDefinitionStatement* node) noexcept;
        void generateMethodDefinition(const std::string_view& typeName, MethodDefinitionStatement* node) noexcept;
        void generateGlobalDiscardedExpression(Expression* expression) noexcept;
        void generateLocalDiscardedExpression(Expression* expression) noexcept;
        void generateCppBlock(CppBlockStatement* node) noexcept;
        void generateBlockNode(BlockNode* node) noexcept;
        void generateExpressionStatement(ExpressionStatement* node) noexcept;
        void generateAssignmentStatement(AssignmentStatement* node) noexcept;
        std::string generateEnumSignature(EnumDefinitionStatement* node) noexcept;
        std::string generateFunctionSignatureReturnPart(ReturnTypesNode* returnTypesNode, bool isMainFunction) noexcept;
        std::string generateFunctionSignatureNamePart(std::string_view functionName) noexcept;
        std::string generateFunctionSignatureParameterPart(ParametersNode* parametersNode) noexcept;
        void generateTypeDefinitionStatement(TypeDefinitionStatement* node) noexcept;
        std::string generateTypeFieldName(NameExpression* node) noexcept;
        void generateFunctionDefinition(FunctionDefinitionStatement* node) noexcept;
        void generateEnumDefinitionStatement(EnumDefinitionStatement* node) noexcept;
        void generateIfStatement(IfStatement* node) noexcept;
        void generateWhileStatement(WhileStatement* node) noexcept;
        void generateBreakStatement(BreakStatement* node) noexcept;
        void generateSkipStatement(SkipStatement* node) noexcept;
        void generateReturnStatement(ReturnStatement* node) noexcept;
        void generateGroupingExpression(GroupingExpression* node) noexcept;
        void generateUnaryExpression(UnaryExpression* node) noexcept;
        void generateBinaryExpression(BinaryExpression* node) noexcept;
        void generateNameExpression(NameExpression* node) noexcept;
        void generateFunctionCallExpression(FunctionCallExpression* node) noexcept;
        void generateMemberAccessExpression(MemberAccessExpression* node) noexcept;
        void generateBoolLiteral(BoolLiteral* node) noexcept;
        void generateNumberLiteral(NumberLiteral* node) noexcept;
        void generateStringLiteral(StringLiteral* node) noexcept;

        void generateBuiltinPrintFunction(ArgumentsNode* node) noexcept;

        CppTypeDef* buildCppTypeDefinition(TypeDefinitionStatement* node) noexcept;
        std::string_view getCppNameForType(TypeNameNode* typeName) noexcept;

        const ParseTree& m_parseTree;
        Scope m_currentScope;
        i32 m_discardCount;
        NodeKind m_currentStatement;
        StringBuilder m_builder;
        StringBuilder m_cppIncludes;
        StringBuilder m_forwardDeclarations;

        std::unordered_map<std::string_view, std::unique_ptr<CppTypeDef>> m_cppTypeDefs;
    };

    CARACAL_API std::string generateCpp(const ParseTree& parseTree) noexcept;
}
