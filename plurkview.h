#ifndef PLURKVIEW_H
#define PLURKVIEW_H

#include <QWidget>
#include <QListView>
#include <QStringListModel>
#include <QLabel>
#include <QVBoxLayout>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkCookieJar>
#include <qjson/parser.h>
#include "clicklabel.h"

namespace Ui {
    class PlurkView;
}

class PlurkView : public QWidget
{
    Q_OBJECT

public:
    explicit PlurkView(QWidget *parent = 0);
    ~PlurkView();
    void setCookie(QVariant* cookie);
    void setNetwork(QNetworkAccessManager* manager);
    void loadPlurks();

private:
    Ui::PlurkView *ui;
    QNetworkAccessManager *manager;
    QNetworkRequest *req;
    QNetworkReply *rep;
    QVariant *cookie;
    QList<clickLabel*> plurkList;
    QMap<QString, QString> imgMap;
    QVBoxLayout* plurkLayout;

public slots:
    void loadFinished();
};

#endif // PLURKVIEW_H
