#include <Syntax/Parser.h>
#include <Syntax/CppBlockStatement.h>
#include <Syntax/BoolLiteral.h>
#include <Syntax/NumberLiteral.h>
#include <Syntax/StringLiteral.h>
#include <Syntax/ErrorExpression.h>
#include <Semantic/TypeDatabase.h>
#include <Syntax/AssignmentStatement.h>
#include <Syntax/ConstantDeclaration.h>
#include <Syntax/VariableDeclaration.h>
#include <Syntax/GroupingExpression.h>
#include <Syntax/ExpressionStatement.h>
#include <Syntax/FunctionDefinitionStatement.h>
#include <Syntax/EnumDefinitionStatement.h>
#include <Syntax/TypeDefinitionStatement.h>
//#include <Syntax/FieldDefinitionStatement.h>
//#include <Syntax/MethodDefinitionStatement.h>
#include <Syntax/UnaryExpression.h>
#include <Syntax/BinaryExpression.h>
#include <Syntax/NameExpression.h>
#include <Syntax/IfStatement.h>
#include <Syntax/WhileStatement.h>
#include <Syntax/BreakStatement.h>
#include <Syntax/SkipStatement.h>
#include <Syntax/ReturnStatement.h>
#include <Syntax/DiscardLiteral.h>
//#include <Syntax/MemberAccessExpression.h>
//#include <Syntax/ScopeAccessExpression.h>
#include <Syntax/FunctionCallExpression.h>

namespace Caracal
{
    Parser::Parser(const TokenBuffer& tokens, DiagnosticsBag& diagnostics)
        : m_tokens{ tokens }
        , m_diagnostics{ diagnostics }
        , m_currentIndex{ 0 }
    {
    }

    ParseTree Parser::parse()
    {
        auto statements = parseStatements(StatementScope::Global);
        if (m_currentIndex < m_tokens.size() - 1)
        {
            const auto& location = m_tokens.getSourceLocation(m_tokens.getToken(m_currentIndex));
            m_diagnostics.AddError(DiagnosticKind::_0004_ExtraTokensRemaining, location);
        }

        return ParseTree(m_tokens, std::move(statements));
    }

    std::vector<StatementUPtr> Parser::parseStatements(StatementScope scope)
    {
        std::vector<StatementUPtr> statements{};
        auto current = currentToken();
        while (true)
        {
            switch (current.kind)
            {
                case TokenKind::CloseBracket:
                {
                    if (scope == StatementScope::Global)
                    {
                        const auto& location = m_tokens.getSourceLocation(current);
                        m_diagnostics.AddError(DiagnosticKind::Unknown, location);

                        advanceCurrentIndex();
                        break;
                    }
                    [[fallthrough]];
                }
                case TokenKind::EndOfFile:
                {
                    return statements;
                }
                default:
                {
                    statements.push_back(parseStatement(scope));
                    break;
                }
            }
            
            current = currentToken();
        }
    }

