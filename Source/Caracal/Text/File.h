#pragma once

#include <Caracal/API.h>
#include <filesystem>
#include <string>
#include <optional>

namespace Caracal::File
{
    CARACAL_API std::optional<std::string> readText(const std::filesystem::path& filePath);
}
