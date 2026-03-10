#pragma once

#include <Caracal/Syntax/Statement.h>
#include <Caracal/Semantic/Type.h>
#include <Caracal/Semantic/Parameter.h>
#include <vector>
#include <string>

namespace Caracal
{
    enum class FunctionType
    {
        None,
        FreeFunction,
        StaticMethod,
        PublicMethod,
        PrivateMethod,
        Constructor,
        Destructor
    };

    class FunctionDefinition
    {
    public:
        FunctionDefinition(
            const Statement* statement,
            Type type,
            Type parentType,
            FunctionType functionType,
            const std::string& name,
            const std::string& fullName,
            bool isVariadic,
            const std::vector<Parameter>& parameters = std::vector<Parameter>(),
            const std::vector<Type>& returnTypes = std::vector<Type>());

        [[nodiscard]] Type type() const noexcept { return m_type; }
        [[nodiscard]] Type parentType() const noexcept { return m_parentType; }
        [[nodiscard]] FunctionType functionType() const noexcept { return m_functionType; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] const std::string& fullName() const noexcept { return m_fullName; }
        [[nodiscard]] bool isVariadic() const noexcept { return m_isVariadic; }
        [[nodiscard]] const std::vector<Parameter>& parameters() const noexcept { return m_parameters; }
        [[nodiscard]] const std::vector<Type>& returnTypes() const noexcept { return m_returnTypes; }
        [[nodiscard]] const Statement* statement() const noexcept { return m_statement; }

        void setParameters(const std::vector<Parameter>& parameters) noexcept;
        void setReturnTypes(const std::vector<Type>& returnTypes) noexcept;
        void setIsVariadic(bool isVariadic) noexcept;

    private:
        Type m_type;
        Type m_parentType;
        FunctionType m_functionType;
        std::string m_name;
        std::string m_fullName;
        bool m_isVariadic;
        std::vector<Parameter> m_parameters;
        std::vector<Type> m_returnTypes;
        const Statement* m_statement;
    };
}
