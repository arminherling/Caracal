#pragma once

#include <cstdint>

#define CARACAL_DELETE_COPY_DEFAULT_MOVE(ClassName)         \
    ClassName(const ClassName&) = delete;                   \
    ClassName& operator=(const ClassName&) = delete;        \
    ClassName(ClassName&&) = default;                       \
    ClassName& operator=(ClassName&&) = default;

#define TODO(X) __debugbreak();

using i8 = int8_t;
using i32 = int32_t;

using u8 = uint8_t;
using u16 = uint16_t;