    StatementUPtr Parser::parseStatement(StatementScope scope)
    {
        const auto current = currentToken();
        switch (current.kind)
        {
            case TokenKind::OpenBracket:
            {
                if (scope == StatementScope::Function)
                {
                    return parseBlockNode(scope);
                }
                TODO("Block node in other scopes");
                break;
            }
            case TokenKind::DefKeyword:
            {
                if (scope == StatementScope::Global)
                {
                    return parseFunctionDefinitionStatement();
                }
                TODO("Function definition in other scopes");
                break;
            }
            case TokenKind::EnumKeyword:
            {
                if (scope == StatementScope::Global)
                {
                    return parseEnumDefinitionStatement();
                }
                TODO("Enum definition in other scopes");
                break;
            }
            case TokenKind::TypeKeyword:
            {
                if (scope == StatementScope::Global)
                {
                    return parseTypeDefinitionStatement();
                }
                TODO("Type definition in other scopes");
                break;
            }
            case TokenKind::CppKeyword:
            {
                if (scope == StatementScope::Global || scope == StatementScope::Function)
                {
                    return parseCppBlock();
                }
                TODO("Cpp block in other scopes");
                break;
            }
            case TokenKind::IfKeyword:
            {
                if (scope == StatementScope::Function)
                {
                    return parseIfStatement(scope);
                }

                TODO("If statement in other scopes");
                break;
            }
            case TokenKind::WhileKeyword:
            {
                if (scope == StatementScope::Function)
                {
                    return parseWhileStatement(scope);
                }
                TODO("While statement in other scopes");
                break;
            }
            case TokenKind::BreakKeyword:
            {
                if (scope == StatementScope::Function)
                {
                    return parseBreakStatement();
                }
                TODO("Break statement in other scopes");
                break;
            }
            case TokenKind::SkipKeyword:
            {
                if (scope == StatementScope::Function)
                {
                    return parseSkipStatement();
                }
                TODO("Skip statement in other scopes");
                break;
            }
            case TokenKind::ReturnKeyword:
            {
                if (scope == StatementScope::Function)
                {
                    return parseReturnStatement();
                }
                TODO("Return statement in other scopes");
                break;
            }
            case TokenKind::Underscore:
            case TokenKind::Identifier:
            {
                auto expression = parsePrimaryExpression();
                if (currentToken().kind == TokenKind::Colon && nextToken().kind == TokenKind::Colon)
                {
                    if (scope == StatementScope::Global || scope == StatementScope::Function)
                    {
                        return parseConstantDeclaration(std::move(expression));
                    }
                    TODO("Emit an error");
                    break;
                }
                if (currentToken().kind == TokenKind::Colon && nextToken().kind == TokenKind::Equal)
                {
                    if (scope == StatementScope::Function)
                    {
                        return parseVariableDeclaration(std::move(expression));
                    }
                    TODO("Variable declaration in other scopes");
                    break;
                }
                if (expression->kind() == NodeKind::FunctionCallExpression)
                {
                    if (scope == StatementScope::Function)
                    {
                        return parseExpressionStatement(std::move(expression));
                    }
                    TODO("Function call expression in other scopes");
                    break;
                }
                if (expression->kind() == NodeKind::NameExpression && currentToken().kind == TokenKind::Equal)
                {
                    if (scope == StatementScope::Function)
                    {
                        return parseAssignmentStatement(std::move(expression));
                        break;
                    }
                    TODO("Assignment statement in other scopes");
                    break;
                }

                TODO("Identifier in other scopes");
                break;
            }
        }

        TODO("Unexpected token in parseStatement");
        const auto& location = m_tokens.getSourceLocation(current);
        m_diagnostics.AddError(DiagnosticKind::Unknown, location);

        auto errorExpression = std::make_unique<ErrorExpression>(current);
        advanceCurrentIndex();
        return std::make_unique<ExpressionStatement>(std::move(errorExpression), Token::ToError(current));
    }

    StatementUPtr Parser::parseCppBlock()
    {
        const auto cppKeyword = advanceOnMatch(TokenKind::CppKeyword);
        const auto openBracket = advanceOnMatch(TokenKind::OpenBracket);

        std::vector<Token> lines;
        auto current = currentToken();
        while (current.kind == TokenKind::String)
        {
            lines.push_back(current);
            advanceCurrentIndex();
            current = currentToken();
        }

        const auto closeBracket = advanceOnMatch(TokenKind::CloseBracket);

        return std::make_unique<CppBlockStatement>(cppKeyword, openBracket, lines, closeBracket);
    }
    
    StatementUPtr Parser::parseExpressionStatement(ExpressionUPtr&& expression)
    {
        auto semicolon = advanceOnMatch(TokenKind::Semicolon);

        return std::make_unique<ExpressionStatement>(std::move(expression), semicolon);
    }

    StatementUPtr Parser::parseFunctionDefinitionStatement()
    {
        auto keyword = advanceOnMatch(TokenKind::DefKeyword);
        auto nameExpression = parseNameExpression();
        auto parameters = parseParametersNode();
        auto returnTypes = parseReturnTypesNode();
        auto body = parseFunctionBody();

        return std::make_unique<FunctionDefinitionStatement>(keyword, std::move(nameExpression), std::move(parameters), std::move(returnTypes), std::move(body));
    }

