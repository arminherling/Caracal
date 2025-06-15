#include <Compiler/File.h>
#include <QFile>

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
}
