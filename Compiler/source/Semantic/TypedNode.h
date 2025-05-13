//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Node.h>
//#include <Syntax/NodeKind.h>
//#include <Semantic/Type.h>
//
//class COMPILER_API TypedNode
//{
//public:
//    TypedNode(Node* source, NodeKind kind, Type type);
//
//    [[nodiscard]] Node* source() const noexcept { return m_source; }
//    [[nodiscard]] NodeKind kind() const noexcept { return m_kind; }
//    [[nodiscard]] Type type() const noexcept { return m_type; }
//    void setType(Type type) noexcept { m_type = type; }
//
//private:
//    Node* m_source;
//    NodeKind m_kind;
//    Type m_type;
//};
