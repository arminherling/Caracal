#pragma once

#include <Caracal/Syntax/TypeDefinitionStatement.h>
#include <Caracal/Semantic/Type.h>
#include <Caracal/Semantic/FieldDefinition.h>
#include <optional>
#include <string>

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
        void addField(Type fieldType, const std::string& fieldName, Expression* expression) noexcept;
        [[nodiscard]] const FieldDefinition& tryGetFieldByName(std::string_view fieldName) const noexcept;
        void addMethod(Type methodType, const std::string& methodName) noexcept;
        [[nodiscard]] Type tryGetMethodTypeByName(std::string_view methodName) const noexcept;
        [[nodiscard]] const TypeDefinitionStatement* statement() const noexcept { return m_statement; }
        
    private:
        Type m_type;
        std::string m_name;
        std::unordered_map<std::string, FieldDefinition> m_fields;
        std::unordered_map<std::string, Type> m_methods;
        const TypeDefinitionStatement* m_statement;
    };
}