    StatementUPtr Parser::parseConstantDeclaration(ExpressionUPtr&& leftExpression)
    {
        auto firstColon = advanceOnMatch(TokenKind::Colon);
        auto secondColon = advanceOnMatch(TokenKind::Colon);
        auto rightExpression = parseExpression();
        auto semicolon = advanceOnMatch(TokenKind::Semicolon);

        return std::make_unique<ConstantDeclaration>(std::move(leftExpression), firstColon, secondColon, std::move(rightExpression), semicolon);
    }

    StatementUPtr Parser::parseVariableDeclaration(ExpressionUPtr&& leftExpression)
    {
        auto colon = advanceOnMatch(TokenKind::Colon);
        auto equal = advanceOnMatch(TokenKind::Equal);
        auto rightExpression = parseExpression();
        auto semicolon = advanceOnMatch(TokenKind::Semicolon);

        return std::make_unique<VariableDeclaration>(std::move(leftExpression), colon, equal, std::move(rightExpression), semicolon);
    }

    StatementUPtr Parser::parseAssignmentStatement(ExpressionUPtr&& leftExpression)
    {
        auto equal = advanceOnMatch(TokenKind::Equal);
        auto rightExpression = parseExpression();
        auto semicolon = advanceOnMatch(TokenKind::Semicolon);
     
        return std::make_unique<AssignmentStatement>(std::move(leftExpression), equal, std::move(rightExpression), semicolon);
    }

    StatementUPtr Parser::parseEnumDefinitionStatement()
    {
        auto keyword = advanceOnMatch(TokenKind::EnumKeyword);
        auto nameExpression = parseNameExpression();
    
        auto current = currentToken();
        std::optional<Token> colonToken;
        std::optional<TypeNameNodeUPtr> baseType;
        if (current.kind == TokenKind::Colon)
        {
            colonToken = advanceOnMatch(TokenKind::Colon);
            baseType = parseTypeNameNode();
        }
    
        auto openBracket = advanceOnMatch(TokenKind::OpenBracket);
        auto enumFields = parseEnumFields();
        auto closeBracket = advanceOnMatch(TokenKind::CloseBracket);
    
        return std::make_unique<EnumDefinitionStatement>(keyword, std::move(nameExpression), colonToken, std::move(baseType), openBracket, std::move(enumFields), closeBracket);
    }

    std::vector<EnumFieldNodeUPtr> Parser::parseEnumFields()
    {
        std::vector<EnumFieldNodeUPtr> fields;
        auto current = currentToken();
        while (current.kind != TokenKind::CloseBracket && current.kind != TokenKind::EndOfFile)
        {
            if (current.kind == TokenKind::Identifier)
            {
                auto nameExpression = parseNameExpression();
                if (currentToken().kind == TokenKind::Colon && nextToken().kind == TokenKind::Colon)
                {
                    auto colon1 = advanceOnMatch(TokenKind::Colon);
                    auto colon2 = advanceOnMatch(TokenKind::Colon);
                    auto valueExpression = parseExpression();
                    fields.push_back(std::make_unique<EnumFieldNode>(std::move(nameExpression), colon1, colon2, std::move(valueExpression)));
                }
                else
                {
                    fields.push_back(std::make_unique<EnumFieldNode>(std::move(nameExpression)));
                }
            }
            else
            {
                const auto& location = m_tokens.getSourceLocation(current);
                m_diagnostics.AddError(DiagnosticKind::_0005_ExpectedEnumField, location);
            }
            current = currentToken();
        }
        return fields;
    }

    StatementUPtr Parser::parseTypeDefinitionStatement()
    {
        auto keyword = advanceOnMatch(TokenKind::TypeKeyword);
        auto nameExpression = parseNameExpression();
        auto body = parseTypeBody();

        return std::make_unique<TypeDefinitionStatement>(keyword, std::move(nameExpression), std::move(body));
    }
    
    BlockNodeUPtr Parser::parseFunctionBody()
    {
        return parseBlockNode(StatementScope::Function);
    }

    BlockNodeUPtr Parser::parseTypeBody()
    {
        auto openBracket = advanceOnMatch(TokenKind::OpenBracket);
        auto statements = std::vector<StatementUPtr>();
        auto closeBracket = advanceOnMatch(TokenKind::CloseBracket);

        return std::make_unique<BlockNode>(openBracket, std::move(statements), closeBracket);
    }

