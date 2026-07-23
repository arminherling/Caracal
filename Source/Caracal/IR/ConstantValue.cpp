#include <Caracal/IR/ConstantValue.h>

namespace Caracal
{
    ConstantValue ConstantValue::FromBool(bool value) noexcept
    {
        return ConstantValue{ Data{ LiteralData{ value } } };
    }

    ConstantValue ConstantValue::FromU8(u8 value) noexcept
    {
        return ConstantValue{ Data{ LiteralData{ value } } };
    }

    ConstantValue ConstantValue::FromI32(i32 value) noexcept
    {
        return ConstantValue{ Data{ LiteralData{ value } } };
    }

    ConstantValue ConstantValue::FromF32(float value) noexcept
    {
        return ConstantValue{ Data{ LiteralData{ value } } };
    }

    ConstantValue ConstantValue::FromString(std::string value) noexcept
    {
        return ConstantValue{ Data{ LiteralData{ std::move(value) } } };
    }

    ConstantValue ConstantValue::FromLiteralData(LiteralData value) noexcept
    {
        return ConstantValue{ Data{ std::move(value) } };
    }

    ConstantValue ConstantValue::FromEnum(Type enumType, std::string enumName, std::string fieldName, LiteralData underlyingValue) noexcept
    {
        return ConstantValue{ Data{ EnumConstant{ enumType, std::move(enumName), std::move(fieldName), std::move(underlyingValue) } } };
    }

    ConstantValue ConstantValue::FromAggregate(AggregateData elements) noexcept
    {
        return ConstantValue{ Data{ std::move(elements) } };
    }

    ConstantValue::ConstantValue(Data data) noexcept
        : m_data{ std::move(data) }
    {
    }
}
