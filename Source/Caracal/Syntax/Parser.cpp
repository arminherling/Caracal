#include <Caracal/Syntax/AssignmentStatement.h>
#include <Caracal/Syntax/BinaryExpression.h>
#include <Caracal/Syntax/BoolLiteral.h>
#include <Caracal/Syntax/BreakStatement.h>
#include <Caracal/Syntax/ConstantDeclaration.h>
#include <Caracal/Syntax/DiscardLiteral.h>
#include <Caracal/Syntax/EnumDefinitionStatement.h>
#include <Caracal/Syntax/ErrorExpression.h>
#include <Caracal/Syntax/ExpressionStatement.h>
#include <Caracal/Syntax/FunctionCallExpression.h>
#include <Caracal/Syntax/FunctionDefinitionStatement.h>
#include <Caracal/Syntax/GroupingExpression.h>
#include <Caracal/Syntax/IfStatement.h>
#include <Caracal/Syntax/MemberAccessExpression.h>
#include <Caracal/Syntax/MethodDefinitionStatement.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/Parser.h>
#include <Caracal/Syntax/ReturnStatement.h>
#include <Caracal/Syntax/SkipStatement.h>
#include <Caracal/Syntax/TypeDefinitionStatement.h>
#include <Caracal/Syntax/TypeFieldDeclaration.h>
#include <Caracal/Syntax/UnaryExpression.h>
#include <Caracal/Syntax/VariableDeclaration.h>
#include <Caracal/Syntax/WhileStatement.h>
#include <Caracal/Semantic/ExternAnnotation.h>

