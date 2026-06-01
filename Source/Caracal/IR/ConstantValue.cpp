#include <Caracal/IR/ConstantValue.h>

namespace Caracal
{
    ConstantValue ConstantValue::FromBool(bool value) noexcept
    {
        return ConstantValue{ Data{ value } };
    }

    ConstantValue ConstantValue::FromU8(u8 value) noexcept
    {
        return ConstantValue{ Data{ value } };
    }

    ConstantValue ConstantValue::FromI32(i32 value) noexcept
    {
        return ConstantValue{ Data{ value } };
    }

    ConstantValue ConstantValue::FromF32(float value) noexcept
    {
        return ConstantValue{ Data{ value } };
    }

    ConstantValue ConstantValue::FromString(std::string value) noexcept
    {
        return ConstantValue{ Data{ std::move(value) } };
    }

    ConstantValue::ConstantValue(Data data) noexcept
        : m_data{ std::move(data) }
    {
    }
}
