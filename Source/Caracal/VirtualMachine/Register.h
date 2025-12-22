#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>

struct CARACAL_API Register
{
    Register(u16 value);

    u16 index;
};
