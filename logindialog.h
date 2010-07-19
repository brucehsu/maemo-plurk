#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkCookieJar>
#include "plurkview.h"

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QWidget
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private:
    Ui::LoginDialog *ui;
    QNetworkAccessManager *manager;
    QNetworkRequest *req;
    QNetworkReply *rep;
    QVariant *cookie;
    PlurkView *pv;

public slots:
    void loginPlurk();
    void loginFinished();
};

#endif // LOGINDIALOG_H
