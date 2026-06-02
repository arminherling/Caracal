#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>
#include <Caracal/Semantic/Type.h>

#include <string>
#include <variant>

namespace Caracal
{
    class CARACAL_API ConstantValue
    {
    public:
        using LiteralData = std::variant<bool, u8, i32, float, std::string>;

        struct EnumConstant final
        {
            Type enumType{ Type::Undefined() };
            std::string enumName;
            std::string fieldName;
            LiteralData underlyingValue;

            bool operator==(const EnumConstant& other) const noexcept = default;
        };

        using Data = std::variant<LiteralData, EnumConstant>;

        static ConstantValue FromBool(bool value) noexcept;
        static ConstantValue FromU8(u8 value) noexcept;
        static ConstantValue FromI32(i32 value) noexcept;
        static ConstantValue FromF32(float value) noexcept;
        static ConstantValue FromString(std::string value) noexcept;
        static ConstantValue FromLiteralData(LiteralData value) noexcept;
        static ConstantValue FromEnum(Type enumType, std::string enumName, std::string fieldName, LiteralData underlyingValue) noexcept;

        [[nodiscard]] const Data& data() const noexcept { return m_data; }
        [[nodiscard]] const LiteralData* tryGetLiteralData() const noexcept { return std::get_if<LiteralData>(&m_data); }
        [[nodiscard]] const EnumConstant* tryGetEnumConstant() const noexcept { return std::get_if<EnumConstant>(&m_data); }

    private:
        explicit ConstantValue(Data data) noexcept;

        Data m_data;
    };
}