    BlockNodeUPtr Parser::parseBlockNode(StatementScope scope)
    {
        auto openBracket = advanceOnMatch(TokenKind::OpenBracket);
        auto statements = parseStatements(scope);
        auto closeBracket = advanceOnMatch(TokenKind::CloseBracket);

        return std::make_unique<BlockNode>(openBracket, std::move(statements), closeBracket);
    }

    ParameterNodeUPtr Parser::parseParameterNode()
    {
        auto name = parseNameExpression();
        auto colon = advanceOnMatch(TokenKind::Colon);
        auto typeName = parseTypeNameNode();

        return std::make_unique<ParameterNode>(std::move(name), colon, std::move(typeName));
    }

    StatementUPtr Parser::parseIfStatement(StatementScope scope)
    {
        auto ifKeyword = advanceOnMatch(TokenKind::IfKeyword);
        auto condition = parseExpression();
        auto trueStatement = parseStatement(scope);

        if (currentToken().kind == TokenKind::ElseKeyword)
        {
            auto elseKeyword = advanceOnMatch(TokenKind::ElseKeyword);
            auto falseStatement = parseStatement(scope);
            return std::make_unique<IfStatement>(ifKeyword, std::move(condition), std::move(trueStatement), elseKeyword, std::move(falseStatement));
        }

        return std::make_unique<IfStatement>(ifKeyword, std::move(condition), std::move(trueStatement));
    }

    StatementUPtr Parser::parseWhileStatement(StatementScope scope)
    {
        auto whileKeyword = advanceOnMatch(TokenKind::WhileKeyword);
        auto condition = parseExpression();
        auto trueStatement = parseStatement(scope);

        return std::make_unique<WhileStatement>(whileKeyword, std::move(condition), std::move(trueStatement));
    }

    StatementUPtr Parser::parseBreakStatement()
    {
        auto keyword = advanceOnMatch(TokenKind::BreakKeyword);
        auto semicolon = advanceOnMatch(TokenKind::Semicolon);
        return std::make_unique<BreakStatement>(keyword, semicolon);
    }

    StatementUPtr Parser::parseSkipStatement()
    {
        auto keyword = advanceOnMatch(TokenKind::SkipKeyword);
        auto semicolon = advanceOnMatch(TokenKind::Semicolon);
        return std::make_unique<SkipStatement>(keyword, semicolon);
    }
    
    StatementUPtr Parser::parseReturnStatement()
    {
        auto returnKeyword = advanceOnMatch(TokenKind::ReturnKeyword);
        std::optional<ExpressionUPtr> expression;
        if (currentToken().kind != TokenKind::Semicolon)
        {
            expression = parseExpression();
        }
        auto semicolon = advanceOnMatch(TokenKind::Semicolon);

        return std::make_unique<ReturnStatement>(returnKeyword, std::move(expression), semicolon);
    }

    ExpressionUPtr Parser::parseExpression()
    {
        return parseBinaryExpression(0);
    }

    ExpressionUPtr Parser::parseBinaryExpression(i32 parentPrecedence)
    {
        ExpressionUPtr left{};
        auto unaryOperatorToken = currentToken();

        auto unaryPrecedence = unaryOperatorPrecedence(unaryOperatorToken.kind);
        if (unaryPrecedence == 0 || unaryPrecedence < parentPrecedence)
        {
            left = parsePrimaryExpression();
        }
        else
        {
            advanceCurrentIndex();
            auto expression = parseBinaryExpression(unaryPrecedence);
            left = std::make_unique<UnaryExpression>(unaryOperatorToken, std::move(expression));
        }

        while (true)
        {
            auto binaryOperatorToken = currentToken();
            if (binaryOperatorToken.kind == TokenKind::EndOfFile)
                break;

            auto binaryPrecedence = binaryOperatorPrecedence(binaryOperatorToken.kind);
            if (binaryPrecedence == 0 || binaryPrecedence <= parentPrecedence)
                break;

            advanceCurrentIndex();
            auto right = parseBinaryExpression(binaryPrecedence);
            left = std::make_unique<BinaryExpression>(std::move(left), binaryOperatorToken, std::move(right));
        }

        return left;
    }

