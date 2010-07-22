#ifndef PLURKVIEW_H
#define PLURKVIEW_H

#include <QWidget>
#include <QStringListModel>
#include <QLabel>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QDateTime>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkCookieJar>
#include <qjson/parser.h>
#include "clicklabel.h"
#include "plurkdbmanager.h"

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

private:
    Ui::PlurkView *ui;
    QNetworkAccessManager *networkManager;
    QNetworkRequest *req;
    QNetworkReply *rep;
    QVariant *cookie;
    QList<ClickLabel*> plurkList;
    QList<QMap<QString,QString>*>* userList;
    QVBoxLayout *plurkLayout;
    QButtonGroup *btnGroup;
    PlurkDbManager *dbManager;
    void addPlurkLabel(QString plurk_id,QString owner_name,
                       QString qual_trans, QString content,
                       QString res_cnt);

public slots:
    void loadFinished();
    void loadPlurks();
};

#endif // PLURKVIEW_H
