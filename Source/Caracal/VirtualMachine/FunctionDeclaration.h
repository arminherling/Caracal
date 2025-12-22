#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>
#include <Caracal/VirtualMachine/Label.h>

#include <string>

struct CARACAL_API FunctionDeclaration
{
    std::string name;
    Label entryPoint;
    u8 returnValues;
    u8 parameterValues;
};
