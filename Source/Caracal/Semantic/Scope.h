#pragma once

#include <Caracal/API.h>
#include <Caracal/Semantic/Type.h>

#include <string_view>
#include <unordered_map>
#include <optional>

namespace Caracal 
{
    enum class ScopeKind
    {
        Invalid,
        Global,
        Function,
        Type,
        Method,
    };

    class CARACAL_API Scope
    {
    public:
        Scope(Scope* parent, ScopeKind kind);

        [[nodiscard]] ScopeKind kind() const noexcept { return m_kind; }

        bool hasVariableBinding(std::string_view identifier) const noexcept;

        //void addTypeBinding(std::string_view identifier, Type node);
        void addVariableBinding(std::string_view identifier, Type node);
        //void addFunctionBinding(std::string_view identifier, Type node);
        //[[nodiscard]] std::optional<Type> tryGetTypeBinding(std::string_view identifier) const noexcept;
        [[nodiscard]] std::optional<Type> tryGetVariableBinding(std::string_view identifier) const noexcept;
        //[[nodiscard]] std::optional<Type> tryGetFunctionBinding(std::string_view identifier) const noexcept;

    private:
        Scope* m_parent;
        ScopeKind m_kind;
        //std::unordered_map<std::string_view, Type> m_typeBindings;
        std::unordered_map<std::string_view, Type> m_variableBindings;
        //std::unordered_map<std::string_view, Type> m_functionBindings;
    };
}
