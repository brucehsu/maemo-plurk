#include "plurkview.h"
#include "ui_plurkview.h"
#include "const.h"
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
    btnGroup->addButton(ui->likedBtn);
    btnGroup->addButton(ui->unreadBtn);

    //ui->plurkListScroll->setWidget(ui->plurkListWidget);
    ui->plurkListWidget->setLayout(plurkLayout);
    ui->contentEdit->setCompleter(0);
    this->show();
}

PlurkView::~PlurkView()
{
    foreach(ClickLabel* label, plurkList) {
        delete label;
    }

    delete plurkLayout;
    delete btnGroup;
    delete dbManager;
    delete ui;
}

void PlurkView::setCookie(QVariant *cookie) {
    this->cookie = cookie;
}

void PlurkView::setNetwork(QNetworkAccessManager *manager) {
    this->networkManager = manager;
}

void PlurkView::loadPlurks() {
    if(QFile::exists("plurks.db")) {
        req = new QNetworkRequest(QUrl(APIURL+ POLL_GET_PLURKS +"api_key=" + APIKEY +"&limit=30"));
    } else {
        req = new QNetworkRequest(QUrl(APIURL+ TIMELINE_GET_PLURKS +"api_key=" + APIKEY +"&limit=30"));
    }
    dbManager = new PlurkDbManager();
    req->setHeader(QNetworkRequest::CookieHeader,*cookie);
    rep = networkManager->get(*req);
    connect(rep,SIGNAL(finished()),this,SLOT(loadFinished()));
}

void PlurkView::loadFinished() {
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(rep->readAll(),&ok).toMap();
    if(!ok) return;
    QList<QVariant> plurks = result["plurks"].toList();
    QVariantMap users = result["plurk_users"].toMap();

    for(int i=0;i<plurks.count();i++) {
        QVariantMap pMap = plurks[i].toMap();
        QVariantMap uMap = users[pMap["owner_id"].toString()].toMap();
        QString plurk_id = pMap["plurk_id"].toString();
        QString plurk_type = pMap["plurk_type"].toString();
        QString owner_id = pMap["owner_id"].toString();
        QString content = pMap["content"].toString();
        QString is_unread = pMap["is_unread"].toString();
        QString favorite = pMap["favorite"].toString();
        QString qual_trans = pMap["qualifier_translated"].toString();
        QString res_seen = pMap["responses_seen"].toString();
        QString res_cnt = pMap["response_count"].toString();
        QString posted = pMap["posted"].toString();
        QString owner_name = (uMap["display_name"].toString().isEmpty()) ? uMap["nick_name"].toString() : uMap["display_name"].toString();

        dbManager->addPlurk(plurk_id,plurk_type,owner_id,content,
                            is_unread,favorite,qual_trans,res_seen,
                            res_cnt,posted);
        addPlurkLabel(plurk_id,owner_name,qual_trans,content,res_cnt);
    }
}

void PlurkView::addPlurkLabel(QString plurk_id, QString owner_name, QString qual_trans,
                   QString content, QString res_cnt) {
    QString whole = owner_name + " " + qual_trans + ": " + content +
                    "<br /><div align=\"right\"><font color=\"gray\">" + res_cnt +
                    " Responses</font></div>";

    ClickLabel* tmpLabel = new ClickLabel(whole,plurk_id);
    tmpLabel->setWordWrap(true);
    tmpLabel->setOpenExternalLinks(true);
    plurkList.append(tmpLabel);
    plurkLayout->addWidget(tmpLabel);
}
