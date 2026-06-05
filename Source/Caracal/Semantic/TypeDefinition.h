#pragma once

#include <Caracal/Syntax/TypeDefinitionStatement.h>
#include <Caracal/Semantic/Type.h>
#include <Caracal/Semantic/FieldDefinition.h>
#include <optional>
#include <string>
#include <vector>

namespace Caracal
{
    class TypeDefinition
    {
    public:
        TypeDefinition(
            const TypeDefinitionStatement* statement,
            Type type,
            const std::string& name);

        [[nodiscard]] Type type() const noexcept { return m_type; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        void addField(Type fieldType, const std::string& fieldName, i32 fieldIndex, Expression* expression, bool isConstant = false) noexcept;
        [[nodiscard]] const FieldDefinition& tryGetFieldByName(std::string_view fieldName) const noexcept;
        [[nodiscard]] const std::vector<FieldDefinition>& fields() const noexcept { return m_fields; }
        void addMethod(Type methodType, const std::string& methodName) noexcept;
        [[nodiscard]] Type tryGetMethodTypeByName(std::string_view methodName) const noexcept;
        [[nodiscard]] const TypeDefinitionStatement* statement() const noexcept { return m_statement; }
        
    private:
        Type m_type;
        std::string m_name;
        std::vector<FieldDefinition> m_fields;
        std::unordered_map<std::string, Type> m_methods;
        const TypeDefinitionStatement* m_statement;
    };
}
