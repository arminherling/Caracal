#include "DebugTools.h"
#include <QtWidgets/QApplication>

#include "TokenListView.h"
#include <Caracal/Text/SourceText.h>
#include <Caracal/Syntax/Lexer.h>

i32 main(i32 argc, char *argv[])
{
    QApplication a(argc, argv);
    DebugTools w;
    //TokenListView w;

    //auto input = QString("define sum(a int, b int) \r\n {\r\n return a + b \r\n}\r\n");

    //auto source = SourceText(input);
    //auto lexer = Lexer(source);

    //auto result = lexer.Lex();

    //w.setTokens(QList<Token>() << result.tokens[9]);
    //w.setTokens(result.tokens);

    w.show();
    return a.exec();
}
