#include "DebugTools.h"

#include <QTextEdit>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QHBoxLayout>

#include <Caracal/Text/SourceText.h>
#include <Caracal/Syntax/Lexer.h>

#include "TokenListView.h"

DebugTools::DebugTools(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(" ");
    resize(1000, 600);
    auto menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    //auto mainToolBar = new QToolBar(this);
    //mainToolBar->setMovable(false);
    //addToolBar(mainToolBar);
    
    auto centralWidget = new QWidget(this);

    auto centralLayout = new QHBoxLayout(centralWidget);

    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);

    QFont consolas{ "Consolas", 12, QFont::Normal };
    textEdit = new QTextEdit(this);
    textEdit->setFont(consolas);
    textEdit->setPlainText("define sum(a int, b int) \r\n{\r\n    return a + b \r\n}\r\n");
    textEdit->createStandardContextMenu();
    textEdit->setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
    connect(textEdit, &QTextEdit::textChanged, this, &DebugTools::onTextChanged);


    centralLayout->addWidget(textEdit);
    //setCentralWidget(textEdit);

    //auto debugDockWidget = new QDockWidget(this);
    //debugDockWidget->setWindowTitle("Token");
    tokenList = new TokenListView(this);
    //tokenList->setMinimumWidth(500);
    centralLayout->addWidget(tokenList);
    //debugDockWidget->setWidget(tokenList);
    //auto f = debugDockWidget->features();

    //setTabPosition(Qt::DockWidgetArea::RightDockWidgetArea, QTabWidget::TabPosition::North);

    //addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, debugDockWidget);

    //auto statusBar = new QStatusBar(this);
    //setStatusBar(statusBar);
    //statusBar->showMessage("Test");
    onTextChanged();
}
#include <chrono>
#include <qdebug.h>
void DebugTools::onTextChanged()
{
    auto input = textEdit->toPlainText();

    //auto source = SourceText(input);
    //auto lexer = Lexer(source);
    //auto start = std::chrono::steady_clock::now();

    //// do some stuff here
    //auto result = lexer.Lex();

    //auto end = std::chrono::steady_clock::now();
    //qDebug() <<"onTextChanged"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    ////w.setTokens(QList<Token>() << result.tokens[9]);
    //tokenList->setTokens(result.tokens);


}
