#pragma once

#include <cstdint>

#define CARACAL_DELETE_COPY_DEFAULT_MOVE(ClassName)         \
    ClassName(const ClassName&) = delete;                   \
    ClassName& operator=(const ClassName&) = delete;        \
    ClassName(ClassName&&) = default;                       \
    ClassName& operator=(ClassName&&) = default;

#define CARACAL_DELETE_COPY_DELETE_MOVE(ClassName)          \
    ClassName(const ClassName&) = delete;                   \
    ClassName& operator=(const ClassName&) = delete;        \
    ClassName(ClassName&&) = delete;                        \
    ClassName& operator=(ClassName&&) = delete;

#if defined(_MSC_VER)
#define TODO(X)  __debugbreak();
#elif defined(__GNUC__) || defined(__clang__)
#define TODO(X)  __builtin_trap();
#else
#define TODO(X) static_assert(false, "Debug break not supported for this compiler");
#endif

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;
