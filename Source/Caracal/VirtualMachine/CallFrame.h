#pragma once

#include <Caracal/Defines.h>

struct CallFrame
{
    u16 returnAddress;
    u16 baseRegister;
};
