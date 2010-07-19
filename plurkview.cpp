#include "plurkview.h"
#include "ui_plurkview.h"
#include <QDebug>

PlurkView::PlurkView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlurkView)
{
    ui->setupUi(this);
    plurkLayout = new QVBoxLayout(ui->plurkListScroll);
    plurkLayout->setMargin(1);

    btnGroup = new QButtonGroup();
    btnGroup->addButton(ui->allPlurkBtn);
    btnGroup->addButton(ui->myPlurkBtn);
    btnGroup->addButton(ui->privateBtn);
    btnGroup->addButton(ui->respondedBtn);
    btnGroup->addButton(ui->unreadBtn);

    //ui->plurkListScroll->setWidget(ui->plurkListWidget);
    ui->plurkListWidget->setLayout(plurkLayout);
    ui->contentEdit->setCompleter(0);
    this->show();
}

PlurkView::~PlurkView()
{
    foreach(clickLabel* label, plurkList) {
        delete label;
    }

    delete ui;
}

void PlurkView::setCookie(QVariant *cookie) {
    this->cookie = cookie;
}

void PlurkView::setNetwork(QNetworkAccessManager *manager) {
    this->manager = manager;
}

void PlurkView::loadPlurks() {
    const QString apiurl = "http://www.plurk.com/API";
    req = new QNetworkRequest(QUrl(apiurl+"/Timeline/getPlurks?api_key=RsNlrQJFiHkPelX8aCMbx48Nq89QnZRB&limit=30"));
    req->setHeader(QNetworkRequest::CookieHeader,*cookie);
    rep = manager->get(*req);
    connect(rep,SIGNAL(finished()),this,SLOT(loadFinished()));
}

void PlurkView::loadFinished() {
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(rep->readAll(),&ok).toMap();
    QList<QVariant> plurks = result["plurks"].toList();
    QVariantMap users = result["plurk_users"].toMap();
    for(int i=0;i<plurks.count();i++) {
        QVariantMap pMap = plurks[i].toMap();
        QVariantMap uMap = users[pMap["owner_id"].toString()].toMap();
        QString owner_name = (uMap["display_name"].toString().isEmpty()) ? uMap["nick_name"].toString() : uMap["display_name"].toString();
        QString qual_trans = pMap["qualifier_translated"].toString();
        QString content = pMap["content"].toString();
        QString whole = owner_name + " " + qual_trans + ": " + content +
                        "<br /><div align=\"right\"><font color=\"gray\">" + pMap["response_count"].toString() +
                        " Responses</font></div>";
        clickLabel* tmpLabel = new clickLabel(whole,pMap["plurk_id"].toString());
        tmpLabel->setWordWrap(true);
        tmpLabel->setOpenExternalLinks(true);
        plurkList.append(tmpLabel);
        plurkLayout->addWidget(tmpLabel);
    }
}
