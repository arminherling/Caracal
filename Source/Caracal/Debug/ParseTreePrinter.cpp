#include "ParseTreePrinter.h"
#include <Caracal/Semantic/SemanticContext.h>

namespace Caracal
{
    ParseTreePrinter::ParseTreePrinter(
        const ParseTree& parseTree, 
        SemanticContext* module, 
        i32 indentation)
        : m_parseTree{ parseTree }
        , m_module{ module }
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
                prettyPrintConstantDeclaration(static_cast<ConstantDeclaration*>(node));
                break;
            }
            case NodeKind::VariableDeclaration:
            {
                prettyPrintVariableDeclaration(static_cast<VariableDeclaration*>(node));
                break;
            }
            case NodeKind::TypeFieldDeclaration:
            {
                prettyPrintTypeFieldDeclaration(static_cast<TypeFieldDeclaration*>(node));
                break;
            }
            case NodeKind::ExpressionStatement:
            {
                prettyPrintExpressionStatement(static_cast<ExpressionStatement*>(node));
                break;
            }
            case NodeKind::AssignmentStatement:
            {
                prettyPrintAssignmentStatement(static_cast<AssignmentStatement*>(node));
                break;
            }
            case NodeKind::FunctionDefinitionStatement:
            {
                prettyPrintFunctionDefinitionStatement(static_cast<FunctionDefinitionStatement*>(node));
                break;
            }
            case NodeKind::EnumDefinitionStatement:
            {
                prettyPrintEnumDefinitionStatement(static_cast<EnumDefinitionStatement*>(node));
                break;
            }
            case NodeKind::TypeDefinitionStatement:
            {
                prettyPrintTypeDefinitionStatement(static_cast<TypeDefinitionStatement*>(node));
                break;
            }
            case NodeKind::MethodDefinitionStatement:
            {
                prettyPrintMethodDefinitionStatement(static_cast<MethodDefinitionStatement*>(node));
                break;
            }
            case NodeKind::IfStatement:
            {
                prettyPrintIfStatement(static_cast<IfStatement*>(node));
                break;
            }
            case NodeKind::WhileStatement:
            {
                prettyPrintWhileStatement(static_cast<WhileStatement*>(node));
                break;
            }
            case NodeKind::BreakStatement:
            {
                prettyPrintBreakStatement(static_cast<BreakStatement*>(node));
                break;
            }
            case NodeKind::SkipStatement:
            {
                prettyPrintSkipStatement(static_cast<SkipStatement*>(node));
                break;
            }
            case NodeKind::ReturnStatement:
            {
                prettyPrintReturnStatement(static_cast<ReturnStatement*>(node));
                break;
            }
            case NodeKind::GroupingExpression:
            {
                prettyPrintGroupingExpression(static_cast<GroupingExpression*>(node));
                break;
            }
            case NodeKind::UnaryExpression:
            {
                prettyPrintUnaryExpression(static_cast<UnaryExpression*>(node));
                break;
            }
            case NodeKind::BinaryExpression:
            {
                prettyPrintBinaryExpression(static_cast<BinaryExpression*>(node));
                break;
            }
            case NodeKind::NameExpression:
            {
                prettyPrintNameExpression(static_cast<NameExpression*>(node));
                break;
            }
            case NodeKind::FunctionCallExpression:
            {
                prettyPrintFunctionCallExpression(static_cast<FunctionCallExpression*>(node));
                break;
            }
            case NodeKind::MemberAccessExpression:
            {
                prettyPrintMemberAccessExpression(static_cast<MemberAccessExpression*>(node));
                break;
            }
            case NodeKind::DiscardLiteral:
            {
                prettyPrintDiscardLiteral(static_cast<DiscardLiteral*>(node));
                break;
            }
            case NodeKind::BoolLiteral:
            {
                prettyPrintBoolLiteral(static_cast<BoolLiteral*>(node));
                break;
            }
            case NodeKind::NumberLiteral:
            {
                prettyPrintNumberLiteral(static_cast<NumberLiteral*>(node));
                break;
            }
            case NodeKind::StringLiteral:
            {
                prettyPrintStringLiteral(static_cast<StringLiteral*>(node));
                break;
            }
            case NodeKind::TypeNameNode:
            {
                prettyPrintTypeNameNode(static_cast<TypeNameNode*>(node));
                break;
            }
            case NodeKind::BlockNode:
            {
                prettyPrintBlockNode(static_cast<BlockNode*>(node));
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

        writeIndentedTypeName(statement->type());
        
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

        writeIndentedTypeName(statement->type());

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

        m_builder.appendIndentedLine("Right: {");
        m_builder.pushIndentation();

        prettyPrintNode(statement->rightExpression().get());

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

    void ParseTreePrinter::prettyPrintFunctionDefinitionStatement(FunctionDefinitionStatement* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();

        writeIndentedTypeName(statement->type());

        m_builder.appendIndented("Name: ").appendLine(statement->name());

        for (const auto& annotation : statement->annotations())
        {
            prettyPrintAnnotation(annotation.get());
        }

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

        writeIndentedTypeName(statement->type());

        if (statement->specialFunctionType() != SpecialFunctionType::None)
        {
            m_builder.appendIndented("SpecialFunctionType: ").appendLine(stringify(statement->specialFunctionType()));
        }
        m_builder.appendIndented("Modifier: ").appendLine(stringify(statement->modifier()));
        prettyPrintMethodNameNode(statement->methodNameNode().get());

        for (const auto& annotation : statement->annotations())
        {
            prettyPrintAnnotation(annotation.get());
        }

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

        writeIndentedTypeName(statement->type());
        m_builder.appendIndented("Name: ").appendLine(statement->name());

        for (const auto& annotation : statement->annotations())
        {
            prettyPrintAnnotation(annotation.get());
        }

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
        m_builder.appendIndented("Name: ").appendLine(statement->name());
    
        if (statement->valueExpression().has_value())
        {
            prettyPrintNode(statement->valueExpression().value().get());
        }
        if(statement->constantValue().has_value())
        {
            m_builder.appendIndented("Value: ").appendLine(std::to_string(statement->constantValue().value()));
        }
    
        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintTypeDefinitionStatement(TypeDefinitionStatement* statement)
    {
        m_builder.appendIndented(stringify(statement->kind())).appendLine(": {");
        m_builder.pushIndentation();

        writeIndentedTypeName(statement->type());
        m_builder.appendIndented("Name: ").appendLine(statement->name());
        for (const auto& annotation : statement->annotations())
        {
            prettyPrintAnnotation(annotation.get());
        }
        if (statement->constructorParameters().has_value())
        {
            prettyPrintParametersNode(statement->constructorParameters().value().get());
        }
        prettyPrintBlockNode(statement->bodyNode().get());

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

        writeIndentedTypeName(statement->type());

        if (statement->expression().has_value())
            prettyPrintNode(statement->expression().value().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintGroupingExpression(GroupingExpression* grouping)
    {
        m_builder.appendIndented(stringify(grouping->kind())).appendLine(": {");
        m_builder.pushIndentation();

        writeIndentedTypeName(grouping->type());

        prettyPrintNode(grouping->expression().get());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintUnaryExpression(UnaryExpression* unaryExpression)
    {
        m_builder.appendIndented(stringify(unaryExpression->kind())).appendLine(": {");
        m_builder.pushIndentation();

        writeIndentedTypeName(unaryExpression->type());

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

        writeIndentedTypeName(binaryExpression->type());

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

        writeIndentedTypeName(name->type());

        m_builder.appendIndented("Name: ").appendLine(name->name());

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintMethodNameNode(MethodNameNode* node)
    {
        m_builder.appendIndented(stringify(node->kind())).appendLine(": {");
        m_builder.pushIndentation();

        m_builder.appendIndented("MethodName: ");
        if (node->hasTypeName())
        {
            const auto& typeName = node->typeName().value();
            m_builder.append(typeName).append(".");
        }
        const auto& methodName = node->methodName();
        m_builder.appendLine(methodName);

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::prettyPrintFunctionCallExpression(FunctionCallExpression* functionCall)
    {
        m_builder.appendIndented(stringify(functionCall->kind())).appendLine(": {");
        m_builder.pushIndentation();

        writeIndentedTypeName(functionCall->type());
        writeIndentedTypeName(functionCall->functionType(), "Function type: ");

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

        const auto& name = parameter->name();
        m_builder.appendIndented("Name: ").appendLine(name);
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

        writeIndentedTypeName(node->type());

        m_builder.appendIndented("Lexeme: ").appendLine(node->name());
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

    void ParseTreePrinter::prettyPrintAnnotation(AnnotationNode* annotation)
    {
        m_builder.appendIndentedLine("Annotation: {");
        m_builder.pushIndentation();

        m_builder.appendIndented("Name: ").appendLine(annotation->name());
        if (annotation->hasParentheses())
        {
            const auto& arguments = annotation->arguments();

            m_builder.appendIndented("ArgumentsNode(").append(std::to_string(arguments.size())).appendLine("): {");
            m_builder.pushIndentation();

            for (const auto& argument : arguments)
            {
                if (argument.isNamed())
                {
                    m_builder.appendIndentedLine("NamedArgument: {");
                    m_builder.pushIndentation();

                    m_builder.appendIndented("Name: ").appendLine(argument.name());
                    prettyPrintNode(argument.value().get());

                    m_builder.popIndentation();
                    m_builder.appendIndentedLine("}");
                }
                else
                {
                    prettyPrintNode(argument.value().get());
                }
            }

            m_builder.popIndentation();
            m_builder.appendIndentedLine("}");
        }

        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void ParseTreePrinter::writeIndentedTypeName(Type type, std::string_view prefix)
    {
        m_builder.appendIndented(prefix);
        
        if(type.isReference())
        {
            m_builder.append("ref ");
            type = type.toValue();
        }
        
        if (type.kind() == TypeKind::Enum)
        {
            const auto& enumDefinition = m_module->getEnumDefinition(type);
            auto& name = enumDefinition.name();
            auto baseType = enumDefinition.baseType();

            m_builder
                .append(name)
                .append("(")
                .append(m_module->getNameByType(baseType))
                .appendLine(")");
            return;
        }
        else if (type.kind() == TypeKind::Type)
        {
            const auto& typeDefinition = m_module->getTypeDefinition(type);
            auto& name = typeDefinition.name();

            m_builder
                .appendLine(name);
            return;
        }
        else if (type.kind() == TypeKind::Function || type.kind() == TypeKind::Method || type.kind() == TypeKind::Constructor)
        {
            const auto& functionDefinition = m_module->getFunctionDefinition(type);
            const auto& parameters = functionDefinition.parameters();
            const auto& returnTypes = functionDefinition.returnTypes();

            m_builder.append("(");

            for (size_t i = 0; i < parameters.size(); i++)
            {
                auto parameter = parameters[i].type();
                if (parameter.isReference())
                {
                    m_builder.append("ref ");
                }
                m_builder.append(m_module->getNameByType(parameter));
                if (i < parameters.size() - 1)
                    m_builder.append(", ");
            }

            m_builder.append(") -> ");
            if (returnTypes.empty())
            {
                m_builder.appendLine("void");
                return;
            }

            for (size_t i = 0; i < returnTypes.size(); i++)
            {
                auto returnType = returnTypes[i];
                m_builder.append(m_module->getNameByType(returnType));
                if (i < returnTypes.size() - 1)
                    m_builder.append(", ");
            }
            m_builder.appendLine("");
            return;
        }

        auto name = m_module->getNameByType(type);
        m_builder.appendLine(name);
    }
}
