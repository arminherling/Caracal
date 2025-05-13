#include <Debug/ParseTreePrinter.h>

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
            case NodeKind::CppBlockStatement:
            {
                prettyPrintCppBlockStatement((CppBlockStatement*)node);
                break;
            }
            //case NodeKind::AssignmentStatement:
            //{
            //    PrettyPrintAssignmentStatement((AssignmentStatement*)node);
            //    break;
            //}
            //case NodeKind::ExpressionStatement:
            //{
            //    PrettyPrintExpressionStatement((ExpressionStatement*)node);
            //    break;
            //}
            //case NodeKind::FunctionDefinitionStatement:
            //{
            //    PrettyPrintFunctionDefinitionStatement((FunctionDefinitionStatement*)node);
            //    break;
            //}
            //case NodeKind::EnumDefinitionStatement:
            //{
            //    PrettyPrintEnumDefinitionStatement((EnumDefinitionStatement*)node);
            //    break;
            //}
            //case NodeKind::EnumFieldDefinitionStatement:
            //{
            //    PrettyPrintEnumFieldDefinitionStatement((EnumFieldDefinitionStatement*)node);
            //    break;
            //}
            //case NodeKind::TypeDefinitionStatement:
            //{
            //    PrettyPrintTypeDefinitionStatement((TypeDefinitionStatement*)node);
            //    break;
            //}
            //case NodeKind::FieldDefinitionStatement:
            //{
            //    PrettyPrintFieldDefinitionStatement((FieldDefinitionStatement*)node);
            //    break;
            //}
            //case NodeKind::MethodDefinitionStatement:
            //{
            //    PrettyPrintMethodDefinitionStatement((MethodDefinitionStatement*)node);
            //    break;
            //}
            //case NodeKind::IfStatement:
            //{
            //    PrettyPrintIfStatement((IfStatement*)node);
            //    break;
            //}
            //case NodeKind::WhileStatement:
            //{
            //    PrettyPrintWhileStatement((WhileStatement*)node);
            //    break;
            //}
            //case NodeKind::ReturnStatement:
            //{
            //    PrettyPrintReturnStatement((ReturnStatement*)node);
            //    break;
            //}
            //case NodeKind::ParameterNode:
            //{
            //    PrettyPrintParameterNode((ParameterNode*)node);
            //    break;
            //}
            //case NodeKind::DiscardLiteral:
            //{
            //    PrettyPrintDiscardLiteral((DiscardLiteral*)node);
            //    break;
            //}
            //case NodeKind::FunctionCallExpression:
            //{
            //    PrettyPrintFunctionCallExpression((FunctionCallExpression*)node);
            //    break;
            //}
            //case NodeKind::BoolLiteral:
            //{
            //    PrettyPrintBoolLiteral((BoolLiteral*)node);
            //    break;
            //}
            //case NodeKind::NameExpression:
            //{
            //    PrettyPrintNameExpression((NameExpression*)node);
            //    break;
            //}
            //case NodeKind::NumberLiteral:
            //{
            //    PrettyPrintNumberLiteral((NumberLiteral*)node);
            //    break;
            //}
            //case NodeKind::GroupingExpression:
            //{
            //    PrettyPrintGroupingExpression((GroupingExpression*)node);
            //    break;
            //}
            //case NodeKind::UnaryExpression:
            //{
            //    PrettyPrintUnaryExpression((UnaryExpression*)node);
            //    break;
            //}
            //case NodeKind::BinaryExpression:
            //{
            //    PrettyPrintBinaryExpression((BinaryExpression*)node);
            //    break;
            //}
            //case NodeKind::MemberAccessExpression:
            //{
            //    PrettyPrintMemberAccessExpression((MemberAccessExpression*)node);
            //    break;
            //}
            //case NodeKind::Error:
            //{
            //    PrettyPrintError((Error*)node);
            //    break;
            //}
            default:
                stream() << indentation() << QString("Missing NodeKind!!") << newLine();
                break;
        }
    }

    void ParseTreePrinter::prettyPrintCppBlockStatement(CppBlockStatement* node)
    {
        const auto& lines = node->lines();

        stream() << indentation() << StringifyNodeKind(node->kind()) << QString(": {") << newLine();
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
}
//void ParseTreePrinter::PrettyPrintAssignmentStatement(AssignmentStatement* statement)
//{
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << QString("Left: {") << NewLine();
//    PushIndentation();
//    PrettyPrintNode(statement->leftExpression());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    stream() << Indentation() << QString("Right: {") << NewLine();
//    PushIndentation();
//    PrettyPrintNode(statement->rightExpression());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintExpressionStatement(ExpressionStatement* statement)
//{
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//
//    PushIndentation();
//    PrettyPrintNode(statement->expression());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintFunctionDefinitionStatement(FunctionDefinitionStatement* statement)
//{
//    auto nameToken = statement->name();
//    auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
//
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//    stream() << Indentation() << StringifyNodeKind(NodeKind::NameExpression) << QString(": %1").arg(nameLexeme) << NewLine();
//    PrettyPrintParametersNode(statement->parameters());
//    PrettyPrintBlockNode(statement->body());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintEnumDefinitionStatement(EnumDefinitionStatement* statement)
//{
//    auto nameToken = statement->name();
//    auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
//
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//    stream() << Indentation() << StringifyNodeKind(NodeKind::NameExpression) << QString(": %1").arg(nameLexeme) << NewLine();
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
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//    stream() << Indentation() << StringifyNodeKind(NodeKind::NameExpression) << QString(": %1").arg(nameLexeme) << NewLine();
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
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//    stream() << Indentation() << StringifyNodeKind(NodeKind::NameExpression) << QString(": %1").arg(nameLexeme) << NewLine();
//    PrettyPrintBlockNode(statement->body());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintFieldDefinitionStatement(FieldDefinitionStatement* statement)
//{
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
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
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//    stream() << Indentation() << StringifyNodeKind(NodeKind::NameExpression) << QString(": %1").arg(nameLexeme) << NewLine();
//    PrettyPrintParametersNode(statement->parameters());
//    PrettyPrintBlockNode(statement->body());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintIfStatement(IfStatement* statement)
//{
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
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
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
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
//void ParseTreePrinter::PrettyPrintReturnStatement(ReturnStatement* statement)
//{
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    if (statement->expression().has_value())
//        PrettyPrintNode(statement->expression().value());
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintArgumentsNode(ArgumentsNode* node)
//{
//    const auto& arguments = node->arguments();
//    stream() << Indentation() << StringifyNodeKind(node->kind()) << QString("(%1): {").arg(arguments.size()) << NewLine();
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
//    stream() << Indentation() << StringifyNodeKind(parameter->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    PrettyPrintNameExpression(parameter->name());
//    PrettyPrintTypeName(parameter->type());
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintParametersNode(ParametersNode* node)
//{
//    const auto& parameters = node->parameters();
//    stream() << Indentation() << StringifyNodeKind(node->kind()) << QString("(%1): {").arg(parameters.size()) << NewLine();
//    PushIndentation();
//
//    for (const auto& parameter : parameters)
//        PrettyPrintNode(parameter);
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintBlockNode(BlockNode* block)
//{
//    const auto& statements = block->statements();
//    stream() << Indentation() << StringifyNodeKind(block->kind()) << QString("(%1): {").arg(statements.size()) << NewLine();
//    PushIndentation();
//
//    for (const auto& statement : block->statements())
//        PrettyPrintNode(statement);
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintDiscardLiteral(DiscardLiteral* discard)
//{
//    stream() << Indentation() << StringifyNodeKind(discard->kind()) << ": _" << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintFunctionCallExpression(FunctionCallExpression* functionCall)
//{
//    auto nameToken = functionCall->name();
//    auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
//    stream() << Indentation() << StringifyNodeKind(functionCall->kind()) << QString(": {") << NewLine();
//
//    PushIndentation();
//    stream() << Indentation() << StringifyNodeKind(NodeKind::NameExpression) << QString(": %1").arg(nameLexeme) << NewLine();
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
//    stream() << Indentation() << StringifyNodeKind(NodeKind::TypeName) << QString(": ") << lexeme << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintBoolLiteral(BoolLiteral* node)
//{
//    auto value = node->value() ? QString("true") : QString("false");
//    stream() << Indentation() << StringifyNodeKind(NodeKind::BoolLiteral) << QString(": ") << value << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintNameExpression(NameExpression* name)
//{
//    auto token = name->identifier();
//    auto lexeme = m_parseTree.tokens().getLexeme(token);
//    stream() << Indentation() << StringifyNodeKind(name->kind()) << QString(": %1").arg(lexeme) << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintNumberLiteral(NumberLiteral* number)
//{
//    auto token = number->token();
//    auto lexeme = m_parseTree.tokens().getLexeme(token);
//    stream() << Indentation() << StringifyNodeKind(number->kind()) << QString(": %1").arg(lexeme) << NewLine();
//
//    auto optionalType = number->type();
//    if (!optionalType.has_value())
//        return;
//
//    PrettyPrintTypeName(optionalType.value());
//}
//
//void ParseTreePrinter::PrettyPrintGroupingExpression(GroupingExpression* grouping)
//{
//    stream() << Indentation() << StringifyNodeKind(grouping->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    PrettyPrintNode(grouping->expression());
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintUnaryExpression(UnaryExpression* unaryExpression)
//{
//    stream() << Indentation() << StringifyNodeKind(unaryExpression->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << QString("Operation: ") << StringifyUnaryOperation(unaryExpression->unaryOperator()) << NewLine();
//
//    stream() << Indentation() << QString("Expression: {") << NewLine();
//    PushIndentation();
//    PrettyPrintNode(unaryExpression->expression());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintBinaryExpression(BinaryExpression* binaryExpression)
//{
//    stream() << Indentation() << StringifyNodeKind(binaryExpression->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << QString("Left: {") << NewLine();
//    PushIndentation();
//    PrettyPrintNode(binaryExpression->leftExpression());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    stream() << Indentation() << QString("Operation: ") << StringifyBinaryOperation(binaryExpression->binaryOperator()) << NewLine();
//
//    stream() << Indentation() << QString("Right: {") << NewLine();
//    PushIndentation();
//    PrettyPrintNode(binaryExpression->rightExpression());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void ParseTreePrinter::PrettyPrintMemberAccessExpression(MemberAccessExpression* memberAccess)
//{
//    stream() << Indentation() << StringifyNodeKind(memberAccess->kind()) << QString(": {") << NewLine();
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
