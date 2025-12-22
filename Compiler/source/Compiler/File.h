#pragma once

#include <Compiler/API.h>
#include <filesystem>
#include <string>
#include <optional>

namespace Caracal::File
{
    COMPILER_API std::optional<std::string> readText(const std::filesystem::path& filePath);
}
