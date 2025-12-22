#pragma once

#include <Caracal/Syntax/Token.h>

#include <QList>
#include <QTableView>
#include <QStandardItemModel>

class TokenListView : public QTableView
{
public:
    TokenListView(QWidget* parent = nullptr);

    void setTokens(const QList<Token>& tokens);

private:
    QStandardItemModel* model;
};
