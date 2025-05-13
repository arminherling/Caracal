//#include <Debug/TypedTreePrinter.h>
//
//TypedTreePrinter::TypedTreePrinter(
//    TypedTree& typedTree,
//    const TypeDatabase& typeDatabase,
//    i32 indentation)
//    : BasePrinter(indentation)
//    , m_typedTree{ typedTree }
//    , m_typeDatabase{ typeDatabase }
//{
//}
//
//QString TypedTreePrinter::PrettyPrint()
//{
//    for (const auto& globalStatement : m_typedTree.globalStatements())
//    {
//        PrettyPrintNode(globalStatement);
//    }
//
//    return toUtf8();
//}
//
//void TypedTreePrinter::PrettyPrintNode(TypedNode* node)
//{
//    if (node == nullptr)
//    {
//        stream() << Indentation() << QString("null") << NewLine();
//        return;
//    }
//
//    switch (node->kind())
//    {
//        case NodeKind::TypedAssignmentStatement:
//        {
//            PrettyPrintTypedAssignmentStatement((TypedAssignmentStatement*)node);
//            break;
//        }
//        case NodeKind::TypedExpressionStatement:
//        {
//            PrettyPrintTypedExpressionStatement((TypedExpressionStatement*)node);
//            break;
//        }
//        case NodeKind::TypedEnumDefinitionStatement:
//        {
//            PrettyPrintTypedEnumDefinitionStatement((TypedEnumDefinitionStatement*)node);
//            break;
//        }
//        case NodeKind::TypedTypeDefinitionStatement:
//        {
//            PrettyPrintTypedTypeDefinitionStatement((TypedTypeDefinitionStatement*)node);
//            break;
//        }
//        case NodeKind::TypedFunctionDefinitionStatement:
//        {
//            PrettyPrintTypedFunctionDefinitionStatement((TypedFunctionDefinitionStatement*)node);
//            break;
//        }
//        case NodeKind::TypedIfStatement:
//        {
//            PrettyPrintTypedIfStatement((TypedIfStatement*)node);
//            break;
//        }
//        case NodeKind::TypedWhileStatement:
//        {
//            PrettyPrintTypedWhileStatement((TypedWhileStatement*)node);
//            break;
//        }
//        case NodeKind::TypedReturnStatement:
//        {
//            PrettyPrintTypedReturnStatement((TypedReturnStatement*)node);
//            break;
//        }
//        case NodeKind::TypedReferenceOfExpression:
//        {
//            PrettyPrintTypedReferenceOfExpression((TypedReferenceOfExpression*)node);
//            break;
//        }
//        case NodeKind::TypedNegationExpression:
//        {
//            PrettyPrintTypedNegationExpression((TypedNegationExpression*)node);
//            break;
//        }
//        case NodeKind::TypedEnumValueAccessExpression:
//        {
//            PrettyPrintTypedEnumValueAccessExpression((TypedEnumValueAccessExpression*)node);
//            break;
//        }
//        case NodeKind::TypedFieldAccessExpression:
//        {
//            PrettyPrintTypedFieldAccessExpression((TypedFieldAccessExpression*)node);
//            break;
//        }
//        case NodeKind::TypedAdditionExpression:
//        case NodeKind::TypedSubtractionExpression:
//        case NodeKind::TypedMultiplicationExpression:
//        case NodeKind::TypedDivisionExpression:
//        {
//            PrettyPrintTypedBinaryExpression((TypedBinaryExpression*)node);
//            break;
//        }
//        case NodeKind::TypedFunctionCallExpression:
//        {
//            PrettyPrintTypedFunctionCallExpression((TypedFunctionCallExpression*)node);
//            break;
//        }
//        case NodeKind::TypedMethodCallExpression:
//        {
//            PrettyPrintTypedMethodCallExpression((TypedMethodCallExpression*)node);
//            break;
//        }
//        case NodeKind::TypedConstant:
//        {
//            PrettyPrintTypedConstant((TypedConstant*)node);
//            break;
//        }
//        case NodeKind::TypedVariable:
//        {
//            PrettyPrintTypedVariable((TypedVariable*)node);
//            break;
//        }
//        case NodeKind::Parameter:
//        {
//            PrettyPrintParameter((Parameter*)node);
//            break;
//        }
//        case NodeKind::Discard:
//        {
//            PrettyPrintDiscard((Discard*)node);
//            break;
//        }
//        case NodeKind::BoolValue:
//        {
//            PrettyPrintBoolValue((BoolValue*)node);
//            break;
//        }
//        case NodeKind::U8Value:
//        {
//            PrettyPrintU8Value((U8Value*)node);
//            break;
//        }
//        case NodeKind::I32Value:
//        {
//            PrettyPrintI32Value((I32Value*)node);
//            break;
//        }
//        default:
//            stream() << Indentation() << QString("Missing NodeKind!!") << NewLine();
//            break;
//    }
//}
//
//void TypedTreePrinter::PrettyPrintTypedAssignmentStatement(TypedAssignmentStatement* statement)
//{
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(statement->type()) << NewLine();
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
//void TypedTreePrinter::PrettyPrintTypedExpressionStatement(TypedExpressionStatement* statement)
//{
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(statement->type()) << NewLine();
//
//    stream() << Indentation() << QString("Expression: {") << NewLine();
//    PushIndentation();
//    PrettyPrintNode(statement->expression());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedEnumDefinitionStatement(TypedEnumDefinitionStatement* statement)
//{
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << QString("TypeKind: enum") << NewLine();
//    stream() << Indentation() << QString("TypeName: ") << statement->name() << NewLine();
//
//    auto& baseTypeDefinition = m_typeDatabase.getTypeDefinition(statement->baseType());
//    stream() << Indentation() << QString("BaseType: ") << baseTypeDefinition.name() << NewLine();
//
//    auto fields = statement->fields();
//    stream() << Indentation() << QString("Fields(%1): {").arg(fields.count()) << NewLine();
//    PushIndentation();
//    for (const auto& field : fields)
//    {
//        PrettyPrintTypedFieldDefinitionNode(field);
//    }
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedTypeDefinitionStatement(TypedTypeDefinitionStatement* statement)
//{
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << QString("TypeKind: type") << NewLine();
//    stream() << Indentation() << QString("TypeName: ") << statement->name() << NewLine();
//
//    auto fields = statement->fields();
//    stream() << Indentation() << QString("Fields(%1): {").arg(fields.count()) << NewLine();
//    PushIndentation();
//    for (const auto& field : fields)
//    {
//        PrettyPrintTypedFieldDefinitionNode(field);
//    }
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    auto methods = statement->methods();
//    stream() << Indentation() << QString("Methods(%1): {").arg(methods.count()) << NewLine();
//    PushIndentation();
//    for (const auto& method : methods)
//    {
//        PrettyPrintTypedMethodDefinitionStatement(method);
//    }
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedFunctionDefinitionStatement(TypedFunctionDefinitionStatement* statement)
//{
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << QString("TypeKind: function") << NewLine();
//    stream() << Indentation() << PrettyPrintType(statement->type()) << NewLine();
//    stream() << Indentation() << QString("Name: ") << statement->name() << NewLine();
//
//    auto& parameters = statement->parameters();
//    stream() << Indentation() << QString("Parameters(%1): {").arg(parameters.count()) << NewLine();
//    PushIndentation();
//    for (const auto parameter : parameters)
//    {
//        PrettyPrintParameter(parameter);
//    }
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    auto returnType = statement->returnType();
//    stream() << Indentation() << QString("ReturnType: {") << NewLine();
//    PushIndentation();
//    stream() << Indentation() << PrettyPrintType(returnType) << NewLine();
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    auto& body = statement->body();
//    stream() << Indentation() << QString("Body(%1): {").arg(body.count()) << NewLine();
//    PushIndentation();
//    for (const auto bodyStatement : body)
//    {
//        PrettyPrintNode(bodyStatement);
//    }
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedMethodDefinitionStatement(TypedMethodDefinitionStatement* statement)
//{
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << QString("TypeKind: method") << NewLine();
//    stream() << Indentation() << PrettyPrintType(statement->type(), statement->parentType()) << NewLine();
//    stream() << Indentation() << QString("Name: ") << statement->name() << NewLine();
//
//    auto& parameters = statement->parameters();
//    stream() << Indentation() << QString("Parameters(%1): {").arg(parameters.count()) << NewLine();
//    PushIndentation();
//    for (const auto parameter : parameters)
//    {
//        PrettyPrintParameter(parameter);
//    }
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    auto returnType = statement->returnType();
//    stream() << Indentation() << QString("ReturnType: {") << NewLine();
//    PushIndentation();
//    stream() << Indentation() << PrettyPrintType(returnType) << NewLine();
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    auto& body = statement->body();
//    stream() << Indentation() << QString("Body(%1): {").arg(body.count()) << NewLine();
//    PushIndentation();
//    for (const auto bodyStatement : body)
//    {
//        PrettyPrintNode(bodyStatement);
//    }
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedIfStatement(TypedIfStatement* statement)
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
//    auto& body = statement->body();
//    stream() << Indentation() << QString("Body(%1): {").arg(body.count()) << NewLine();
//    PushIndentation();
//    for (const auto bodyStatement : body)
//    {
//        PrettyPrintNode(bodyStatement);
//    }
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    stream() << Indentation() << QString("ElseBlock: {") << NewLine();
//    PushIndentation();
//    auto optionalElseBlock = statement->elseBlock();
//    if(optionalElseBlock.has_value())
//        PrettyPrintNode(optionalElseBlock.value());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedWhileStatement(TypedWhileStatement* statement)
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
//    auto& body = statement->body();
//    stream() << Indentation() << QString("Body(%1): {").arg(body.count()) << NewLine();
//    PushIndentation();
//    for (const auto bodyStatement : body)
//    {
//        PrettyPrintNode(bodyStatement);
//    }
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedReturnStatement(TypedReturnStatement* statement)
//{
//    stream() << Indentation() << StringifyNodeKind(statement->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(statement->type()) << NewLine();
//    stream() << Indentation() << QString("Expression: {") << NewLine();
//    PushIndentation();
//
//    auto optionalExpression = statement->expression();
//    if(optionalExpression.has_value())
//    PrettyPrintNode(optionalExpression.value());
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedReferenceOfExpression(TypedReferenceOfExpression* expression)
//{
//    stream() << Indentation() << StringifyNodeKind(expression->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(expression->type()) << NewLine();
//    stream() << Indentation() << QString("Expression: {") << NewLine();
//    PushIndentation();
//
//    PrettyPrintNode(expression->expression());
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedNegationExpression(TypedNegationExpression* expression)
//{
//    stream() << Indentation() << StringifyNodeKind(expression->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(expression->type()) << NewLine();
//    stream() << Indentation() << QString("Expression: {") << NewLine();
//    PushIndentation();
//
//    PrettyPrintNode(expression->expression());
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedEnumValueAccessExpression(TypedEnumValueAccessExpression* expression)
//{
//    stream() << Indentation() << StringifyNodeKind(expression->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(expression->type()) << NewLine();
//    stream() << Indentation() << QString("Name: ") << expression->fieldName() << NewLine();
//    stream() << Indentation() << QString("Expression: {") << NewLine();
//    PushIndentation();
//    PrettyPrintNode(expression->field()->value());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedFieldAccessExpression(TypedFieldAccessExpression* expression)
//{
//    stream() << Indentation() << StringifyNodeKind(expression->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(expression->type()) << NewLine();
//    stream() << Indentation() << QString("ThisType: ") << PrettyPrintTypeName(expression->thisType()) << NewLine();
//    stream() << Indentation() << QString("Name: ") << expression->fieldName() << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedBinaryExpression(TypedBinaryExpression* expression)
//{
//    stream() << Indentation() << StringifyNodeKind(expression->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(expression->type()) << NewLine();
//
//    stream() << Indentation() << QString("Left: {") << NewLine();
//    PushIndentation();
//    PrettyPrintNode(expression->leftExpression());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    stream() << Indentation() << QString("Right: {") << NewLine();
//    PushIndentation();
//    PrettyPrintNode(expression->rightExpression());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedConstant(TypedConstant* constant)
//{
//    stream() << Indentation() << StringifyNodeKind(constant->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(constant->type()) << NewLine();
//    stream() << Indentation() << QString("Name: ") << constant->name() << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedVariable(TypedVariable* variable)
//{
//    stream() << Indentation() << StringifyNodeKind(variable->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(variable->type()) << NewLine();
//    stream() << Indentation() << QString("Name: ") << variable->name() << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintParameter(Parameter* parameter)
//{
//    stream() << Indentation() << StringifyNodeKind(parameter->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(parameter->type()) << NewLine();
//    stream() << Indentation() << QString("Name: ") << parameter->name() << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedFunctionCallExpression(TypedFunctionCallExpression* functionCall)
//{
//    stream() << Indentation() << StringifyNodeKind(functionCall->kind()) << QString(": {") << NewLine();
//
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(functionCall->type()) << NewLine();
//    stream() << Indentation() << QString("Name: ") << functionCall->name() << NewLine();
//
//    auto arguments = functionCall->arguments();
//    stream() << Indentation() << QString("Arguments(%1): {").arg(arguments.count()) << NewLine();
//    PushIndentation();
//    for (const auto argument : arguments)
//    {
//        PrettyPrintNode(argument);
//    }
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedMethodCallExpression(TypedMethodCallExpression* methodCall)
//{
//    stream() << Indentation() << StringifyNodeKind(methodCall->kind()) << QString(": {") << NewLine();
//
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(methodCall->type()) << NewLine();
//    stream() << Indentation() << QString("ThisType: ") << PrettyPrintTypeName(methodCall->thisType()) << NewLine();
//    stream() << Indentation() << QString("Name: ") << methodCall->name() << NewLine();
//
//    auto arguments = methodCall->arguments();
//    stream() << Indentation() << QString("Arguments(%1): {").arg(arguments.count()) << NewLine();
//    PushIndentation();
//    for (const auto argument : arguments)
//    {
//        PrettyPrintNode(argument);
//    }
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintDiscard(Discard* discard)
//{
//    stream() << Indentation() << StringifyNodeKind(discard->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(discard->type()) << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintBoolValue(BoolValue* value)
//{
//    stream() << Indentation() << StringifyNodeKind(value->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(value->type()) << NewLine();
//    auto v = (value->value() ? QString("true") : QString("false"));
//    stream() << Indentation() << QString("Value: ") << v << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintU8Value(U8Value* value)
//{
//    stream() << Indentation() << StringifyNodeKind(value->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(value->type()) << NewLine();
//    stream() << Indentation() << QString("Value: ") << value->value() << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintI32Value(I32Value* value)
//{
//    stream() << Indentation() << StringifyNodeKind(value->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//
//    stream() << Indentation() << PrettyPrintType(value->type()) << NewLine();
//    stream() << Indentation() << QString("Value: ") << value->value() << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedFieldDefinitionNode(TypedFieldDefinitionNode* field)
//{
//    stream() << Indentation() << StringifyNodeKind(field->kind()) << QString(": {") << NewLine();
//    PushIndentation();
//    stream() << Indentation() << PrettyPrintType(field->valueType()) << NewLine();
//    stream() << Indentation() << QString("Name: ") << field->name() << NewLine();
//
//    stream() << Indentation() << QString("Expression: {") << NewLine();
//    PushIndentation();
//    PrettyPrintNode(field->value());
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//void TypedTreePrinter::PrettyPrintTypedArgumentsNode()
//{
//    // TODO rewrite whole method once we type check the arguments
//    stream() << Indentation() << StringifyNodeKind(NodeKind::ArgumentsNode) << QString("(0): {") << NewLine();
//    PushIndentation();
//    PopIndentation();
//    stream() << Indentation() << QString("}") << NewLine();
//}
//
//QString TypedTreePrinter::PrettyPrintType(Type type, Type thisType) noexcept
//{
//    if (type.kind() == TypeKind::Function)
//    {
//        QList<Parameter*> parameters;
//        auto returnType = Type::Undefined();
//        if (thisType == Type::Undefined())
//        {
//            auto& functionDefinition = m_typeDatabase.getFunctionDefinition(type);
//            parameters = functionDefinition.parameters();
//            returnType = functionDefinition.returnType();
//        }
//        else
//        {
//            auto& typeDefinition = m_typeDatabase.getTypeDefinition(thisType);
//            auto& methodDefinition = typeDefinition.getFunctionDefinition(type);
//            parameters = methodDefinition.parameters();
//            returnType = methodDefinition.returnType();
//        }
//
//        QStringList stringifiedParameterTypes;
//        for (const auto& parameter: parameters)
//        {
//            stringifiedParameterTypes.append(
//                PrettyPrintTypeName(parameter->type()));
//        }
//
//        QString returnTypeName;
//        if (returnType != Type::Void())
//        {
//            returnTypeName = PrettyPrintTypeName(returnType);
//        }
//
//        return QString("Type: (%1)->(%2)")
//            .arg(
//                stringifiedParameterTypes.join(QString(", ")),
//                returnTypeName);
//    }
//
//    return QString("Type: %1").arg(PrettyPrintTypeName(type));
//}
//
//QString TypedTreePrinter::PrettyPrintTypeName(Type type) noexcept
//{
//    if (type.kind() == TypeKind::Type || type.kind() == TypeKind::Builtin)
//    {
//        auto& typeDefinition = m_typeDatabase.getTypeDefinition(type);
//        return typeDefinition.name();
//    }
//    else if (type.kind() == TypeKind::Enum)
//    {
//        auto& enumDefinition = m_typeDatabase.getEnumDefinition(type);
//        return enumDefinition.name();
//    }
//
//    return QString("???");
//}
