#include <Compiler/File.h>
#include <QFile>

namespace Caracal 
{
    QByteArray File::readAll(const QString& filePath)
    {
        auto file = QFile(filePath);
        file.open(QIODevice::ReadOnly);
        return file.readAll();
    }

    QString File::readAllText(const QString& filePath)
    {
        return QString::fromUtf8(File::readAll(filePath));
    }
}
