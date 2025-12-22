#pragma once

#include <Compiler/API.h>
#include <VirtualMachine/JumpIndex.h>
#include <string>

struct COMPILER_API FunctionCallLocation
{
    FunctionCallLocation(const std::string& name, JumpIndex target);

    std::string name;
    JumpIndex target;
};
