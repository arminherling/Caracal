#include <Caracal/Text/SourceText.h>

namespace Caracal
{
    SourceText::SourceText(
        const std::string& text,
        const std::filesystem::path& filePath)
        : text(text)
        , filePath(filePath)
    {
    }
}