    ExpressionUPtr Parser::parsePrimaryExpression()
    {
        auto current = currentToken();
        switch (current.kind)
        {
            case TokenKind::Underscore:
            {
                advanceCurrentIndex();
                return std::make_unique<DiscardLiteral>(current);
            }
            case TokenKind::Identifier:
            {
                return parseFunctionCallOrNameExpression();
            }
            case TokenKind::TrueKeyword:
            {
                advanceCurrentIndex();
                return std::make_unique<BoolLiteral>(current, true);
            }
            case TokenKind::FalseKeyword:
            {
                advanceCurrentIndex();
                return std::make_unique<BoolLiteral>(current, false);
            }
            case TokenKind::Number:
            {
                advanceCurrentIndex();
                return std::make_unique<NumberLiteral>(current);
            }
            case TokenKind::String:
            {
                advanceCurrentIndex();
                return std::make_unique<StringLiteral>(current);
            }
            case TokenKind::OpenParenthesis:
            {
                return parseGroupingExpression();
            }
            default:
            {
                advanceCurrentIndex();
                const auto& location = m_tokens.getSourceLocation(current);
                m_diagnostics.AddError(DiagnosticKind::Unknown, location);
                return std::make_unique<ErrorExpression>(current);
            }
        }
    }
    
    ExpressionUPtr Parser::parseGroupingExpression()
    {
        auto openParenthesis = advanceOnMatch(TokenKind::OpenParenthesis);
        auto expression = parseExpression();
        auto closeParenthesis = advanceOnMatch(TokenKind::CloseParenthesis);
    
        return std::make_unique<GroupingExpression>(openParenthesis, std::move(expression), closeParenthesis);
    }

    NameExpressionUPtr Parser::parseNameExpression()
    {
        auto name = advanceOnMatch(TokenKind::Identifier);
        return std::make_unique<NameExpression>(name);
    }

    ExpressionUPtr Parser::parseFunctionCallOrNameExpression()
    {
        auto next = nextToken();
        if (next.kind == TokenKind::OpenParenthesis)
        {
            return parseFunctionCallExpression();
        }
        else
        {
            return parseNameExpression();
        }
    }

    ExpressionUPtr Parser::parseFunctionCallExpression()
    {
        auto nameExpression = parseNameExpression();
        auto arguments = parseArgumentsNode();
        return std::make_unique<FunctionCallExpression>(std::move(nameExpression), std::move(arguments));
    }

    ArgumentsNodeUPtr Parser::parseArgumentsNode()
    {
        auto openParenthesis = advanceOnMatch(TokenKind::OpenParenthesis);
        auto current = currentToken();

        std::vector<ExpressionUPtr> arguments;
        while (current.kind != TokenKind::CloseParenthesis)
        {
            auto expression = parseExpression();
            arguments.push_back(std::move(expression));
            if (currentToken().kind == TokenKind::Comma)
            {
                advanceCurrentIndex();

                // if(CurrentToken().kind == TokenKind::CloseParenthesis)
                // TODO print error for too many commas or too few arguments
            }
            current = currentToken();
        }

        auto closeParenthesis = advanceOnMatch(TokenKind::CloseParenthesis);
        return std::make_unique<ArgumentsNode>(openParenthesis, std::move(arguments), closeParenthesis);
    }

    TypeNameNodeUPtr Parser::parseTypeNameNode()
    {
        auto refToken = tryMatchKind(TokenKind::RefKeyword);
        auto nameExpression = parseNameExpression();
        auto& nameToken = nameExpression->nameToken();
        auto type = TypeDatabase::TryFindBuiltin(m_tokens.getLexeme(nameToken));

        return std::make_unique<TypeNameNode>(refToken, std::move(nameExpression), type);
    }

    ParametersNodeUPtr Parser::parseParametersNode()
    {
        auto openParenthesis = advanceOnMatch(TokenKind::OpenParenthesis);

        std::vector<ParameterNodeUPtr> parameters;
        auto current = currentToken();
        while (current.kind != TokenKind::CloseParenthesis)
        {
            auto parameter = parseParameterNode();
            parameters.push_back(std::move(parameter));
            // TODO we need a function like skipUntil but for multiple tokens until we find a comma, identifier closing parent or EOF
            if (currentToken().kind == TokenKind::Comma)
            {
                advanceCurrentIndex();
        
                // if(CurrentToken().kind == TokenKind::CloseParenthesis)
                // Too many commas or too few parameters
            }
            current = currentToken();
        }
        
        auto closeParenthesis = advanceOnMatch(TokenKind::CloseParenthesis);

        return std::make_unique<ParametersNode>(openParenthesis, std::move(parameters), closeParenthesis);
    }

