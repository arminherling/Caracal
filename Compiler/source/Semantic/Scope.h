//#pragma once
//
//#include <Compiler/API.h>
//#include <Semantic/Type.h>
//
//#include <QHash>
//#include <QString>
//
//#include <unordered_map>
//
//enum class ScopeKind
//{
//    Invalid,
//    Global,
//    Function,
//    Type,
//    Method,
//};
//
//class COMPILER_API Scope
//{
//public:
//    Scope(Scope* parent, ScopeKind kind);
//
//    [[nodiscard]] ScopeKind kind() const noexcept { return m_kind; }
//
//    void addTypeBinding(QStringView identifier, Type node);
//    void addVariableBinding(QStringView identifier, Type node);
//    void addFunctionBinding(QStringView identifier, Type node);
//    [[nodiscard]] Type tryGetTypeBinding(QStringView identifier) const noexcept;
//    [[nodiscard]] Type tryGetVariableBinding(QStringView identifier) const noexcept;
//    [[nodiscard]] Type tryGetFunctionBinding(QStringView identifier) const noexcept;
//
//private:
//    Scope* m_parent;
//    ScopeKind m_kind;
//    std::unordered_map<QString, Type> m_typeBindings;
//    std::unordered_map<QString, Type> m_variableBindings;
//    std::unordered_map<QString, Type> m_functionBindings;
//};
