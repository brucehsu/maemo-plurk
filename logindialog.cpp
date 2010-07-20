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
    ui->usernameEdit->setCompleter(0);
    ui->passwdEdit->setCompleter(0);
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

    req = new QNetworkRequest(QUrl(QString(APIURL_SSL + USERS_LOGIN + "api_key=" + APIKEY)
                                   + "&username=" + ui->usernameEdit->text() +
                                   "&password=" + ui->passwdEdit->text() + "&no_data=1"));
    rep = manager->get(*req);
    connect(rep,SIGNAL(finished()),this,SLOT(loginFinished()));
}

void LoginDialog::loginFinished() {
    disconnect(rep,SIGNAL(finished()),this,SLOT(loginFinished()));
    cookie = new QVariant(rep->header(QNetworkRequest::SetCookieHeader));
    if(QString(rep->readAll()).contains("success")) {
        pv = new PlurkView();
        pv->setCookie(cookie);
        pv->setNetwork(manager);
        pv->loadPlurks();
        this->setVisible(false);
    } else {
        ui->usernameEdit->setEnabled(true);
        ui->passwdEdit->setEnabled(true);
        ui->checkBox->setEnabled(true);
        ui->loginBtn->setEnabled(true);
    }
}