    ReturnTypesNodeUPtr Parser::parseReturnTypesNode()
    {
        std::vector<TypeNameNodeUPtr> returnTypes;
        auto current = currentToken();
        if (current.kind != TokenKind::OpenBracket)
        {
            auto typeName = parseTypeNameNode();
            returnTypes.push_back(std::move(typeName));

            // TODO multiple return types
        }

        return std::make_unique<ReturnTypesNode>(std::move(returnTypes));
    }

    Token Parser::advanceOnMatch(TokenKind kind)
    {
        auto current = currentToken();
        if (current.kind == kind)
        {
            advanceCurrentIndex();
            return current;
        }
        else
        {
            const auto& location = m_tokens.getSourceLocation(current);
            m_diagnostics.AddError(DiagnosticKind::_0003_ExpectedXButGotY, location);
            return Token::ToError(current);
        }
    }

    std::optional<Token> Parser::tryMatchKind(TokenKind kind)
    {
        auto current = currentToken();
        if (current.kind == kind)
        {
            advanceCurrentIndex();
            return std::make_optional<Token>(current);
        }
    
        return std::optional<Token>();
    }

    Token Parser::peek(i32 offset)
    {
        auto index = m_currentIndex + offset;
        if (index >= m_tokens.size())
            return Token{ .kind = TokenKind::EndOfFile };

        return m_tokens.getToken(index);
    }

    ParseTree parse(const TokenBuffer& tokens, DiagnosticsBag& diagnostics) noexcept
    {
        Parser parser{ tokens, diagnostics };
        return parser.parse();
    }
}


//Statement* Parser::parseFieldDefinitionStatement()
//{
//    auto name = parseNameExpression();
//    auto current = currentToken();
//    std::optional<Token> colon;
//    std::optional<TypeName> type;
//    if (current.kind == TokenKind::Colon)
//    {
//        advanceCurrentIndex();
//        colon = current;
//        type = parseTypeNode();
//        current = currentToken();
//    }
//
//    std::optional<Token> equals;
//    std::optional<Expression*> expression;
//    if (current.kind == TokenKind::Equal)
//    {
//        equals = current;
//        advanceCurrentIndex();
//        expression = parseExpression();
//    }
//
//    return new FieldDefinitionStatement(name, colon, type, equals, expression);
//}
//
//Statement* Parser::parseMethodDefinitionStatement()
//{
//    auto keyword = advanceOnMatch(TokenKind::Identifier);
//    auto name = advanceOnMatch(TokenKind::Identifier);
//    auto signature = parseParametersNode();
//    auto body = parseMethodBody();
//
//    return new MethodDefinitionStatement(keyword, name, signature, body);
//}
// 
//
//NumberLiteral* Parser::parseNumberLiteral()
//{
//    auto number = advanceOnMatch(TokenKind::Number);
//
//    auto current = currentToken();
//    if (current.kind == TokenKind::Colon)
//    {
//        advanceCurrentIndex();
//        auto type = parseTypeNode();
//        // TODO add diagnostic for invalid type
//        return new NumberLiteral(number, current, type);
//    }
//
//    return new NumberLiteral(number);
//}

//
//void Parser::skipUntil(TokenKind kind)
//{
//    auto current = currentToken();
//    while (current.kind != kind || current.kind == TokenKind::EndOfFile)
//    {
//        const auto& location = m_tokens.getSourceLocation(current);
//        m_diagnostics.AddError(DiagnosticKind::Unknown, location);
//
//        advanceCurrentIndex();
//        current = currentToken();
//    }
//}

//
//i32 Parser::lineDistanceSinceLastToken()
//{
//    auto lastToken = peek(-1);
//    auto current = currentToken();
//    auto& lastTokenLocation = m_tokens.getSourceLocation(lastToken);
//    auto& currentTokenLocation = m_tokens.getSourceLocation(current);
//
//    //return currentTokenLocation.startLine - lastTokenLocation.endLine;
//    return 0;
//}
