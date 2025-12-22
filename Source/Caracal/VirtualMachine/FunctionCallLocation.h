#pragma once

#include <Caracal/API.h>
#include <Caracal/VirtualMachine/JumpIndex.h>
#include <string>

struct CARACAL_API FunctionCallLocation
{
    FunctionCallLocation(const std::string& name, JumpIndex target);

    std::string name;
    JumpIndex target;
};
