#include <Compiler/File.h>
#include <fstream>
#include <sstream>

namespace Caracal::File
{
    std::optional<std::string> readText(const std::filesystem::path& filePath)
    {
        if (!std::filesystem::exists(filePath))
        {
            return std::nullopt;
        }

        std::ifstream file(filePath, std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            return std::nullopt;
        }

        std::ostringstream contentStream;
        contentStream << file.rdbuf();
        return contentStream.str();
    }
}
