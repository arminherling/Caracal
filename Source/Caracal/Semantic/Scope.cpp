//#include "Scope.h"
//
//Scope::Scope(Scope* parent, ScopeKind kind)
//    : m_parent{ parent }
//    , m_kind{ kind }
//{
//}
//
//void Scope::addTypeBinding(QStringView identifier, Type type)
//{
//    m_typeBindings.try_emplace(identifier.toString(), type);
//}
//
//void Scope::addVariableBinding(QStringView identifier, Type type)
//{
//    m_variableBindings.try_emplace(identifier.toString(), type);
//}
//
//void Scope::addFunctionBinding(QStringView identifier, Type type)
//{
//    m_functionBindings.try_emplace(identifier.toString(), type);
//}
//
//Type Scope::tryGetTypeBinding(QStringView identifier) const noexcept
//{
//    if (auto search = m_typeBindings.find(identifier.toString()); search != m_typeBindings.end())
//        return search->second;
//    else
//        return Type::Undefined();
//}
//
//Type Scope::tryGetVariableBinding(QStringView identifier) const noexcept
//{
//    if (auto search = m_variableBindings.find(identifier.toString()); search != m_variableBindings.end())
//        return search->second;
//    else
//        return Type::Undefined();
//}
//
//Type Scope::tryGetFunctionBinding(QStringView identifier) const noexcept
//{
//    if (auto search = m_functionBindings.find(identifier.toString()); search != m_functionBindings.end())
//        return search->second;
//    else if (m_parent != nullptr)
//        return m_parent->tryGetFunctionBinding(identifier);
//    else
//        return Type::Undefined();
//}
