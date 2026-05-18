#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>

#include <variant>

namespace Caracal
{
    class CARACAL_API ConstantValue
    {
    public:
        using Data = std::variant<bool, u8, i32, float>;

        static ConstantValue FromBool(bool value) noexcept;
        static ConstantValue FromU8(u8 value) noexcept;
        static ConstantValue FromI32(i32 value) noexcept;
        static ConstantValue FromF32(float value) noexcept;

        [[nodiscard]] const Data& data() const noexcept { return m_data; }

    private:
        explicit ConstantValue(Data data) noexcept;

        Data m_data;
    };
}
