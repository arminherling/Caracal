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
            case NodeKind::CppBlockStatement:
            {
                prettyPrintCppBlockStatement((CppBlockStatement*)node);
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

        stream() << indentation() << QString("Right: {") << newLine();
        pushIndentation();
        prettyPrintNode(statement->rightExpression().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
     
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

    void ParseTreePrinter::prettyPrintFunctionDefinitionStatement(FunctionDefinitionStatement* statement)
    {
        const auto& nameToken = statement->name();
        const auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);

        stream() << indentation() << stringify(statement->kind()) << QString(": {") << newLine();
        pushIndentation();
        stream() << indentation() << QString("Name: %1").arg(nameLexeme) << newLine();
        prettyPrintParametersNode(statement->parameters().get());
        prettyPrintReturnTypesNode(statement->returnTypes().get());
        prettyPrintBlockNode(statement->body().get());
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
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
        stream() << indentation() << QString("Identifier: %1").arg(identifierLexeme) << newLine();
        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintDiscardLiteral(DiscardLiteral* discard)
    {
        stream() << indentation() << QString("Discard: _") << newLine();
    }

    void ParseTreePrinter::prettyPrintBoolLiteral(BoolLiteral* node)
    {
        const auto lexeme = m_parseTree.tokens().getLexeme(node->token());
        stream() << indentation() << stringify(node->kind()) << QString(": %1").arg(lexeme) << newLine();
    }

    void ParseTreePrinter::prettyPrintNumberLiteral(NumberLiteral* number)
    {
        const auto lexeme = m_parseTree.tokens().getLexeme(number->token());
        stream() << indentation() << stringify(number->kind()) << QString(": %1").arg(lexeme) << newLine();
    }

    void ParseTreePrinter::prettyPrintStringLiteral(StringLiteral* string)
    {
        const auto lexeme = m_parseTree.tokens().getLexeme(string->token());
        stream() << indentation() << stringify(string->kind()) << QString(": %1").arg(lexeme) << newLine();
    }

    void ParseTreePrinter::prettyPrintParametersNode(ParametersNode* node)
    {
        const auto& parameters = node->parameters();
        stream() << indentation() << stringify(node->kind()) << QString("(%1): {").arg(parameters.size()) << newLine();
        pushIndentation();

        for (const auto& parameter : parameters)
            prettyPrintNode(parameter.get());

        popIndentation();
        stream() << indentation() << QString("}") << newLine();
    }

    void ParseTreePrinter::prettyPrintReturnTypesNode(ReturnTypesNode* node)
    {
        const auto& returnTypes = node->returnTypes();
        stream() << indentation() << stringify(node->kind()) << QString("(%1): {").arg(returnTypes.size()) << newLine();
        pushIndentation();

        for (const auto& returnType : returnTypes)
            prettyPrintNode(returnType.get());

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

//void ParseTreePrinter::PrettyPrintExpressionStatement(ExpressionStatement* statement)
//{
//    stream() << Indentation() << stringify(statement->kind()) << QString(": {") << NewLine();
//
//    PushIndentation();
//    PrettyPrintNode(statement->expression());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintEnumDefinitionStatement(EnumDefinitionStatement* statement)
//{
//    auto nameToken = statement->name();
//    auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
//
//    stream() << Indentation() << stringify(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//    stream() << Indentation() << stringify(NodeKind::NameExpression) << QString(": %1").arg(nameLexeme) << NewLine();
//
//    if (statement->baseType().has_value())
//        PrettyPrintTypeName(statement->baseType().value());
//
//    const auto& fieldDefinitions = statement->fieldDefinitions();
//    stream() << Indentation() << QString("FieldDefinitions(%1): {").arg(fieldDefinitions.size()) << NewLine();
//    PushIndentation();
//
//    for (const auto& fieldDefinition : fieldDefinitions)
//        PrettyPrintNode(fieldDefinition);
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintEnumFieldDefinitionStatement(EnumFieldDefinitionStatement* statement)
//{
//    auto nameToken = statement->name()->identifier();
//    auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
//
//    stream() << Indentation() << stringify(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//    stream() << Indentation() << stringify(NodeKind::NameExpression) << QString(": %1").arg(nameLexeme) << NewLine();
//
//    if (statement->value().has_value())
//    {
//        PrettyPrintNode(statement->value().value());
//    }
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
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
//
//void ParseTreePrinter::PrettyPrintMethodDefinitionStatement(MethodDefinitionStatement* statement)
//{
//    auto nameToken = statement->name();
//    auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
//
//    stream() << Indentation() << stringify(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//    stream() << Indentation() << stringify(NodeKind::NameExpression) << QString(": %1").arg(nameLexeme) << NewLine();
//    PrettyPrintParametersNode(statement->parameters());
//    PrettyPrintBlockNode(statement->body());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintIfStatement(IfStatement* statement)
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
//
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

//
//void ParseTreePrinter::PrettyPrintArgumentsNode(ArgumentsNode* node)
//{
//    const auto& arguments = node->arguments();
//    stream() << Indentation() << stringify(node->kind()) << QString("(%1): {").arg(arguments.size()) << NewLine();
//    PushIndentation();
//
//    for (const auto& argument : arguments)
//        PrettyPrintNode(argument);
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
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
//void ParseTreePrinter::PrettyPrintTypeName(const TypeName& type)
//{
//    auto token = type.name()->identifier();
//    auto lexeme = m_parseTree.tokens().getLexeme(token);
//    if (type.isReference())
//        stream() << Indentation() << QString("Ref: true") << NewLine();
//
//    stream() << Indentation() << stringify(NodeKind::TypeName) << QString(": ") << lexeme << NewLine();
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
