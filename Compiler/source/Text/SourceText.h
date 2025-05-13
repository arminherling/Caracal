#pragma once

#include <Defines.h>
#include <Compiler/API.h>

#include <QString>

namespace Caracal
{
    struct COMPILER_API SourceText
    {
        SourceText(const QString& text, const QString& filePath = QString());

        QString text;
        QString filePath;
    };

    using SourceTextSharedPtr = std::shared_ptr<SourceText>;
}
