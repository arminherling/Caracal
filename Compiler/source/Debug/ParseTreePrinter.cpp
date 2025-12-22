#include <Debug/ParseTreePrinter.h>
#include <Semantic/TypeDatabase.h>

namespace Caracal
{
    ParseTreePrinter::ParseTreePrinter(const ParseTree& parseTree, i32 indentation)
        : m_parseTree{ parseTree }
        , m_builder{ indentation }
    {
    }

    std::string ParseTreePrinter::prettyPrint()
    {
        for (const auto& statement : m_parseTree.statements())
        {
            prettyPrintNode(statement.get());
        }
        return m_builder.toString();
    }

    void ParseTreePrinter::prettyPrintNode(Node* node)
    {
        switch (node->kind())
        {
            case NodeKind::ConstantDeclaration:
            {
                prettyPrintConstantDeclaration((ConstantDeclaration*)node);
                break;
            }
            case NodeKind::VariableDeclaration:
            {
                prettyPrintVariableDeclaration((VariableDeclaration*)node);
                break;
            }
            case NodeKind::TypeFieldDeclaration:
            {
                prettyPrintTypeFieldDeclaration((TypeFieldDeclaration*)node);
                break;
            }
            case NodeKind::CppBlockStatement:
            {
                prettyPrintCppBlockStatement((CppBlockStatement*)node);
                break;
            }
            case NodeKind::ExpressionStatement:
            {
                prettyPrintExpressionStatement((ExpressionStatement*)node);
                break;
            }
            case NodeKind::AssignmentStatement:
            {
                prettyPrintAssignmentStatement((AssignmentStatement*)node);
                break;
            }
            case NodeKind::FunctionDefinitionStatement:
            {
                prettyPrintFunctionDefinitionStatement((FunctionDefinitionStatement*)node);
                break;
            }
            case NodeKind::EnumDefinitionStatement:
            {
                prettyPrintEnumDefinitionStatement((EnumDefinitionStatement*)node);
                break;
            }
            case NodeKind::TypeDefinitionStatement:
            {
                prettyPrintTypeDefinitionStatement((TypeDefinitionStatement*)node);
                break;
            }
            case NodeKind::MethodDefinitionStatement:
            {
                prettyPrintMethodDefinitionStatement((MethodDefinitionStatement*)node);
                break;
            }
            case NodeKind::IfStatement:
            {
                prettyPrintIfStatement((IfStatement*)node);
                break;
            }
            case NodeKind::WhileStatement:
            {
                prettyPrintWhileStatement((WhileStatement*)node);
                break;
            }
            case NodeKind::BreakStatement:
            {
                prettyPrintBreakStatement((BreakStatement*)node);
                break;
            }
            case NodeKind::SkipStatement:
            {
                prettyPrintSkipStatement((SkipStatement*)node);
                break;
            }
            case NodeKind::ReturnStatement:
            {
                prettyPrintReturnStatement((ReturnStatement*)node);
                break;
            }
            case NodeKind::GroupingExpression:
            {
                prettyPrintGroupingExpression((GroupingExpression*)node);
                break;
            }
            case NodeKind::UnaryExpression:
            {
                prettyPrintUnaryExpression((UnaryExpression*)node);
                break;
            }
            case NodeKind::BinaryExpression:
            {
                prettyPrintBinaryExpression((BinaryExpression*)node);
                break;
            }
            case NodeKind::NameExpression:
            {
                prettyPrintNameExpression((NameExpression*)node);
                break;
            }
            case NodeKind::FunctionCallExpression:
            {
                prettyPrintFunctionCallExpression((FunctionCallExpression*)node);
                break;
            }
            case NodeKind::MemberAccessExpression:
            {
                prettyPrintMemberAccessExpression((MemberAccessExpression*)node);
                break;
            }
            case NodeKind::DiscardLiteral:
            {
                prettyPrintDiscardLiteral((DiscardLiteral*)node);
                break;
            }
            case NodeKind::BoolLiteral:
            {
                prettyPrintBoolLiteral((BoolLiteral*)node);
                break;
            }
            case NodeKind::NumberLiteral:
            {
                prettyPrintNumberLiteral((NumberLiteral*)node);
                break;
            }
            case NodeKind::StringLiteral:
            {
                prettyPrintStringLiteral((StringLiteral*)node);
                break;
            }
            case NodeKind::TypeNameNode:
            {
                prettyPrintTypeNameNode((TypeNameNode*)node);
                break;
            }
            case NodeKind::BlockNode:
            {
                prettyPrintBlockNode((BlockNode*)node);
                break;
            }
            default:
            {
                m_builder.appendIndentedLine("Missing NodeKind!!");
                break;
            }
        }
    }

