#include <Text/SourceText.h>

namespace Caracal
{
    SourceText::SourceText(
        const QString& text,
        const QString& filePath)
        : text(text)
        , filePath(filePath)
    {
    }
}
