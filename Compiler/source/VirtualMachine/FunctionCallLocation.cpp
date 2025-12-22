#include "FunctionCallLocation.h"

FunctionCallLocation::FunctionCallLocation(
    const std::string& name,
    JumpIndex target)
    : name{ name }
    , target{ target }
{
}
