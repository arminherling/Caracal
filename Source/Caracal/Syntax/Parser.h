#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>
#include <Caracal/Debug/DiagnosticsBag.h>
#include <Caracal/Syntax/ArgumentsNode.h>
#include <Caracal/Syntax/BlockNode.h>
#include <Caracal/Syntax/EnumFieldDeclaration.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/MethodNameNode.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/NumberLiteral.h>
#include <Caracal/Syntax/ParametersNode.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Syntax/ReturnTypesNode.h>
#include <Caracal/Syntax/TokenBuffer.h>
#include <Caracal/Syntax/TypeNameNode.h>

namespace Caracal
{
    class CARACAL_API Parser
    {
    public:
        Parser(const TokenBuffer& tokens, DiagnosticsBag& diagnostics);

        ParseTree parse();

    private:
        enum class StatementScope
        {
            Global,
            Function,
            Method,
            Type,
            Enum
        };

        std::vector<StatementUPtr> parseStatements(StatementScope scope);
        StatementUPtr parseStatement(StatementScope scope);
        StatementUPtr parseCppBlock();
        StatementUPtr parseExpressionStatement(ExpressionUPtr&& expression);
        StatementUPtr parseFunctionDefinitionStatement();
        StatementUPtr parseConstantOrVariableDeclaration(ExpressionUPtr&& leftExpression, StatementScope scope);
        StatementUPtr parseAssignmentStatement(ExpressionUPtr&& leftExpression, StatementScope scope);
        StatementUPtr parseEnumDefinitionStatement();
        std::vector<EnumFieldDeclarationUPtr> parseEnumFields();
        StatementUPtr parseTypeDefinitionStatement();
        StatementUPtr parseTypeFieldDeclaration(ExpressionUPtr&& leftExpression);
        StatementUPtr parseMethodDefinitionStatement();
        StatementUPtr parseIfStatement(StatementScope scope);
        StatementUPtr parseWhileStatement(StatementScope scope);
        StatementUPtr parseBreakStatement();
        StatementUPtr parseSkipStatement();
        StatementUPtr parseReturnStatement(StatementScope scope);
        ExpressionUPtr parseExpression(StatementScope scope);
        ExpressionUPtr parseBinaryExpression(i32 parentPrecedence, StatementScope scope);
        ExpressionUPtr parsePrimaryExpression(StatementScope scope);
        ExpressionUPtr parseGroupingExpression(StatementScope scope);
        ExpressionUPtr parseMemberAccessExpression();
        NameExpressionUPtr parseNameExpression();
        ExpressionUPtr parseFunctionCallOrNameExpression(StatementScope scope);
        ExpressionUPtr parseFunctionCallExpression(StatementScope scope);
        TypeNameNodeUPtr parseTypeNameNode();
        MethodNameNodeUPtr parseMethodNameNode();
        ParametersNodeUPtr parseParametersNode();
        ReturnTypesNodeUPtr parseReturnTypesNode();
        ArgumentsNodeUPtr parseArgumentsNode(StatementScope scope);
        BlockNodeUPtr parseFunctionBody();
        BlockNodeUPtr parseTypeBody();
        BlockNodeUPtr parseMethodBody();
        BlockNodeUPtr parseBlockNode(StatementScope scope);
        ParameterNodeUPtr parseParameterNode();
        NumberLiteralUPtr parseNumberLiteral();

        Token peek(i32 offset);
        Token currentToken() { return peek(0); }
        Token nextToken() { return peek(1); }
        void advanceCurrentIndex() { m_currentIndex++; }
        Token advanceOnMatch(TokenKind kind);
        std::optional<Token> tryMatchKind(TokenKind kind);

        TokenBuffer m_tokens;
        DiagnosticsBag& m_diagnostics;
        i32 m_currentIndex;
    };

    CARACAL_API ParseTree parse(const TokenBuffer& tokens, DiagnosticsBag& diagnostics) noexcept;
}
