#include <Caracal/IR/EnumDeclaration.h>

namespace Caracal
{
    EnumDeclaration::EnumDeclaration(std::string name, Type type, Type baseType)
        : m_type{ type }
        , m_baseType{ baseType }
        , m_name{ std::move(name) }
    {
    }

    void EnumDeclaration::addField(std::string name, ConstantValue value)
    {
        m_fields.push_back(Field{ std::move(name), std::move(value) });
    }
}
