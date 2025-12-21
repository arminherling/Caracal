#include "TokenListView.h"

#include <QStandardItemModel>
#include <QHeaderView>
#include <qdebug.h>

TokenListView::TokenListView(QWidget* parent)
    : QTableView(parent)
    , model(new QStandardItemModel(this))
{
    setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    setFocusPolicy(Qt::NoFocus);
    setShowGrid(false);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(0);

    setColumnWidth(0, 200);
    setColumnWidth(1, 200);
    horizontalHeader()->setStretchLastSection(true);
    model->setHorizontalHeaderLabels(QStringList() << QString("Lexeme") << QString("Kind") << QString("Location"));
    
    setModel(model);
}

void TokenListView::setTokens(const QList<Token>& tokens)
{
    auto start = std::chrono::steady_clock::now();

    // do some stuff here
    //model->clear();
    model->removeRows(0, model->rowCount());
    //model->setHorizontalHeaderLabels(QStringList() << QString("Lexeme")  << QString("Kind") << QString("Location"));
    for (i32 i = 0; i < tokens.length(); i++)
    {
        auto& token = tokens.at(i);

        //model->setItem(i, 0, new QStandardItem(token.lexeme));
        //model->setItem(i, 1, new QStandardItem(stringify(token.kind)));
        //model->setItem(i, 2, new QStandardItem(token.location.stringify()));
    }
    //resizeColumnsToContents();
    auto end = std::chrono::steady_clock::now();
    qDebug() << "count" << tokens.count();
qDebug() << "setTokens" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

