#include "Scope.h"

namespace Caracal
{
    Scope::Scope(Scope* parent, ScopeKind kind)
        : m_parent{ parent }
        , m_kind{ kind }
    {
    }

    //void Scope::addTypeBinding(std::string_view identifier, Type type)
    //{
    //    m_typeBindings.try_emplace(identifier, type);
    //}

    bool Scope::hasVariableBinding(std::string_view identifier) const noexcept
    {
        if (m_variableBindings.find(identifier) != m_variableBindings.end())
            return true;
        else if (m_parent != nullptr)
            return m_parent->hasVariableBinding(identifier);
        else
            return false;
    }

    void Scope::addVariableBinding(std::string_view identifier, Type type, std::optional<SourceLocation> location, const SourceTextSharedPtr& source)
    {
        m_variableBindings.try_emplace(identifier, VariableBinding{ type, source, location });
    }

    //void Scope::addFunctionBinding(std::string_view identifier, Type type)
    //{
    //    m_functionBindings.try_emplace(identifier, type);
    //}

    //std::optional<Type> Scope::tryGetTypeBinding(std::string_view identifier) const noexcept
    //{
    //    if (auto search = m_typeBindings.find(identifier); search != m_typeBindings.end())
    //        return std::make_optional<Type>(search->second);
    //    else
    //        return std::nullopt;
    //}

    std::optional<Type> Scope::tryGetVariableBinding(std::string_view identifier) const noexcept
    {
        if (auto search = m_variableBindings.find(identifier); search != m_variableBindings.end())
            return std::make_optional<Type>(search->second.type);
        else if (m_parent != nullptr)
            return m_parent->tryGetVariableBinding(identifier);
        else
            return std::nullopt;
    }

    SourceTextSharedPtr Scope::tryGetVariableBindingSource(std::string_view identifier) const noexcept
    {
        if (auto search = m_variableBindings.find(identifier); search != m_variableBindings.end())
            return search->second.source;
        else if (m_parent != nullptr)
            return m_parent->tryGetVariableBindingSource(identifier);
        else
            return nullptr;
    }

    std::optional<SourceLocation> Scope::tryGetVariableBindingLocation(std::string_view identifier) const noexcept
    {
        if (auto search = m_variableBindings.find(identifier); search != m_variableBindings.end())
            return search->second.location;
        else if (m_parent != nullptr)
            return m_parent->tryGetVariableBindingLocation(identifier);
        else
            return std::nullopt;
    }

    //std::optional<Type> Scope::tryGetFunctionBinding(std::string_view identifier) const noexcept
    //{
    //    if (auto search = m_functionBindings.find(identifier); search != m_functionBindings.end())
    //        return std::make_optional<Type>(search->second);
    //    else if (m_parent != nullptr)
    //        return m_parent->tryGetFunctionBinding(identifier);
    //    else
    //        return std::nullopt;
    //}
}
