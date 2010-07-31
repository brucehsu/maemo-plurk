#include "logindialog.h"
#include "ui_logindialog.h"
#include "const.h"
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->checkBox->setVisible(false);
    connect(ui->loginBtn,SIGNAL(clicked()),this,SLOT(loginPlurk()));
    ui->usernameEdit->setInputMethodHints(Qt::ImhNoPredictiveText | Qt::ImhNoAutoUppercase);
    ui->passwdEdit->setInputMethodHints(Qt::ImhNoPredictiveText | Qt::ImhNoAutoUppercase);
    manager = new QNetworkAccessManager();
    pv = 0;
}

LoginDialog::~LoginDialog()
{
    if(pv!=0) delete pv;
    delete manager;
    delete ui;
}

void LoginDialog::loginPlurk() {
    ui->usernameEdit->setEnabled(false);
    ui->passwdEdit->setEnabled(false);
    ui->checkBox->setEnabled(false);
    ui->loginBtn->setEnabled(false);

    QString loginUrl = (APIURL_SSL + USERS_LOGIN + "api_key=" + APIKEY)
                       + "&username=" + ui->usernameEdit->text() +
                       "&password=" + ui->passwdEdit->text();
    rep = manager->get(QNetworkRequest(QUrl(loginUrl)));
    connect(rep,SIGNAL(finished()),this,SLOT(loginFinished()));
}

void LoginDialog::loginFinished() {
    disconnect(rep,SIGNAL(finished()),this,SLOT(loginFinished()));
    cookie = new QVariant(rep->header(QNetworkRequest::SetCookieHeader));
    QByteArray a = rep->readAll();
    if(!(QString(a).contains("error_text"))) {
        QJson::Parser parser;
        bool ok;
        QVariantMap result = parser.parse(a,&ok).toMap();
        QVariantMap user_info = result["user_info"].toMap();
        QString userName = user_info["display_name"].toString();
        if(userName.isEmpty()) userName = user_info["nick_name"].toString();
        pv = new PlurkView();
        pv->setUserId(user_info["id"].toString());
        pv->setUserName(userName);
        pv->setCookie(cookie);
        pv->setNetwork(manager);
        pv->getPlurks();
        this->setVisible(false);
    } else {
        ui->usernameEdit->setEnabled(true);
        ui->passwdEdit->setEnabled(true);
        ui->checkBox->setEnabled(true);
        ui->loginBtn->setEnabled(true);
    }
}
