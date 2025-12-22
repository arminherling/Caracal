#include "Value.h"

Value::Value()
    : data{}
{
}

Value::Value(bool value)
    : data{value}
{
}

Value::Value(i32 value)
    : data{ value }
{
}

bool Value::isBool()
{
    return std::holds_alternative<bool>(data);
}

bool Value::isInt32()
{
    return std::holds_alternative<i32>(data);
}

bool Value::asBool()
{
    return std::get<bool>(data);
}

i32 Value::asInt32()
{
    return std::get<i32>(data);
}
