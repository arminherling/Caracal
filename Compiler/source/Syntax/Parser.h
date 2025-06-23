#pragma once

#include <Defines.h>
#include <Compiler/API.h>
#include <Compiler/DiagnosticsBag.h>
#include <Syntax/ArgumentsNode.h>
#include <Syntax/BlockNode.h>
//#include <Syntax/EnumFieldDefinitionStatement.h>
#include <Syntax/EnumFieldDeclaration.h>
#include <Syntax/Expression.h>
#include <Syntax/NameExpression.h>
//#include <Syntax/ParameterNode.h>
#include <Syntax/ParametersNode.h>
#include <Syntax/ReturnTypesNode.h>
#include <Syntax/ParseTree.h>
#include <Syntax/TokenBuffer.h>
#include <Syntax/TypeNameNode.h>

namespace Caracal
{
    class COMPILER_API Parser
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
            Type
        };

        std::vector<StatementUPtr> parseStatements(StatementScope scope);
        StatementUPtr parseStatement(StatementScope scope);
        StatementUPtr parseCppBlock();
        StatementUPtr parseExpressionStatement(ExpressionUPtr&& expression);
        StatementUPtr parseFunctionDefinitionStatement();
        StatementUPtr parseConstantOrVariableDeclaration(ExpressionUPtr&& leftExpression);
        StatementUPtr parseAssignmentStatement(ExpressionUPtr&& leftExpression);
        StatementUPtr parseEnumDefinitionStatement();
        std::vector<EnumFieldDeclarationUPtr> parseEnumFields();
        StatementUPtr parseTypeDefinitionStatement();
        StatementUPtr parseTypeFieldDeclaration(ExpressionUPtr&& leftExpression);
        StatementUPtr parseMethodDefinitionStatement();
        StatementUPtr parseIfStatement(StatementScope scope);
        StatementUPtr parseWhileStatement(StatementScope scope);
        StatementUPtr parseBreakStatement();
        StatementUPtr parseSkipStatement();
        StatementUPtr parseReturnStatement();
        ExpressionUPtr parseExpression();
        ExpressionUPtr parseBinaryExpression(i32 parentPrecedence);
        ExpressionUPtr parsePrimaryExpression();
        ExpressionUPtr parseGroupingExpression();
        NameExpressionUPtr parseNameExpression();
        ExpressionUPtr parseFunctionCallOrNameExpression();
        ExpressionUPtr parseFunctionCallExpression();
        TypeNameNodeUPtr parseTypeNameNode();
        ParametersNodeUPtr parseParametersNode();
        ReturnTypesNodeUPtr parseReturnTypesNode();
        ArgumentsNodeUPtr parseArgumentsNode();
        BlockNodeUPtr parseFunctionBody();
        BlockNodeUPtr parseTypeBody();
        BlockNodeUPtr parseMethodBody();
        BlockNodeUPtr parseBlockNode(StatementScope scope);
        ParameterNodeUPtr parseParameterNode();
        //NumberLiteral* parseNumberLiteral();

        Token advanceOnMatch(TokenKind kind);
        //std::optional<BoolLiteral*> tryParseBoolLiteral();
        std::optional<Token> tryMatchKind(TokenKind kind);
        //void skipUntil(TokenKind kind);

        Token peek(i32 offset);
        Token currentToken() { return peek(0); }
        Token nextToken() { return peek(1); }
        void advanceCurrentIndex() { m_currentIndex++; }

        //i32 lineDistanceSinceLastToken();
        //bool tokenIsOnNextLine();
        //bool hasEmptyLineSinceLastToken();

        TokenBuffer m_tokens;
        DiagnosticsBag& m_diagnostics;
        i32 m_currentIndex;
    };

    COMPILER_API ParseTree parse(const TokenBuffer& tokens, DiagnosticsBag& diagnostics) noexcept;
}
