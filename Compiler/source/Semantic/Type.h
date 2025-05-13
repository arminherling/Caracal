#pragma once

#include <Compiler/API.h>
#include <Defines.h>

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

    class COMPILER_API Type
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

    private:
        i32 m_id;
        TypeKind m_kind;
    };

    COMPILER_API bool operator==(Type lhs, Type rhs) noexcept;
    COMPILER_API bool operator!=(Type lhs, Type rhs) noexcept;
}
