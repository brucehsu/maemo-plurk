#ifndef RESPONSESVIEW_H
#define RESPONSESVIEW_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QVBoxLayout>
#include <QCloseEvent>
#include "clicklabel.h"
#include "plurkdbmanager.h"

namespace Ui {
    class ResponsesView;
}

class ResponsesView : public QWidget
{
    Q_OBJECT

public:
    explicit ResponsesView(QWidget *parent = 0);
    ~ResponsesView();
    void setMaps(ItemMap *plurkItemMap, QMap<QString, ItemMap*>* userMap);
    void setDbManager(PlurkDbManager *dbManager);

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::ResponsesView *ui;
    ItemMap *plurkItemMap;
    QMap<QString, ItemMap*> *userMap;
    QNetworkAccessManager *manager;
    QVBoxLayout *plurkLayout;
    PlurkDbManager *dbManager;
    ClickLabel *plurkLabel;
    QWidget *parentWidget;

private slots:
    void refreshView();
};

#endif // RESPONSESVIEW_H
