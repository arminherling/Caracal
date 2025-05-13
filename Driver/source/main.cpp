#include <Compiler/File.h>
#include <Debug/ParseTreePrinter.h>
#include <Syntax/Lexer.h>
#include <Syntax/Token.h>
#include <Syntax/TokenKind.h>
#include <Syntax/TokenBuffer.h>
#include <Syntax/Parser.h>

#include <QString>
#include <QFileInfo>

#include <iostream>
#include <memory>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Please provide a file path as a parameter.\n";
        return -1;
    }

    QString filePath = QString::fromLocal8Bit(argv[1]);
    auto fileInfo = QFileInfo(filePath);
    if (!fileInfo.isFile())
    {
        std::cout << "The parameter is not a valid file.\n";
        return -1;
    }
    
    auto absolutePath = fileInfo.absoluteFilePath();
    std::cout << "File path received: " << absolutePath.toStdString() << "\n";


    auto fileContent = File::ReadAllText(absolutePath);
    auto source = std::make_shared<SourceText>(fileContent);
    DiagnosticsBag diagnostics;

    auto tokens = Lex(source, diagnostics);
    auto parseTree = Parse(tokens, diagnostics);
    ParseTreePrinter printer{ parseTree };
    auto output = printer.PrettyPrint();
    std::cout << "AST: \n" << output.toStdString() << "\n";

    return 0;
}
