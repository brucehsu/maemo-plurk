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
#include "responsesview.h"

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
    void setUserInfo(QVariantMap info);

private:
    Ui::PlurkView *ui;
    QNetworkAccessManager *networkManager, *avatarNetworkManager;
    QNetworkRequest req;
    QNetworkReply *rep;
    QVariant *cookie;
    QString userId;
    QString userName;
    QVariantMap userInfo;
    QMap<QString,ClickLabel*> plurkMap;
    QMap<QString, ItemMap*>* dbPlurkMap;
    QMap<QString, ItemMap*>* dbUserMap;
    QVBoxLayout *plurkLayout;
    QButtonGroup *btnGroup;
    PlurkDbManager *dbManager;
    ResponsesView *resView;
    void addPlurkLabel(QString plurk_id);
    void loadPlurkFromDb();
    void getAvatars();
    void refreshQueryMaps();
    void refreshPlurkLabels();

public slots:
    void plurkRequestFinished(QNetworkReply* reply);
    void getAvatarsFinished(QNetworkReply* reply);
    void countCharacters(QString content);
    void getPlurks(QVariantMap *plurksMap=0);
    void displayAllPlurks();
    void displayMyPlurks();
    void displayPrivate();
    void displayResponded();
    void displayLiked();
    void displayUnread();
    void plurkAdd();
    void showResponses(QString plid);
};

#endif // PLURKVIEW_H
