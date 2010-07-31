#ifndef PLURKVIEW_H
#define PLURKVIEW_H

#include <QWidget>
#include <QStringListModel>
#include <QLabel>
#include <QLocale>
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
    void setUserName(QString name);

private:
    Ui::PlurkView *ui;
    QNetworkAccessManager *networkManager, *avatarNetworkManager;
    QNetworkRequest *req;
    QNetworkReply *rep;
    QVariant *cookie;
    QString userId;
    QString userName;
    QMap<QString,ClickLabel*> plurkMap;
    QMap<QString, ItemMap*>* dbPlurkMap;
    QMap<QString, ItemMap*>* dbUserMap;
    QVBoxLayout *plurkLayout;
    QButtonGroup *btnGroup;
    PlurkDbManager *dbManager;
    void addPlurkLabel(QString plurk_id);
    void loadPlurkFromDb();
    void getAvatars();
    void refreshQueryMaps();
    void refreshPlurkLabels();

public slots:
    void plurkRequestFinished(QNetworkReply* reply);
    void getAvatarsFinished(QNetworkReply* reply);
    void getPlurks();
    void displayAllPlurks();
    void displayMyPlurks();
    void displayPrivate();
    void displayResponded();
    void displayLiked();
    void displayUnread();
    void plurkAdd();
};

#endif // PLURKVIEW_H