namespace Caracal
{
    static std::string ReplaceEscapeSequences(std::string_view input)
    {
        std::string result(input);

        auto replaceAll = [](std::string& str, const std::string& from, const std::string& to) {
            size_t startPos = 0;
            while ((startPos = str.find(from, startPos)) != std::string::npos)
            {
                str.replace(startPos, from.length(), to);
                startPos += to.length(); // Weiter nach dem ersetzten Teil suchen
            }
            };

        replaceAll(result, "\\\'", "\'");
        replaceAll(result, "\\\"", "\"");
        replaceAll(result, "\\a", "\a");
        replaceAll(result, "\\b", "\b");
        replaceAll(result, "\\f", "\f");
        replaceAll(result, "\\n", "\n");
        replaceAll(result, "\\r", "\r");
        replaceAll(result, "\\t", "\t");
        replaceAll(result, "\\v", "\v");
        replaceAll(result, "\\\\", "\\");

        return result;
    }

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
                case TokenKind::Hash: 
                {
                    if(scope == StatementScope::Global)
                    {
                        buildAnnotationNode(scope);
                        break;
                    }
                    TODO("Annotations in other scopes");
                }
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
            case TokenKind::DefKeyword:
            {
                if (scope == StatementScope::Global)
                {
                    return parseFunctionDefinitionStatement();
                }
                else if (scope == StatementScope::Type)
                {
                    return parseMethodDefinitionStatement();
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
            case TokenKind::IfKeyword:
            {
                if (scope == StatementScope::Function || scope == StatementScope::Method)
                {
                    return parseIfStatement(scope);
                }

                TODO("If statement in other scopes");
                break;
            }
            case TokenKind::WhileKeyword:
            {
                if (scope == StatementScope::Function || scope == StatementScope::Method)
                {
                    return parseWhileStatement(scope);
                }
                TODO("While statement in other scopes");
                break;
            }
            case TokenKind::BreakKeyword:
            {
                if (scope == StatementScope::Function || scope == StatementScope::Method)
                {
                    return parseBreakStatement();
                }
                TODO("Break statement in other scopes");
                break;
            }
            case TokenKind::SkipKeyword:
            {
                if (scope == StatementScope::Function || scope == StatementScope::Method)
                {
                    return parseSkipStatement();
                }
                TODO("Skip statement in other scopes");
                break;
            }
            case TokenKind::ReturnKeyword:
            {
                if (scope == StatementScope::Function || scope == StatementScope::Method)
                {
                    return parseReturnStatement(scope);
                }
                TODO("Return statement in other scopes");
                break;
            }
            case TokenKind::OpenBracket:
            {
                if (scope == StatementScope::Function || scope == StatementScope::Method)
                {
                    return parseBlockNode(scope);
                }
                TODO("Block node in other scopes");
                break;
            }
            case TokenKind::Dot:
            {
                if (scope != StatementScope::Method)
                {
                    TODO("Dot token in other scopes");
                    break;
                }
                [[fallthrough]];
            }
            case TokenKind::Underscore:
            case TokenKind::Identifier:
            {
                auto expression = parsePrimaryExpression(scope);
                if (currentToken().kind == TokenKind::Colon)
                {
                    if (scope == StatementScope::Global || scope == StatementScope::Function || scope == StatementScope::Method)
                    {
                        return parseConstantOrVariableDeclaration(std::move(expression), scope);
                    }
                    else if(scope == StatementScope::Type && expression->kind() == NodeKind::NameExpression)
                    {
                        return parseTypeFieldDeclaration(std::move(expression));
                    }
                    TODO("Constant or variable declaration in other scopes");
                    break;
                }
                if ((expression->kind() == NodeKind::NameExpression || expression->kind() == NodeKind::MemberAccessExpression) && currentToken().kind == TokenKind::Equal)
                {
                    if (scope == StatementScope::Function || scope == StatementScope::Method)
                    {
                        return parseAssignmentStatement(std::move(expression), scope);
                        break;
                    }
                    TODO("Assignment statement in other scopes");
                    break;
                }
                if (expression->kind() == NodeKind::FunctionCallExpression || expression->kind() == NodeKind::MemberAccessExpression)
                {
                    if (scope == StatementScope::Function || scope == StatementScope::Method)
                    {
                        return parseExpressionStatement(std::move(expression));
                    }
                    TODO("Function call expression in other scopes");
                    break;
                }

                TODO("Identifier in other scopes");
                break;
            }
            default:
            {
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

    StatementUPtr Parser::parseExpressionStatement(ExpressionUPtr&& expression)
    {
        auto semicolon = advanceOnMatch(TokenKind::Semicolon);

        return std::make_unique<ExpressionStatement>(std::move(expression), semicolon);
    }

    StatementUPtr Parser::parseFunctionDefinitionStatement()
    {
        auto keyword = advanceOnMatch(TokenKind::DefKeyword);
        auto nameToken = advanceOnMatch(TokenKind::Identifier);
        auto name = m_tokens.getLexeme(nameToken);
        auto parameters = parseParametersNode();
        auto returnTypes = parseReturnTypesNode();
        auto body = parseFunctionBody();

        auto optionalAnnotation = std::move(m_currentAnnotation);
        m_currentAnnotation = std::nullopt;

        return std::make_unique<FunctionDefinitionStatement>(keyword, nameToken, name, std::move(parameters), std::move(returnTypes), std::move(body), std::move(optionalAnnotation));
    }

    StatementUPtr Parser::parseConstantOrVariableDeclaration(ExpressionUPtr&& leftExpression, StatementScope scope)
    {
        auto firstColon = advanceOnMatch(TokenKind::Colon);

        std::optional<TypeNameNodeUPtr> explicitType;
        auto currentTokenKind = currentToken().kind;
        if (currentTokenKind != TokenKind::Colon && currentTokenKind != TokenKind::Equal)
        {
            explicitType = parseTypeNameNode();
        }
        
        auto secondToken = advanceOnMatch(TokenKind::Colon, TokenKind::Equal);
        auto rightExpression = parseExpression(scope);
        auto semicolon = advanceOnMatch(TokenKind::Semicolon);

        if (secondToken.kind == TokenKind::Colon)
        {
            auto isGlobalConstant = scope == StatementScope::Global;
            return std::make_unique<ConstantDeclaration>(std::move(leftExpression), firstColon, std::move(explicitType), secondToken, std::move(rightExpression), semicolon, isGlobalConstant);
        }
        else
        {
            return std::make_unique<VariableDeclaration>(std::move(leftExpression), firstColon, std::move(explicitType), secondToken, std::move(rightExpression), semicolon);
        }
    }

    StatementUPtr Parser::parseAssignmentStatement(ExpressionUPtr&& leftExpression, StatementScope scope)
    {
        auto equal = advanceOnMatch(TokenKind::Equal);
        auto rightExpression = parseExpression(scope);
        auto semicolon = advanceOnMatch(TokenKind::Semicolon);
     
        return std::make_unique<AssignmentStatement>(std::move(leftExpression), equal, std::move(rightExpression), semicolon);
    }

    StatementUPtr Parser::parseEnumDefinitionStatement()
    {
        auto keyword = advanceOnMatch(TokenKind::EnumKeyword);
        auto nameToken = advanceOnMatch(TokenKind::Identifier);
        auto name = m_tokens.getLexeme(nameToken);

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

        return std::make_unique<EnumDefinitionStatement>(keyword, nameToken, name, colonToken, std::move(baseType), openBracket, std::move(enumFields), closeBracket);
    }

    std::vector<EnumFieldDeclarationUPtr> Parser::parseEnumFields()
    {
        std::vector<EnumFieldDeclarationUPtr> fields;
        auto current = currentToken();
        while (current.kind != TokenKind::CloseBracket && current.kind != TokenKind::EndOfFile)
        {
            if (current.kind == TokenKind::Identifier)
            {
                auto nameToken = advanceOnMatch(TokenKind::Identifier);
                auto name = m_tokens.getLexeme(nameToken);
                if (currentToken().kind == TokenKind::Colon && nextToken().kind == TokenKind::Colon)
                {
                    auto colon1 = advanceOnMatch(TokenKind::Colon);
                    auto colon2 = advanceOnMatch(TokenKind::Colon);
                    auto valueExpression = parseExpression(StatementScope::Enum);
                    fields.push_back(std::make_unique<EnumFieldDeclaration>(nameToken, name, colon1, colon2, std::move(valueExpression)));
                }
                else
                {
                    fields.push_back(std::make_unique<EnumFieldDeclaration>(nameToken, name));
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
        auto nameToken = advanceOnMatch(TokenKind::Identifier);
        auto name = m_tokens.getLexeme(nameToken);
        
        std::optional<ParametersNodeUPtr> maybeParameters;
        if (currentToken().kind == TokenKind::OpenParenthesis)
        {
            maybeParameters = parseParametersNode();
        }

        auto body = parseTypeBody();

        return std::make_unique<TypeDefinitionStatement>(keyword, nameToken, name, std::move(maybeParameters), std::move(body));
    }

    StatementUPtr Parser::parseTypeFieldDeclaration(ExpressionUPtr&& leftExpression)
    {
        // the left expression is always a NameExpression
        auto nameExpression = std::unique_ptr<NameExpression>(static_cast<NameExpression*>(leftExpression.release()));
        auto firstColon = advanceOnMatch(TokenKind::Colon);
        
        std::optional<TypeNameNodeUPtr> explicitType;
        auto currentTokenKind = currentToken().kind;
        if (currentTokenKind != TokenKind::Colon && currentTokenKind != TokenKind::Equal)
        {
            explicitType = parseTypeNameNode();
        }

        auto secondToken = tryMatchKind(TokenKind::Colon);
        if (!secondToken.has_value())
        {
            secondToken = tryMatchKind(TokenKind::Equal);
        }

        std::optional<ExpressionUPtr> rightExpression;
        if (secondToken.has_value())
        {
            rightExpression = parseExpression(StatementScope::Type);
        }

        auto isConstant = secondToken.has_value() && secondToken.value().kind == TokenKind::Colon;

        return std::make_unique<TypeFieldDeclaration>(std::move(nameExpression), firstColon, std::move(explicitType), secondToken, std::move(rightExpression), isConstant);
    }

    StatementUPtr Parser::parseMethodDefinitionStatement()
    {
        auto modifier = MethodModifier::Public;
        auto keyword = advanceOnMatch(TokenKind::DefKeyword);
        auto methodNameNode = parseMethodNameNode();
        auto parameters = parseParametersNode();
        auto returnTypes = parseReturnTypesNode();
        auto body = parseMethodBody();

        if(methodNameNode->hasTypeName())
        {
            // Methods defined as "def Type.function()" are considered static
            modifier = MethodModifier::Static;
        }

        const auto& methodName = methodNameNode->methodName();
        const auto isPrivate = methodName.starts_with('_');
        if(modifier == MethodModifier::Public && isPrivate)
        {
            modifier = MethodModifier::Private;
        }
        else if (modifier == MethodModifier::Static && isPrivate)
        {
            TODO("Static methods can't begin with an underscore for now");
        }

        return std::make_unique<MethodDefinitionStatement>(
            keyword, 
            std::move(methodNameNode),
            std::move(parameters), 
            std::move(returnTypes), 
            std::move(body), 
            modifier,
            SpecialFunctionType::None);
    }
    
    BlockNodeUPtr Parser::parseFunctionBody()
    {
        return parseBlockNode(StatementScope::Function);
    }

    BlockNodeUPtr Parser::parseTypeBody()
    {
        return parseBlockNode(StatementScope::Type);
    }

    BlockNodeUPtr Parser::parseMethodBody()
    {
        return parseBlockNode(StatementScope::Method);
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
        if (currentToken().kind == TokenKind::Identifier)
        {
            auto nameToken = advanceOnMatch(TokenKind::Identifier);
            auto name = m_tokens.getLexeme(nameToken);
            auto colon = advanceOnMatch(TokenKind::Colon);
            auto typeName = parseTypeNameNode();

            return std::make_unique<ParameterNode>(nameToken, name, colon, std::move(typeName));
        }
        else
        {
            auto firstDot = advanceOnMatch(TokenKind::Dot);
            auto secondDot = advanceOnMatch(TokenKind::Dot);
            auto thirdDot = advanceOnMatch(TokenKind::Dot);
            auto fakeTypeName = std::make_unique<TypeNameNode>(std::nullopt, thirdDot, "...");

            return std::make_unique<ParameterNode>(firstDot, "varargs", secondDot, std::move(fakeTypeName));
        }
    }

    NumberLiteralUPtr Parser::parseNumberLiteral()
    {
        auto literal = advanceOnMatch(TokenKind::Number);
        auto uptick = tryMatchKind(TokenKind::Uptick);
        std::optional<TypeNameNodeUPtr> explicitType;
        if (uptick.has_value())
        {
            explicitType = parseTypeNameNode();
        }

        return std::make_unique<NumberLiteral>(literal, uptick, std::move(explicitType));
    }

    StringLiteralUPtr Parser::parseStringLiteral()
    {
        auto literal = advanceOnMatch(TokenKind::String);

        const auto lexeme = m_tokens.getLexeme(literal);
        const auto withoutQuotes = lexeme.substr(1, lexeme.size() - 2);
        const auto escapedContent = ReplaceEscapeSequences(withoutQuotes);

        return std::make_unique<StringLiteral>(literal, escapedContent);
    }

    StatementUPtr Parser::parseIfStatement(StatementScope scope)
    {
        auto ifKeyword = advanceOnMatch(TokenKind::IfKeyword);
        auto condition = parseExpression(scope);
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
        auto condition = parseExpression(scope);
        auto trueStatement = parseStatement(scope);

        return std::make_unique<WhileStatement>(whileKeyword, std::move(condition), std::move(trueStatement));
    }

    StatementUPtr Parser::parseBreakStatement()
    {
        auto keyword = advanceOnMatch(TokenKind::BreakKeyword);

        // parse trailing if statement like "break if condition;"
        if(currentToken().kind == TokenKind::IfKeyword)
        {
            auto ifKeyword = advanceOnMatch(TokenKind::IfKeyword);
            auto condition = parseExpression(StatementScope::Function);
            auto semicolon = advanceOnMatch(TokenKind::Semicolon);
            auto breakStatement = std::make_unique<BreakStatement>(keyword, semicolon);
            return std::make_unique<IfStatement>(ifKeyword, std::move(condition), std::move(breakStatement));
        }

        auto semicolon = advanceOnMatch(TokenKind::Semicolon);
        return std::make_unique<BreakStatement>(keyword, semicolon);
    }

    StatementUPtr Parser::parseSkipStatement()
    {
        auto keyword = advanceOnMatch(TokenKind::SkipKeyword);

        // parse trailing if statement like "skip if condition;"
        if(currentToken().kind == TokenKind::IfKeyword)
        {
            auto ifKeyword = advanceOnMatch(TokenKind::IfKeyword);
            auto condition = parseExpression(StatementScope::Function);
            auto semicolon = advanceOnMatch(TokenKind::Semicolon);
            auto skipStatement = std::make_unique<SkipStatement>(keyword, semicolon);

            return std::make_unique<IfStatement>(ifKeyword, std::move(condition), std::move(skipStatement));
        }

        auto semicolon = advanceOnMatch(TokenKind::Semicolon);
        return std::make_unique<SkipStatement>(keyword, semicolon);
    }
    
    StatementUPtr Parser::parseReturnStatement(StatementScope scope)
    {
        auto returnKeyword = advanceOnMatch(TokenKind::ReturnKeyword);
        std::optional<ExpressionUPtr> expression;
        if (currentToken().kind != TokenKind::Semicolon && currentToken().kind != TokenKind::IfKeyword)
        {
            expression = parseExpression(scope);
        }

        // parse trailing if statement like "return expression if condition;"
        if (currentToken().kind == TokenKind::IfKeyword)
        {
            auto ifKeyword = advanceOnMatch(TokenKind::IfKeyword);
            auto condition = parseExpression(scope);
            auto semicolon = advanceOnMatch(TokenKind::Semicolon);
            auto returnStatement = std::make_unique<ReturnStatement>(returnKeyword, std::move(expression), semicolon);

            return std::make_unique<IfStatement>(ifKeyword, std::move(condition), std::move(returnStatement));
        }

        auto semicolon = advanceOnMatch(TokenKind::Semicolon);
        return std::make_unique<ReturnStatement>(returnKeyword, std::move(expression), semicolon);
    }

    ExpressionUPtr Parser::parseExpression(StatementScope scope)
    {
        return parseBinaryExpression(0, scope);
    }

    ExpressionUPtr Parser::parseBinaryExpression(i32 parentPrecedence, StatementScope scope)
    {
        ExpressionUPtr left{};
        auto unaryOperatorToken = currentToken();

        auto unaryPrecedence = unaryOperatorPrecedence(unaryOperatorToken.kind);
        if (unaryPrecedence == 0 || unaryPrecedence < parentPrecedence)
        {
            left = parsePrimaryExpression(scope);
        }
        else
        {
            advanceCurrentIndex();
            auto expression = parseBinaryExpression(unaryPrecedence, scope);
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
            auto right = parseBinaryExpression(binaryPrecedence, scope);
            left = std::make_unique<BinaryExpression>(std::move(left), binaryOperatorToken, std::move(right));
        }

        return left;
    }

    ExpressionUPtr Parser::parsePrimaryExpression(StatementScope scope)
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
                return parseFunctionCallOrNameExpression(scope);
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
                return parseNumberLiteral();
            }
            case TokenKind::String:
            {
                return parseStringLiteral();
            }
            case TokenKind::OpenParenthesis:
            {
                return parseGroupingExpression(scope);
            }
            case TokenKind::Dot:
            {
                if (scope == StatementScope::Method)
                {
                    return parseMemberAccessExpression();
                }
                TODO("Member access in other scopes");
                [[fallthrough]];
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
    
    ExpressionUPtr Parser::parseGroupingExpression(StatementScope scope)
    {
        auto openParenthesis = advanceOnMatch(TokenKind::OpenParenthesis);
        auto expression = parseExpression(scope);
        auto closeParenthesis = advanceOnMatch(TokenKind::CloseParenthesis);
    
        return std::make_unique<GroupingExpression>(openParenthesis, std::move(expression), closeParenthesis);
    }

    ExpressionUPtr Parser::parseMemberAccessExpression()
    {
        auto dotToken = advanceOnMatch(TokenKind::Dot);
        auto expression = parseFunctionCallOrNameExpression(StatementScope::Method);
     
        return std::make_unique<MemberAccessExpression>(dotToken, std::move(expression));
    }

    NameExpressionUPtr Parser::parseNameExpression()
    {
        auto nameToken = advanceOnMatch(TokenKind::Identifier);
        const auto name = m_tokens.getLexeme(nameToken);
        return std::make_unique<NameExpression>(nameToken, name);
    }

    ExpressionUPtr Parser::parseFunctionCallOrNameExpression(StatementScope scope)
    {
        auto next = nextToken();
        if (next.kind == TokenKind::OpenParenthesis)
        {
            return parseFunctionCallExpression(scope);
        }
        else
        {
            return parseNameExpression();
        }
    }

    ExpressionUPtr Parser::parseFunctionCallExpression(StatementScope scope)
    {
        auto nameExpression = parseNameExpression();
        auto arguments = parseArgumentsNode(scope);
        return std::make_unique<FunctionCallExpression>(std::move(nameExpression), std::move(arguments));
    }

    ArgumentsNodeUPtr Parser::parseArgumentsNode(StatementScope scope)
    {
        auto openParenthesis = advanceOnMatch(TokenKind::OpenParenthesis);
        auto current = currentToken();

        std::vector<ExpressionUPtr> arguments;
        while (current.kind != TokenKind::CloseParenthesis)
        {
            auto expression = parseExpression(scope);
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
        auto nameToken = advanceOnMatch(TokenKind::Identifier);
        auto name = m_tokens.getLexeme(nameToken);
        return std::make_unique<TypeNameNode>(refToken, nameToken, name);
    }

    MethodNameNodeUPtr Parser::parseMethodNameNode()
    {
        auto firstNameToken = advanceOnMatch(TokenKind::Identifier);
        auto firstName = m_tokens.getLexeme(firstNameToken);
        auto dotToken = tryMatchKind(TokenKind::Dot);
        if (dotToken.has_value())
        {
            auto secondNameToken = advanceOnMatch(TokenKind::Identifier);
            auto secondName = m_tokens.getLexeme(secondNameToken);
            return std::make_unique<MethodNameNode>(firstNameToken, firstName, dotToken.value(), secondNameToken, secondName);
        }

        return std::make_unique<MethodNameNode>(firstNameToken, firstName);
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

    void Parser::buildAnnotationNode(StatementScope scope)
    {
        auto hashToken = advanceOnMatch(TokenKind::Hash);
        auto nameToken = advanceOnMatch(TokenKind::Identifier);
        auto name = m_tokens.getLexeme(nameToken);

        std::optional<ArgumentsNodeUPtr> arguments;
        if (currentToken().kind == TokenKind::OpenParenthesis)
        {
            arguments = parseArgumentsNode(scope);
        }

        if (m_currentAnnotation.has_value())
        {
            const auto& location = m_tokens.getSourceLocation(nameToken);
            m_diagnostics.AddError(DiagnosticKind::_0006_UnexpectedAnnotation, location);
            return;
        }

        if(name == "extern")
        {
            m_currentAnnotation = std::make_unique<ExternAnnotation>(hashToken, nameToken, name, std::move(arguments));
        }
        else
        {
            const auto& location = m_tokens.getSourceLocation(nameToken);
            m_diagnostics.AddError(DiagnosticKind::_0007_UnknownAnnotation, location);
        }
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

    Token Parser::advanceOnMatch(TokenKind kind1, TokenKind kind2)
    {
        auto current = currentToken();
        if (current.kind == kind1 || current.kind == kind2)
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
