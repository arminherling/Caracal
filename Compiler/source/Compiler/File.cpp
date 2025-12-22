#include <Compiler/File.h>
#include <QFile>

#include <fstream>
#include <sstream>

namespace Caracal 
{
    QByteArray File::ReadAll(const QString& filePath)
    {
        auto file = QFile(filePath);
        file.open(QIODevice::ReadOnly);
        return file.readAll();
    }

    QString File::ReadAllText(const QString& filePath)
    {
        return QString::fromUtf8(File::ReadAll(filePath));
    }

    std::optional<std::string> File::readText(const std::filesystem::path& filePath)
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
