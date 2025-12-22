#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>

#include <variant>

struct CARACAL_API Value
{
    Value();
    Value(bool value);
    Value(i32 value);

    bool isBool();
    bool isInt32();

    bool asBool();
    i32 asInt32();

    std::variant<bool, i32> data;
};
