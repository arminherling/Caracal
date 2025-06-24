#include <Debug/ParseTreePrinter.h>
#include <Semantic/TypeDatabase.h>

namespace Caracal
{
    ParseTreePrinter::ParseTreePrinter(ParseTree& parseTree, i32 indentation)
        : BasePrinter(indentation)
        , m_parseTree{ parseTree }
    {
    }

    QString ParseTreePrinter::prettyPrint()
    {
        for (const auto& statement : m_parseTree.statements())
        {
            prettyPrintNode(statement.get());
        }

        return toUtf8();
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
                stream() << indentation() << QString("Missing NodeKind!!") << newLine();
                break;
            }
        }
    }

    void ParseTreePrinter::prettyPrintConstantDeclaration(ConstantDeclaration* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();
        
        stream() << indentation() << QString("Left: {") << newLine();
        pushIndentation();
        prettyPrintNode(statement->leftExpression().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();

        if (statement->explicitType().has_value())
        {
            stream() << indentation() << QString("ExplicitType: {") << newLine();
            pushIndentation();
            prettyPrintNode(statement->explicitType().value().get());
            popIndentation();
            stream() << indentation() << QString("}") << newLine();
        }

        stream() << indentation() << QString("Right: {") << newLine();
        pushIndentation();
        prettyPrintNode(statement->rightExpression().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
        
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintVariableDeclaration(VariableDeclaration* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();
        
        stream() << indentation() << QString("Left: {") << newLine();
        pushIndentation();
        prettyPrintNode(statement->leftExpression().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
        
        if (statement->explicitType().has_value())
        {
            stream() << indentation() << QString("ExplicitType: {") << newLine();
            pushIndentation();
            prettyPrintNode(statement->explicitType().value().get());
            popIndentation();
            stream() << indentation() << QString("}") << newLine();
        }

        if(statement->rightExpression().has_value())
        {
            stream() << indentation() << QString("Right: {") << newLine();
            pushIndentation();
            prettyPrintNode(statement->rightExpression().value().get());
            popIndentation();
            stream() << indentation() << QString("}") << newLine();
        }
        else
        {
            stream() << indentation() << QString("Right: null") << newLine();
        }
     
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintTypeFieldDeclaration(TypeFieldDeclaration* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();

        stream() << indentation() << QString("IsConstant: %1").arg(statement->isConstant() ? "true" : "false") << newLine();
        stream() << indentation() << QString("Left: {") << newLine();
        pushIndentation();
        prettyPrintNode(statement->nameExpression().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();

        if (statement->explicitType().has_value())
        {
            stream() << indentation() << QString("ExplicitType: {") << newLine();
            pushIndentation();
            prettyPrintNode(statement->explicitType().value().get());
            popIndentation();
            stream() << indentation() << QString("}") << newLine();
        }

        if (statement->rightExpression().has_value())
        {
            stream() << indentation() << QString("Right: {") << newLine();
            pushIndentation();
            prettyPrintNode(statement->rightExpression().value().get());
            popIndentation();
            stream() << indentation() << QString("}") << newLine();
        }
        else
        {
            stream() << indentation() << QString("Right: null") << newLine();
        }

        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintCppBlockStatement(CppBlockStatement* node)
    {
        const auto& lines = node->lines();

        stream() << indentation() << stringify(node->kind()) << QString(": {") << newLine();
        pushIndentation();
        stream() << indentation() << QString("Lines (%1): {").arg(lines.size()) << newLine();
        pushIndentation();
        for (const auto& line : lines)
        {
            auto lexeme = m_parseTree.tokens().getLexeme(line);
            stream() << indentation() << QString("%1").arg(lexeme) << newLine();
        }
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintExpressionStatement(ExpressionStatement* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();
    
        prettyPrintNode(statement->expression().get());
        
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }
    
    void ParseTreePrinter::prettyPrintAssignmentStatement(AssignmentStatement* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();
        
        stream() << indentation() << QString("Left: {") << newLine();
        pushIndentation();
        prettyPrintNode(statement->leftExpression().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
        
        stream() << indentation() << QString("Right: {") << newLine();
        pushIndentation();
        prettyPrintNode(statement->rightExpression().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
        
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintTypeDefinitionStatement(TypeDefinitionStatement* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();

        prettyPrintNameExpression(statement->nameExpression().get());
        prettyPrintBlockNode(statement->bodyNode().get());

        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintFunctionDefinitionStatement(FunctionDefinitionStatement* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();
        prettyPrintNameExpression(statement->nameExpression().get());
        prettyPrintParametersNode(statement->parametersNode().get());
        prettyPrintReturnTypesNode(statement->returnTypesNode().get());
        prettyPrintBlockNode(statement->bodyNode().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintMethodDefinitionStatement(MethodDefinitionStatement* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();
        stream() << indentation() << QString("Modifier: %1").arg(stringify(statement->modifier())) << newLine();
        prettyPrintMethodNameNode(statement->methodNameNode().get());
        prettyPrintParametersNode(statement->parametersNode().get());
        prettyPrintReturnTypesNode(statement->returnTypesNode().get());
        prettyPrintBlockNode(statement->bodyNode().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintEnumDefinitionStatement(EnumDefinitionStatement* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();
        prettyPrintNameExpression(statement->nameExpression().get());

        if (statement->baseType().has_value())
        {
            prettyPrintTypeNameNode(statement->baseType().value().get());
        }
    
        const auto& fieldNodes = statement->fieldNodes();
        stream() << indentation() << QString("FieldNodes(%1): {").arg(fieldNodes.size()) << newLine();
        pushIndentation();
    
        for (const auto& fieldNode : fieldNodes)
        {
            prettyPrintEnumFieldDeclaration(fieldNode.get());
        }
    
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }
    
    void ParseTreePrinter::prettyPrintEnumFieldDeclaration(EnumFieldDeclaration* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();
        prettyPrintNameExpression(statement->nameExpression().get());
    
        if (statement->valueExpression().has_value())
        {
            prettyPrintNode(statement->valueExpression().value().get());
        }
    
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }
    
    void ParseTreePrinter::prettyPrintIfStatement(IfStatement* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();

        stream() << indentation() << QString("Condition: {") << newLine();
        pushIndentation();
        prettyPrintNode(statement->condition().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();

        stream() << indentation() << QString("True: {") << newLine();
        pushIndentation();
        prettyPrintNode(statement->trueStatement().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();

        if (statement->hasFalseBlock())
        {
            stream() << indentation() << QString("False: {") << newLine();
            pushIndentation();
            prettyPrintNode(statement->falseStatement().value().get());
            popIndentation();
            stream() << indentation() << QString("}") << newLine();
        }

        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintWhileStatement(WhileStatement* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();

        stream() << indentation() << QString("Condition: {") << newLine();
        pushIndentation();
        prettyPrintNode(statement->condition().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();

        stream() << indentation() << QString("True: {") << newLine();
        pushIndentation();
        prettyPrintNode(statement->trueStatement().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();

        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintBreakStatement(BreakStatement* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << newLine();
    }

    void ParseTreePrinter::prettyPrintSkipStatement(SkipStatement* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << newLine();
    }

    void ParseTreePrinter::prettyPrintReturnStatement(ReturnStatement* statement)
    {
        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();

        if (statement->expression().has_value())
            prettyPrintNode(statement->expression().value().get());

        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintUnaryExpression(UnaryExpression* unaryExpression)
    {
        stream() << indentation() << stringify(unaryExpression->kind()) << QString(": {") << newLine();
        pushIndentation();
        stream() << indentation() << QString("Operation: ") << stringify(unaryExpression->unaryOperator()) << newLine();
        stream() << indentation() << QString("Expression: {") << newLine();
        pushIndentation();
        prettyPrintNode(unaryExpression->expression().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintBinaryExpression(BinaryExpression* binaryExpression)
    {
        stream() << indentation() << stringify(binaryExpression->kind()) << QString(": {") << newLine();
        pushIndentation();

        stream() << indentation() << QString("Left: {") << newLine();
        pushIndentation();
        prettyPrintNode(binaryExpression->leftExpression().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();

        stream() << indentation() << QString("Operation: ") << stringify(binaryExpression->binaryOperator()) << newLine();

        stream() << indentation() << QString("Right: {") << newLine();
        pushIndentation();
        prettyPrintNode(binaryExpression->rightExpression().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();

        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintNameExpression(NameExpression* name)
    {
        stream() << indentation() << stringify(name->kind()) << QString(": {") << newLine();
        pushIndentation();
        const auto& identifierToken = name->nameToken();
        const auto identifierLexeme = m_parseTree.tokens().getLexeme(identifierToken);
        stream() << indentation() << QString("Name: %1").arg(identifierLexeme) << newLine();
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintMethodNameNode(MethodNameNode* methodName)
    {
        stream() << indentation() << stringify(methodName->kind()) << QString(": {") << newLine();
        pushIndentation();
        stream() << indentation() << QString("MethodName: ");
        if (methodName->typeNameExpression().has_value())
        {
            const auto& typeNameToken = methodName->typeNameExpression().value()->nameToken();
            const auto typeNameLexeme = m_parseTree.tokens().getLexeme(typeNameToken);
            stream() << typeNameLexeme << QString(".");
        }
        const auto& methodNameToken = methodName->methodNameExpression()->nameToken();
        const auto methodNameLexeme = m_parseTree.tokens().getLexeme(methodNameToken);
        stream() << methodNameLexeme << newLine();
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintFunctionCallExpression(FunctionCallExpression* functionCall)
    {
        stream() << indentation() << stringify(functionCall->kind()) << QString(": {") << newLine();
        pushIndentation();
        prettyPrintNameExpression(functionCall->nameExpression().get());
        prettyPrintArgumentsNode(functionCall->argumentsNode().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintDiscardLiteral(DiscardLiteral* /*discard*/)
    {
        stream() << indentation() << QString("Discard: _") << newLine();
    }

    void ParseTreePrinter::prettyPrintBoolLiteral(BoolLiteral* node)
    {
        const auto lexeme = m_parseTree.tokens().getLexeme(node->literalToken());
        stream() << indentation() << stringify(node->kind()) << QString(": %1").arg(lexeme) << newLine();
    }

    void ParseTreePrinter::prettyPrintNumberLiteral(NumberLiteral* number)
    {
        const auto lexeme = m_parseTree.tokens().getLexeme(number->literalToken());
        stream() << indentation() << stringify(number->kind()) << QString(": %1").arg(lexeme) << newLine();
        if(number->explicitType().has_value())
        {
            prettyPrintTypeNameNode(number->explicitType().value().get());
        }
    }

    void ParseTreePrinter::prettyPrintStringLiteral(StringLiteral* string)
    {
        const auto lexeme = m_parseTree.tokens().getLexeme(string->literalToken());
        stream() << indentation() << stringify(string->kind()) << QString(": %1").arg(lexeme) << newLine();
    }
    
    void ParseTreePrinter::prettyPrintArgumentsNode(ArgumentsNode* node)
    {
        const auto& arguments = node->arguments();
        stream() << indentation() << stringify(node->kind()) << QString("(%1): {").arg(arguments.size()) << newLine();
        pushIndentation();
    
        for (const auto& argument : arguments)
            prettyPrintNode(argument.get());
    
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }
    
    void ParseTreePrinter::prettyPrintParameterNode(ParameterNode* parameter)
    {
        stream() << indentation() << stringify(parameter->kind()) << QString(": {") << newLine();
        pushIndentation();

        const auto& nameToken = parameter->nameExpression()->nameToken();
        const auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
        stream() << indentation() << QString("Name: %1").arg(nameLexeme) << newLine();
        prettyPrintTypeNameNode(parameter->typeName().get());

        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintParametersNode(ParametersNode* node)
    {
        const auto& parameters = node->parameters();
        stream() << indentation() << stringify(node->kind()) << QString("(%1): {").arg(parameters.size()) << newLine();
        pushIndentation();

        for (const auto& parameter : parameters)
            prettyPrintParameterNode(parameter.get());

        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintReturnTypesNode(ReturnTypesNode* node)
    {
        const auto& returnTypes = node->returnTypes();
        stream() << indentation() << stringify(node->kind()) << QString("(%1): {").arg(returnTypes.size()) << newLine();
        pushIndentation();

        for (const auto& returnType : returnTypes)
            prettyPrintTypeNameNode(returnType.get());

        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintTypeNameNode(TypeNameNode* node)
    {
        stream() << indentation() << stringify(node->kind()) << QString(": {") << newLine();
        pushIndentation();

        const auto type = node->type();
        const auto typeName = TypeDatabase::TryFindName(type);

        stream() << indentation() << QString("Type: %1").arg(typeName) << newLine();
        if (node->isReference())
        {
            stream() << indentation() << QString("Ref: true") << newLine();
        }
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintBlockNode(BlockNode* block)
    {
        const auto& statements = block->statements();
        stream() << indentation() << stringify(block->kind()) << QString("(%1): {").arg(statements.size()) << newLine();
        pushIndentation();

        for (const auto& statement : block->statements())
            prettyPrintNode(statement.get());

        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintGroupingExpression(GroupingExpression* grouping)
    {
        stream() << indentation() << stringify(grouping->kind()) << QString(": {") << newLine();
        pushIndentation();
        prettyPrintNode(grouping->expression().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }
}

//void ParseTreePrinter::PrettyPrintTypeDefinitionStatement(TypeDefinitionStatement* statement)
//{
//    auto nameToken = statement->name();
//    auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
//
//    stream() << Indentation() << stringify(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//    stream() << Indentation() << stringify(NodeKind::NameExpression) << QString(": %1").arg(nameLexeme) << NewLine();
//    PrettyPrintBlockNode(statement->body());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintFieldDefinitionStatement(FieldDefinitionStatement* statement)
//{
//    stream() << Indentation() << stringify(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << QString("Field: {") << NewLine();
//    PushIndentation();
//    PrettyPrintNode(statement->name());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    if (statement->type().has_value())
//        PrettyPrintTypeName(statement->type().value());
//
//    if (statement->expression().has_value())
//    {
//        stream() << Indentation() << QString("Value: {") << NewLine();
//        PushIndentation();
//        PrettyPrintNode(statement->expression().value());
//        PopIndentation();
//        stream() << Indentation() << QString("}") << NewLine();
//    }
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}

//void ParseTreePrinter::PrettyPrintWhileStatement(WhileStatement* statement)
//{
//    stream() << Indentation() << stringify(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << QString("Condition: {") << NewLine();
//    PushIndentation();
//    PrettyPrintNode(statement->condition());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PrettyPrintBlockNode(statement->body());
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}

//void ParseTreePrinter::PrettyPrintParameterNode(ParameterNode* parameter)
//{
//    stream() << Indentation() << stringify(parameter->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    PrettyPrintNameExpression(parameter->name());
//    PrettyPrintTypeName(parameter->type());
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}

//void ParseTreePrinter::PrettyPrintFunctionCallExpression(FunctionCallExpression* functionCall)
//{
//    auto nameToken = functionCall->name();
//    auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
//    stream() << Indentation() << stringify(functionCall->kind()) << QString(": {") << NewLine();
//
//    PushIndentation();
//    stream() << Indentation() << stringify(NodeKind::NameExpression) << QString(": %1").arg(nameLexeme) << NewLine();
//    PrettyPrintArgumentsNode(functionCall->arguments());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}

//
//void ParseTreePrinter::PrettyPrintMemberAccessExpression(MemberAccessExpression* memberAccess)
//{
//    stream() << Indentation() << stringify(memberAccess->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    PrettyPrintNode(memberAccess->expression());
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintError(Error* error)
//{
//    stream() << Indentation() << QString("Error!!") << NewLine();
//}
