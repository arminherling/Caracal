//#pragma once
//
//#include <Caracal/Defines.h>
//#include <Semantic/Parameter.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <QHash>
//#include <QString>
//
//class FunctionDefinition
//{
//public:
//    FunctionDefinition(Type type, const QString& name);
//
//    [[nodiscard]] Type type() const noexcept { return m_type; }
//    [[nodiscard]] QString name() const noexcept { return m_name; }
//    [[nodiscard]] QList<Parameter*> parameters() const noexcept { return m_parameters; }
//    [[nodiscard]] Type returnType() const noexcept { return m_returnType; }
//
//    void setParameters(const QList<Parameter*>& parameters);
//    void setReturnType(Type returnType);
//
//private:
//    Type m_type;
//    QString m_name;
//    QList<Parameter*> m_parameters;
//    Type m_returnType;
//};
