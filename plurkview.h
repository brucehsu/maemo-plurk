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
    void setUserId(QString id);

private:
    Ui::PlurkView *ui;
    QNetworkAccessManager *networkManager;
    QNetworkRequest *req;
    QNetworkReply *rep;
    QVariant *cookie;
    QString userId;
    QMap<QString,ClickLabel*> plurkMap;
    QMap<QString, ItemMap*>* dbPlurkMap;
    QMap<QString, ItemMap*>* dbUserMap;
    QVBoxLayout *plurkLayout;
    QButtonGroup *btnGroup;
    PlurkDbManager *dbManager;
    void addPlurkLabel(QString plurk_id,QString owner_id,
                       QString owner_name,QString owner_image,
                       QString owner_avatar, QString qual_trans,
                       QString content, QString res_cnt);
    void loadPlurkFromDb();

public slots:
    void getPlurksFinished(QNetworkReply* reply);
    void getPlurks();
    void displayAllPlurks();
    void displayMyPlurks();
    void displayPrivate();
    void displayResponded();
    void displayLiked();
    void displayUnread();
};

#endif // PLURKVIEW_H
