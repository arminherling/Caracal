#pragma once

#include <Compiler/API.h>

#include <QByteArray>
#include <QString>

namespace Caracal
{
    class COMPILER_API File
    {
    public:
        static QByteArray readAll(const QString& filePath);
        static QString readAllText(const QString& filePath);
    };
}
