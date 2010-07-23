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

    dbManager = 0;
    dbList = 0;

    connect(ui->refreshBtn,SIGNAL(clicked()),this,SLOT(loadPlurks()));
    connect(ui->allPlurkBtn,SIGNAL(clicked()),this,SLOT(displayAllPlurks()));
    connect(ui->myPlurkBtn,SIGNAL(clicked()),SLOT(displayMyPlurks()));
    connect(ui->privateBtn,SIGNAL(clicked()),SLOT(displayPrivate()));
    connect(ui->respondedBtn,SIGNAL(clicked()),this,SLOT(displayResponded()));
    connect(ui->likedBtn,SIGNAL(clicked()),SLOT(displayLiked()));
    connect(ui->unreadBtn,SIGNAL(clicked()),this,SLOT(displayUnread()));

    //ui->plurkListScroll->setWidget(ui->plurkListWidget);
    ui->plurkListWidget->setLayout(plurkLayout);
    ui->contentEdit->setCompleter(0);
    this->show();
}

PlurkView::~PlurkView()
{
    foreach(ClickLabel* label, plurkMap) {
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
    if(dbManager==0) {
        ui->plurkListWidget->setMinimumWidth(774);
        ui->plurkListWidget->setMaximumWidth(774);
    }
    if(QFile::exists("plurks.db")) {
        QDateTime latest;
        if(dbManager==0) {
            dbManager = new PlurkDbManager();
            dbManager->markAllAsUnread();
            if(dbList!=0) delete dbList;
            dbList = dbManager->getAllPlurks();
            QMap<QString,QString>* map;
            foreach(map, (*dbList)) {
                QMap<QString,QString> tmpMap = *map;
                latest.setTime_t(tmpMap["posted"].toInt());
                addPlurkLabel(tmpMap["plurk_id"],dbManager->getUserNameById(tmpMap["owner_id"]),
                              tmpMap["qualifier_trasnlated"],tmpMap["content"],tmpMap["response_count"]);
            }
        }

        //For new plurks
        QString offset = latest.toString("yyyy-MM-ddThh:mm:ss");
        req = new QNetworkRequest(QUrl(APIURL+ POLL_GET_PLURKS +"api_key=" + APIKEY + "&offset=" + offset));
        req->setHeader(QNetworkRequest::CookieHeader,*cookie);
        rep = networkManager->get(*req);

        //For unread plurks
        req = new QNetworkRequest(QUrl(APIURL+ TIMELINE_GET_UNREAD +"api_key=" + APIKEY));
    } else {
        req = new QNetworkRequest(QUrl(APIURL+ TIMELINE_GET_PLURKS +"api_key=" + APIKEY +"&limit=30"));
    }
    if(dbManager==0) {
        dbManager = new PlurkDbManager();
    }
    req->setHeader(QNetworkRequest::CookieHeader,*cookie);
    rep = networkManager->get(*req);
    connect(networkManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(loadFinished(QNetworkReply*)));
}

void PlurkView::loadFinished(QNetworkReply* reply) {
    QByteArray a = reply->readAll();
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(a,&ok).toMap();
    if(!ok) return;
    QList<QVariant> plurks = result["plurks"].toList();
    QVariantMap users = result["plurk_users"].toMap();

    for(int i=plurks.count()-1;i>=0;i--) {
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
        QString posted = pMap["posted"].toString().remove(0,5);
        QString owner_name = (uMap["display_name"].toString().isEmpty()) ? uMap["nick_name"].toString() : uMap["display_name"].toString();
        posted.chop(4);

        //Convert formatted time into seconds
        QDateTime t = QDateTime::fromString(posted,"dd MMM yyyy HH:mm:ss");

        dbManager->addPlurk(plurk_id,plurk_type,owner_id,content,
                            is_unread,favorite,qual_trans,res_seen,
                            res_cnt,QString::number(t.toTime_t()));
        dbManager->addUser(owner_id,uMap["nick_name"].toString(),owner_name);
        addPlurkLabel(plurk_id,owner_name,qual_trans,content,res_cnt);
    }


    if(dbList!=0) delete dbList;
    dbList = dbManager->getAllPlurks();
}

void PlurkView::addPlurkLabel(QString plurk_id, QString owner_name, QString qual_trans,
                   QString content, QString res_cnt) {

    QString whole = owner_name + " " + qual_trans + ": " + content +
                    "<br /><div align=\"right\"><font color=\"gray\">" + res_cnt +
                    " Responses</font></div>";

    if(plurkMap.contains(plurk_id)) {
        //Edit existing label
        plurkMap[plurk_id]->setText(whole);
        return;
    }

    //Add new label
    ClickLabel* tmpLabel = new ClickLabel(whole,plurk_id);
    tmpLabel->setWordWrap(true);
    tmpLabel->setOpenExternalLinks(true);
    plurkMap[plurk_id] = tmpLabel;
    plurkLayout->insertWidget(0,tmpLabel);
}

void PlurkView::setUserId(QString id) {
    this->userId = id;
}

void PlurkView::displayAllPlurks() {
    foreach(ClickLabel* label, plurkMap) {
        label->setVisible(true);
    }
}

void PlurkView::displayMyPlurks() {
    QMap<QString,QString>* pmap;
    foreach(pmap, *dbList) {
        QMap<QString,QString> map = (*pmap);
        if(map["owner_id"]==this->userId) {
            plurkMap[map["plurk_id"]]->setVisible(true);
        } else {
            plurkMap[map["plurk_id"]]->setVisible(false);
        }
    }
}

void PlurkView::displayPrivate() {
    QMap<QString,QString>* pmap;
    foreach(pmap, *dbList) {
        QMap<QString,QString> map = (*pmap);
        if(map["plurk_type"]=="1" || map["plurk_type"]=="3") {
            plurkMap[map["plurk_id"]]->setVisible(true);
        } else {
            plurkMap[map["plurk_id"]]->setVisible(false);
        }
    }
}

void PlurkView::displayResponded() {

}

void PlurkView::displayLiked() {
    QMap<QString,QString>* pmap;
    foreach(pmap, *dbList) {
        QMap<QString,QString> map = (*pmap);
        if(map["favorite"]=="true") {
            plurkMap[map["plurk_id"]]->setVisible(true);
        } else {
            plurkMap[map["plurk_id"]]->setVisible(false);
        }
    }
}

void PlurkView::displayUnread() {
    QMap<QString,QString>* pmap;
    foreach(pmap, *dbList) {
        QMap<QString,QString> map = (*pmap);
        if(map["is_unread"]=="1") {
            plurkMap[map["plurk_id"]]->setVisible(true);
        } else {
            plurkMap[map["plurk_id"]]->setVisible(false);
        }
    }
}
