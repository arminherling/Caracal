//#pragma once
//
//#include <Caracal/Defines.h>
//#include <Semantic/Field.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <QString>
//#include <QHash>
//
//class EnumDefinition
//{
//public:
//    EnumDefinition(Type type, const QString& name);
//
//    [[nodiscard]] Type type() const noexcept { return m_type; }
//    [[nodiscard]] QString name() const noexcept { return m_name; }
//    [[nodiscard]] Field* getFieldByName(QStringView fieldName) const noexcept;
//
//    void addField(Type type, QStringView name, TypedExpression* expression) noexcept;
//
//private:
//    Type m_type;
//    QString m_name;
//    std::unordered_map<QString, Field*> m_fields;
//};
