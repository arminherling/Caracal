#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>
#include <functional>

namespace Caracal
{
    enum class TypeKind
    {
        Invalid,
        Builtin,
        Enum,
        Type,
        Function
    };

    class CARACAL_API Type
    {
    public:
        Type(i32 id, TypeKind kind);

        [[nodiscard]] i32 id() const noexcept { return m_id; }
        [[nodiscard]] TypeKind kind() const noexcept { return m_kind; }

        [[nodiscard]] static Type Discard() noexcept { return Type(-2, TypeKind::Builtin); }
        [[nodiscard]] static Type Undefined() noexcept { return Type(-1, TypeKind::Builtin); }
        [[nodiscard]] static Type Void() noexcept { return Type(1, TypeKind::Builtin); }

        [[nodiscard]] static Type Bool() noexcept { return Type(2, TypeKind::Builtin); }
        [[nodiscard]] static Type RefBool() noexcept { return Type(3, TypeKind::Builtin); }
        //[[nodiscard]] static Type NullableBool() noexcept { return Type(4, TypeKind::Builtin); }
        //[[nodiscard]] static Type NullableRefBool() noexcept { return Type(5, TypeKind::Builtin); }

        [[nodiscard]] static Type U8() noexcept { return Type(6, TypeKind::Builtin); }
        [[nodiscard]] static Type RefU8() noexcept { return Type(7, TypeKind::Builtin); }
        //[[nodiscard]] static Type NullableU8() noexcept { return Type(8, TypeKind::Builtin); }
        //[[nodiscard]] static Type NullableRefU8() noexcept { return Type(9, TypeKind::Builtin); }

        [[nodiscard]] static Type I32() noexcept { return Type(10, TypeKind::Builtin); }
        [[nodiscard]] static Type RefI32() noexcept { return Type(11, TypeKind::Builtin); }
        //[[nodiscard]] static Type NullableI32() noexcept { return Type(12, TypeKind::Builtin); }
        //[[nodiscard]] static Type NullableRefI32() noexcept { return Type(13, TypeKind::Builtin); }

        [[nodiscard]] static Type F32() noexcept { return Type(14, TypeKind::Builtin); }
        [[nodiscard]] static Type RefF32() noexcept { return Type(15, TypeKind::Builtin); }
        //[[nodiscard]] static Type NullableF32() noexcept { return Type(16, TypeKind::Builtin); }
        //[[nodiscard]] static Type NullableRefF32() noexcept { return Type(17, TypeKind::Builtin); }

        [[nodiscard]] static Type String() noexcept { return Type(18, TypeKind::Builtin); }
        [[nodiscard]] static Type RefString() noexcept { return Type(19, TypeKind::Builtin); }
        //[[nodiscard]] static Type NullableString() noexcept { return Type(20, TypeKind::Builtin); }
        //[[nodiscard]] static Type NullableRefString() noexcept { return Type(21, TypeKind::Builtin); }

    private:
        i32 m_id;
        TypeKind m_kind;
    };

    CARACAL_API bool operator==(Type lhs, Type rhs) noexcept;
    CARACAL_API bool operator!=(Type lhs, Type rhs) noexcept;
}

namespace std {
    template<>
    struct hash<Caracal::Type>
    {
        std::size_t operator()(const Caracal::Type& t) const noexcept
        {
            return std::hash<int>()(t.id());
        }
    };
}