    void ParseTreePrinter::prettyPrintConstantDeclaration(ConstantDeclaration* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();
        
        m_builder.appendIndentedLine("Left: {");
        m_builder.pushIndentation();

        prettyPrintNode(statement->leftExpression().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");

        if (statement->explicitType().has_value())
        {
            m_builder.appendIndentedLine("ExplicitType: {");
            m_builder.pushIndentation();

            prettyPrintNode(statement->explicitType().value().get());

            m_builder.popIndentation();
            m_builder.appendIndentedLine("}");
        }

        m_builder.appendIndentedLine("Right: {");
        m_builder.pushIndentation();

        prettyPrintNode(statement->rightExpression().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
        
        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintVariableDeclaration(VariableDeclaration* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();
        
        m_builder.appendIndentedLine("Left: {");
        m_builder.pushIndentation();

        prettyPrintNode(statement->leftExpression().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
        
        if (statement->explicitType().has_value())
        {
            m_builder.appendIndentedLine("ExplicitType: {");
            m_builder.pushIndentation();

            prettyPrintNode(statement->explicitType().value().get());

            m_builder.popIndentation();
            m_builder.appendIndentedLine("}");
        }

        if(statement->rightExpression().has_value())
        {
            m_builder.appendIndentedLine("Right: {");
            m_builder.pushIndentation();

            prettyPrintNode(statement->rightExpression().value().get());

            m_builder.popIndentation();
            m_builder.appendIndentedLine("}");
        }
        else
        {
            m_builder.appendIndentedLine("Right: null");
        }
     
        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintTypeFieldDeclaration(TypeFieldDeclaration* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();

        m_builder.appendIndented("IsConstant: ").appendLine(statement->isConstant() ? "true" : "false");
        m_builder.appendIndentedLine("Left: {");
        m_builder.pushIndentation();

        prettyPrintNode(statement->nameExpression().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");

        if (statement->explicitType().has_value())
        {
            m_builder.appendIndentedLine("ExplicitType: {");
            m_builder.pushIndentation();

            prettyPrintNode(statement->explicitType().value().get());

            m_builder.popIndentation();
            m_builder.appendIndentedLine("}");
        }

        if (statement->rightExpression().has_value())
        {
            m_builder.appendIndentedLine("Right: {");
            m_builder.pushIndentation();

            prettyPrintNode(statement->rightExpression().value().get());

            m_builder.popIndentation();
            m_builder.appendIndentedLine("}");
        }
        else
        {
            m_builder.appendIndentedLine("Right: null");
        }

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}")
            ;
    }

    void ParseTreePrinter::prettyPrintCppBlockStatement(CppBlockStatement* node)
    {
        const auto& lines = node->lines();

        m_builder.appendIndented(stringify(node->kind())).appendLine(": {");
        m_builder.pushIndentation();

        m_builder.appendIndented("Lines (").append(std::to_string(lines.size())).appendLine("): {");
        m_builder.pushIndentation();

        for (const auto& line : lines)
        {
            auto lexeme = m_parseTree.tokens().getLexeme(line);
            m_builder.appendIndentedLine(lexeme);
        }

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintExpressionStatement(ExpressionStatement* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();
    
        prettyPrintNode(statement->expression().get());
        
        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }
    
    void ParseTreePrinter::prettyPrintAssignmentStatement(AssignmentStatement* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();
        
        m_builder.appendIndentedLine("Left: {");
        m_builder.pushIndentation();

        prettyPrintNode(statement->leftExpression().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
        
        m_builder.appendIndentedLine("Right: {");
        m_builder.pushIndentation();

        prettyPrintNode(statement->rightExpression().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
        
        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintTypeDefinitionStatement(TypeDefinitionStatement* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();

        prettyPrintNameExpression(statement->nameExpression().get());
        if (statement->constructorParameters().has_value())
        {
            prettyPrintParametersNode(statement->constructorParameters().value().get());
        }
        prettyPrintBlockNode(statement->bodyNode().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintFunctionDefinitionStatement(FunctionDefinitionStatement* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();

        prettyPrintNameExpression(statement->nameExpression().get());
        prettyPrintParametersNode(statement->parametersNode().get());
        prettyPrintReturnTypesNode(statement->returnTypesNode().get());
        prettyPrintBlockNode(statement->bodyNode().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintMethodDefinitionStatement(MethodDefinitionStatement* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();

        if (statement->specialFunctionType() != SpecialFunctionType::None)
        {
            m_builder.appendIndented("SpecialFunctionType: ").appendLine(stringify(statement->specialFunctionType()));
        }
        m_builder.appendIndented("Modifier: ").appendLine(stringify(statement->modifier()));
        prettyPrintMethodNameNode(statement->methodNameNode().get());
        prettyPrintParametersNode(statement->parametersNode().get());
        prettyPrintReturnTypesNode(statement->returnTypesNode().get());
        prettyPrintBlockNode(statement->bodyNode().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintEnumDefinitionStatement(EnumDefinitionStatement* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();
        prettyPrintNameExpression(statement->nameExpression().get());

        if (statement->baseType().has_value())
        {
            prettyPrintTypeNameNode(statement->baseType().value().get());
        }
    
        const auto& fieldNodes = statement->fieldNodes();
        m_builder.appendIndented("FieldNodes(").append(std::to_string(fieldNodes.size())).appendLine("): {");
        m_builder.pushIndentation();
    
        for (const auto& fieldNode : fieldNodes)
        {
            prettyPrintEnumFieldDeclaration(fieldNode.get());
        }
    
        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    
        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }
    
    void ParseTreePrinter::prettyPrintEnumFieldDeclaration(EnumFieldDeclaration* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();
        prettyPrintNameExpression(statement->nameExpression().get());
    
        if (statement->valueExpression().has_value())
        {
            prettyPrintNode(statement->valueExpression().value().get());
        }
    
        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }
    
    void ParseTreePrinter::prettyPrintIfStatement(IfStatement* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();

        m_builder.appendIndentedLine("Condition: {");
        m_builder.pushIndentation();

        prettyPrintNode(statement->condition().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");

        m_builder.appendIndentedLine("True: {");
        m_builder.pushIndentation();

        prettyPrintNode(statement->trueStatement().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");

        if (statement->hasFalseBlock())
        {
            m_builder.appendIndentedLine("False: {");
            m_builder.pushIndentation();

            prettyPrintNode(statement->falseStatement().value().get());

            m_builder.popIndentation();
            m_builder.appendIndentedLine("}");
        }

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintWhileStatement(WhileStatement* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();

        m_builder.appendIndentedLine("Condition: {");
        m_builder.pushIndentation();

        prettyPrintNode(statement->condition().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");

        m_builder.appendIndentedLine("True: {");
        m_builder.pushIndentation();

        prettyPrintNode(statement->trueStatement().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintBreakStatement(BreakStatement* statement)
    {
        m_builder.appendIndentedLine(stringify(statement->kind()));
    }

    void ParseTreePrinter::prettyPrintSkipStatement(SkipStatement* statement)
    {
        m_builder.appendIndentedLine(stringify(statement->kind()));
    }

    void ParseTreePrinter::prettyPrintReturnStatement(ReturnStatement* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();

        if (statement->expression().has_value())
            prettyPrintNode(statement->expression().value().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintUnaryExpression(UnaryExpression* unaryExpression)
    {
        m_builder.appendIndented(stringify(unaryExpression->kind())).appendLine(": {");
        m_builder.pushIndentation();

        m_builder.appendIndented("Operation: ").appendLine(stringify(unaryExpression->unaryOperator()));
        m_builder.appendIndentedLine("Expression: {");

        m_builder.pushIndentation();
        
        prettyPrintNode(unaryExpression->expression().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintBinaryExpression(BinaryExpression* binaryExpression)
    {
        m_builder.appendIndented(stringify(binaryExpression->kind())).appendLine(": {");
        m_builder.pushIndentation();

        m_builder.appendIndentedLine("Left: {");
        m_builder.pushIndentation();

        prettyPrintNode(binaryExpression->leftExpression().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");

        m_builder.appendIndented("Operation: ").appendLine(stringify(binaryExpression->binaryOperator()));

        m_builder.appendIndentedLine("Right: {");
        m_builder.pushIndentation();

        prettyPrintNode(binaryExpression->rightExpression().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintNameExpression(NameExpression* name)
    {
        m_builder.appendIndented(stringify(name->kind())).appendLine(": {");
        m_builder.pushIndentation();

        const auto& identifierToken = name->nameToken();
        const auto identifierLexeme = m_parseTree.tokens().getLexeme(identifierToken);
        m_builder.appendIndented("Name: ").appendLine(identifierLexeme);

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintMethodNameNode(MethodNameNode* methodName)
    {
        m_builder.appendIndented(stringify(methodName->kind())).appendLine(": {");
        m_builder.pushIndentation();

        m_builder.appendIndented("MethodName: ");
        if (methodName->typeNameExpression().has_value())
        {
            const auto& typeNameToken = methodName->typeNameExpression().value()->nameToken();
            const auto typeNameLexeme = m_parseTree.tokens().getLexeme(typeNameToken);
            m_builder.append(typeNameLexeme).append(".");
        }
        const auto& methodNameToken = methodName->methodNameExpression()->nameToken();
        const auto methodNameLexeme = m_parseTree.tokens().getLexeme(methodNameToken);
        m_builder.appendLine(methodNameLexeme);

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintFunctionCallExpression(FunctionCallExpression* functionCall)
    {
        m_builder.appendIndented(stringify(functionCall->kind())).appendLine(": {");
        m_builder.pushIndentation();

        prettyPrintNameExpression(functionCall->nameExpression().get());
        prettyPrintArgumentsNode(functionCall->argumentsNode().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintMemberAccessExpression(MemberAccessExpression* expression)
    {
        m_builder.appendIndented(stringify(expression->kind())).appendLine(": {");
        m_builder.pushIndentation();

        prettyPrintNode(expression->expression().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintDiscardLiteral(DiscardLiteral* /*discard*/)
    {
        m_builder.appendIndentedLine("Discard: _");
    }

    void ParseTreePrinter::prettyPrintBoolLiteral(BoolLiteral* node)
    {
        const auto lexeme = m_parseTree.tokens().getLexeme(node->literalToken());
        m_builder.appendIndented(stringify(node->kind())).append(": ").appendLine(lexeme);
    }

    void ParseTreePrinter::prettyPrintNumberLiteral(NumberLiteral* number)
    {
        const auto lexeme = m_parseTree.tokens().getLexeme(number->literalToken());
        m_builder.appendIndented(stringify(number->kind())).append(": ").appendLine(lexeme);
        if(number->explicitType().has_value())
        {
            prettyPrintTypeNameNode(number->explicitType().value().get());
        }
    }

    void ParseTreePrinter::prettyPrintStringLiteral(StringLiteral* string)
    {
        const auto lexeme = m_parseTree.tokens().getLexeme(string->literalToken());
        m_builder.appendIndented(stringify(string->kind())).append(": ").appendLine(lexeme);
    }
    
    void ParseTreePrinter::prettyPrintArgumentsNode(ArgumentsNode* node)
    {
        const auto& arguments = node->arguments();
        m_builder.appendIndented(stringify(node->kind())).append("(").append(std::to_string(arguments.size())).appendLine("): {");
        m_builder.pushIndentation();
    
        for (const auto& argument : arguments)
            prettyPrintNode(argument.get());
    
        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }
    
    void ParseTreePrinter::prettyPrintParameterNode(ParameterNode* parameter)
    {
        m_builder.appendIndented(stringify(parameter->kind())).appendLine(": {");
        m_builder.pushIndentation();

        const auto& nameToken = parameter->nameExpression()->nameToken();
        const auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
        m_builder.appendIndented("Name: ").appendLine(nameLexeme);
        prettyPrintTypeNameNode(parameter->typeName().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintParametersNode(ParametersNode* node)
    {
        const auto& parameters = node->parameters();
        m_builder.appendIndented(stringify(node->kind())).append("(").append(std::to_string(parameters.size())).appendLine("): {");
        m_builder.pushIndentation();

        for (const auto& parameter : parameters)
            prettyPrintParameterNode(parameter.get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintReturnTypesNode(ReturnTypesNode* node)
    {
        const auto& returnTypes = node->returnTypes();
        m_builder.appendIndented(stringify(node->kind())).append("(").append(std::to_string(returnTypes.size())).appendLine("): {");
        m_builder.pushIndentation();

        for (const auto& returnType : returnTypes)
            prettyPrintTypeNameNode(returnType.get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintTypeNameNode(TypeNameNode* node)
    {
        m_builder.appendIndented(stringify(node->kind())).appendLine(": {");
        m_builder.pushIndentation();

        const auto type = node->type();
        const auto typeName = TypeDatabase::TryFindName(type);

        m_builder.appendIndented("Type: ").appendLine(typeName);
        if (node->isReference())
        {
            m_builder.appendIndentedLine("Ref: true");
        }
        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintBlockNode(BlockNode* block)
    {
        const auto& statements = block->statements();
        m_builder.appendIndented(stringify(block->kind())).append("(").append(std::to_string(statements.size())).appendLine("): {");
        m_builder.pushIndentation();

        for (const auto& statement : block->statements())
            prettyPrintNode(statement.get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintGroupingExpression(GroupingExpression* grouping)
    {
        m_builder.appendIndented(stringify(grouping->kind())).appendLine(": {");
        m_builder.pushIndentation();
        prettyPrintNode(grouping->expression().get());
        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }
}

//void ParseTreePrinter::PrettyPrintTypeDefinitionStatement(TypeDefinitionStatement* statement)
//{
//    auto nameToken = statement->name();
//    auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
//
//    stream() << Indentation() << stringify(statement->kind()) << QString(": {") << NewLine();
//    m_builder.pushIndentation();
//    stream() << Indentation() << stringify(NodeKind::NameExpression) << QString(": %1").arg(nameLexeme) << NewLine();
//    PrettyPrintBlockNode(statement->body());
//    m_builder.popIndentation();
//    m_builder.appendIndentedLine("}");
//}
//
//void ParseTreePrinter::PrettyPrintFieldDefinitionStatement(FieldDefinitionStatement* statement)
//{
//    stream() << Indentation() << stringify(statement->kind()) << QString(": {") << NewLine();
//    m_builder.pushIndentation();
//
//    stream() << Indentation() << QString("Field: {") << NewLine();
//    m_builder.pushIndentation();
//    PrettyPrintNode(statement->name());
//    m_builder.popIndentation();
//    m_builder.appendIndentedLine("}");
//
//    if (statement->type().has_value())
//        PrettyPrintTypeName(statement->type().value());
//
//    if (statement->expression().has_value())
//    {
//        stream() << Indentation() << QString("Value: {") << NewLine();
//        m_builder.pushIndentation();
//        PrettyPrintNode(statement->expression().value());
//        m_builder.popIndentation();
//        m_builder.appendIndentedLine("}");
//    }
//
//    m_builder.popIndentation();
//    m_builder.appendIndentedLine("}");
//}

//void ParseTreePrinter::PrettyPrintWhileStatement(WhileStatement* statement)
//{
//    stream() << Indentation() << stringify(statement->kind()) << QString(": {") << NewLine();
//    m_builder.pushIndentation();
//
//    stream() << Indentation() << QString("Condition: {") << NewLine();
//    m_builder.pushIndentation();
//    PrettyPrintNode(statement->condition());
//    m_builder.popIndentation();
//    m_builder.appendIndentedLine("}");
//
//    PrettyPrintBlockNode(statement->body());
//
//    m_builder.popIndentation();
//    m_builder.appendIndentedLine("}");
//}

//void ParseTreePrinter::PrettyPrintParameterNode(ParameterNode* parameter)
//{
//    stream() << Indentation() << stringify(parameter->kind()) << QString(": {") << NewLine();
//    m_builder.pushIndentation();
//
//    PrettyPrintNameExpression(parameter->name());
//    PrettyPrintTypeName(parameter->type());
//
//    m_builder.popIndentation();
//    m_builder.appendIndentedLine("}");
//}

//void ParseTreePrinter::PrettyPrintFunctionCallExpression(FunctionCallExpression* functionCall)
//{
//    auto nameToken = functionCall->name();
//    auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
//    stream() << Indentation() << stringify(functionCall->kind()) << QString(": {") << NewLine();
//
//    m_builder.pushIndentation();
//    stream() << Indentation() << stringify(NodeKind::NameExpression) << QString(": %1").arg(nameLexeme) << NewLine();
//    PrettyPrintArgumentsNode(functionCall->arguments());
//    m_builder.popIndentation();
//    m_builder.appendIndentedLine("}");
//}

//
//void ParseTreePrinter::PrettyPrintMemberAccessExpression(MemberAccessExpression* memberAccess)
//{
//    stream() << Indentation() << stringify(memberAccess->kind()) << QString(": {") << NewLine();
//    m_builder.pushIndentation();
//
//    PrettyPrintNode(memberAccess->expression());
//
//    m_builder.popIndentation();
//    m_builder.appendIndentedLine("}");
//}
//
//void ParseTreePrinter::PrettyPrintError(Error* error)
//{
//    stream() << Indentation() << QString("Error!!") << NewLine();
//}
