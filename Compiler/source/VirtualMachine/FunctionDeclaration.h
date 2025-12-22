#pragma once

#include <Defines.h>
#include <Compiler/API.h>
#include <VirtualMachine/Label.h>

#include <string>

struct COMPILER_API FunctionDeclaration
{
    std::string name;
    Label entryPoint;
    u8 returnValues;
    u8 parameterValues;
};
