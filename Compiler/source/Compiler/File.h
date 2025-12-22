#pragma once

#include <Compiler/API.h>

#include <QByteArray>
#include <QString>
#include <filesystem>
#include <string>
#include <optional>

namespace Caracal
{
    class COMPILER_API File
    {
    public:
        // TODO remove qt
        static QByteArray ReadAll(const QString& filePath);
        static QString ReadAllText(const QString& filePath);
        
        static std::optional<std::string> readText(const std::filesystem::path& filePath);
    };
}
