#pragma once

#include <Defines.h>
#include <Compiler/API.h>
#include <Compiler/DiagnosticsBag.h>
//#include <Syntax/ArgumentsNode.h>
//#include <Syntax/BinaryExpression.h>
#include <Syntax/BlockNode.h>
//#include <Syntax/BoolLiteral.h>
//#include <Syntax/EnumFieldDefinitionStatement.h>
#include <Syntax/Expression.h>
//#include <Syntax/GroupingExpression.h>
//#include <Syntax/NumberLiteral.h>
//#include <Syntax/ParameterNode.h>
#include <Syntax/ParametersNode.h>
#include <Syntax/ReturnTypesNode.h>
#include <Syntax/ParseTree.h>
//#include <Syntax/Statement.h>
#include <Syntax/TokenBuffer.h>
//#include <Syntax/UnaryExpression.h>

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

        //QList<Statement*> parseGlobalStatements();
        std::vector<StatementUPtr> parseStatements(StatementScope scope);
        StatementUPtr parseCppBlock();
        //Statement* parseAssignmentStatement();
        //Statement* parseExpressionStatement();
        StatementUPtr parseFunctionDefinitionStatement();
        StatementUPtr parseConstantDeclaration(const Token& identifier);
        StatementUPtr parseVariableDeclaration(const Token& identifier);
        StatementUPtr parseAssignmentStatement(const Token& identifier);
        //Statement* parseEnumDefinitionStatement();
        //QList<EnumFieldDefinitionStatement*> parseEnumFieldDefinitions();
        //Statement* parseTypeDefinitionStatement();
        //Statement* parseFieldDefinitionStatement();
        //Statement* parseMethodDefinitionStatement();
        //Statement* parseIfStatement(StatementScope scope);
        //Statement* parseWhileStatement(StatementScope scope);
        StatementUPtr parseReturnStatement();
        ExpressionUPtr parseExpression();
        ExpressionUPtr parseBinaryExpression(i32 parentPrecedence);
        ExpressionUPtr parsePrimaryExpression();
        //Expression* parseFunctionCallOrNameExpression();
        //Expression* parseFunctionCallExpression();
        ParametersNodeUPtr parseParametersNode();
        ReturnTypesNodeUPtr parseReturnTypesNode();
        //ArgumentsNode* parseArgumentsNode();
        BlockNodeUPtr parseFunctionBody();
        //BlockNode* parseTypeBody();
        //BlockNode* parseMethodBody();
        BlockNodeUPtr parseBlockNode(StatementScope scope);
        //ParameterNodeUPtr parseParameterNode();
        //TypeName parseTypeNode();
        //NameExpression* parseNameExpression();
        //NumberLiteral* parseNumberLiteral();
        //GroupingExpression* parseGroupingExpression();
        //EnumFieldDefinitionStatement* parseEnumFieldDefinitionStatement();

        Token advanceOnMatch(TokenKind kind);
        Token advanceOnMatchEither(TokenKind kind1, TokenKind kind2);
        //std::optional<BoolLiteral*> tryParseBoolLiteral();
        //std::optional<Token> tryMatchKind(TokenKind kind);
        //void skipUntil(TokenKind kind);

        Token peek(i32 offset);
        Token currentToken() { return peek(0); }
        Token nextToken() { return peek(1); }
        void advanceCurrentIndex() { m_currentIndex++; }

        //i32 lineDistanceSinceLastToken();
        //bool tokenIsOnNextLine();
        //bool hasEmptyLineSinceLastToken();

        //UnaryOperatornKind convertUnaryOperatorTokenKindToEnum(TokenKind kind) const;
        //BinaryOperatornKind convertBinaryOperatorTokenKindToEnum(TokenKind kind) const;

        TokenBuffer m_tokens;
        DiagnosticsBag& m_diagnostics;
        i32 m_currentIndex;
    };

    COMPILER_API ParseTree parse(const TokenBuffer& tokens, DiagnosticsBag& diagnostics) noexcept;
}
