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

    ui->addIndicator->setVisible(false);

    btnGroup = new QButtonGroup();
    btnGroup->addButton(ui->allPlurkBtn);
    btnGroup->addButton(ui->myPlurkBtn);
    btnGroup->addButton(ui->privateBtn);
    btnGroup->addButton(ui->respondedBtn);
    btnGroup->addButton(ui->likedBtn);
    btnGroup->addButton(ui->unreadBtn);

    //Create directory for storing avatars
    QDir dir;
    dir.mkdir("avatars");

    //Create directory for storing emoticons
    dir.mkdir("emoticons");

    dbManager = 0;
    dbPlurkMap = 0;
    dbUserMap = 0;
    avatarNetworkManager = 0;

    connect(ui->refreshBtn,SIGNAL(clicked()),this,SLOT(getPlurks()));
    connect(ui->allPlurkBtn,SIGNAL(clicked()),this,SLOT(displayAllPlurks()));
    connect(ui->myPlurkBtn,SIGNAL(clicked()),SLOT(displayMyPlurks()));
    connect(ui->privateBtn,SIGNAL(clicked()),SLOT(displayPrivate()));
    connect(ui->respondedBtn,SIGNAL(clicked()),this,SLOT(displayResponded()));
    connect(ui->likedBtn,SIGNAL(clicked()),SLOT(displayLiked()));
    connect(ui->unreadBtn,SIGNAL(clicked()),this,SLOT(displayUnread()));
    connect(ui->plurkBtn,SIGNAL(clicked()),this,SLOT(plurkAdd()));

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
    delete avatarNetworkManager;
    delete ui;
}

void PlurkView::setCookie(QVariant *cookie) {
    this->cookie = cookie;
}

void PlurkView::setNetwork(QNetworkAccessManager *manager) {
    this->networkManager = manager;
}

void PlurkView::refreshQueryMaps() {
    if(dbPlurkMap!=0) delete dbPlurkMap;
    dbPlurkMap = dbManager->getAllPlurks();

    if(dbUserMap!=0) delete dbUserMap;
    dbUserMap = dbManager->getAllUsers();
}

void PlurkView::getPlurks() {
    if(dbManager==0) {
        ui->plurkListWidget->setMinimumWidth(774);
        ui->plurkListWidget->setMaximumWidth(774);
    }
    if(QFile::exists("plurks.db")) {
        if(dbManager==0) {
            dbManager = new PlurkDbManager();
            dbManager->markAllAsUnread();

            loadPlurkFromDb();
        }

        //For new plurks
        QDateTime latest = dbManager->getLatestPosted();
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
    connect(networkManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(plurkRequestFinished(QNetworkReply*)),Qt::UniqueConnection);
}

void PlurkView::getAvatars() {
    if(avatarNetworkManager==0) {
        avatarNetworkManager = new QNetworkAccessManager();
    }

    QMap<QString,QString>* pmap;
    foreach(pmap, (*dbUserMap)) {
        QMap<QString,QString> map = *pmap;

        QString avatarName = map["user_id"] + "-medium"
                             + (map["avatar"]=="0" ? "" : map["avatar"])
                             + ".gif";
        QString avatarPath = "avatars/" + avatarName;

        if(!QFile::exists(avatarPath)) {
            if(map["profile"]!="1") {
                avatarNetworkManager->get(QNetworkRequest(QUrl(DEFAULT_AVATAR_MED)));
            } else {
               avatarNetworkManager->get(QNetworkRequest(QUrl(AVATAR_URL + avatarName)));
            }
            connect(avatarNetworkManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(getAvatarsFinished(QNetworkReply*)));
        }
    }
}

void PlurkView::plurkRequestFinished(QNetworkReply* reply) {
    reply->deleteLater();
    QByteArray a = reply->readAll();
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(a,&ok).toMap();
    if(!ok) return;

    //Handle error returned by the server
    if(result.contains("error_text")) {
        QString error = result["error_text"].toString();
        qDebug() << error << endl;
        if(error=="anti-flood-same-content") {

        } else if(error=="anti-flood-too-many-new") {

        }
        return;
    }

    QLocale locale(QLocale::English, QLocale::UnitedStates);

    if(reply->url().path().contains("plurkAdd")) {
        //Only add the plurk user just posted
        QString plurk_id = result["plurk_id"].toString();
        QString plurk_type = result["plurk_type"].toString();
        QString owner_id = result["owner_id"].toString();
        QString content = result["content"].toString();
        QString is_unread = result["is_unread"].toString();
        QString favorite = result["favorite"].toString();
        QString qual_trans = result["qualifier_translated"].toString();
        QString res_seen = result["responses_seen"].toString();
        QString res_cnt = result["response_count"].toString();
        QString posted = result["posted"].toString().remove(0,5);
        posted.chop(4);

        //Convert formatted time into seconds
        //QDateTime t = QDateTime::fromString(posted,"dd MMM yyyy HH:mm:ss");
        QDateTime t = locale.toDateTime(posted,"dd MMM yyyy HH:mm:ss");
        t.setTimeSpec(Qt::UTC);

        dbManager->addPlurk(plurk_id,plurk_type,owner_id,content,
                            is_unread,favorite,qual_trans,res_seen,
                            res_cnt,QString::number(t.toTime_t()));
        loadPlurkFromDb();
        return;
    }

    //Process the data returned by calling getPlurks and getUnreadPlurks API
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
        //QDateTime t = QDateTime::fromString(posted,"dd MMM yyyy HH:mm:ss");
        QDateTime t = locale.toDateTime(posted,"dd MMM yyyy HH:mm:ss");
        t.setTimeSpec(Qt::UTC);

        dbManager->addPlurk(plurk_id,plurk_type,owner_id,content,
                            is_unread,favorite,qual_trans,res_seen,
                            res_cnt,QString::number(t.toTime_t()));
        dbManager->addUser(owner_id,uMap["nick_name"].toString(),owner_name,
                           uMap["has_profile_image"].toString(),
                           uMap["avatar"].toString());
    }

    loadPlurkFromDb();
    getAvatars();
}

void PlurkView::getAvatarsFinished(QNetworkReply *reply) {
    reply->deleteLater();
    QString url = reply->url().toString();
    QString filename = url.right(url.length()-url.lastIndexOf("/") - 1);
    filename.replace("_","-");

    if(QFile::exists("avatars/" + filename)) {
        return;
    }

    QFile *local = new QFile("avatars/" + filename);
    local->open(QIODevice::WriteOnly);
    local->write(reply->readAll());
    local->flush();
    local->close();
    delete local;

    refreshPlurkLabels();
}


void PlurkView::refreshPlurkLabels() {
    foreach(ClickLabel* label, plurkMap) {
        label->refreshText(label->text());
    }
}

void PlurkView::addPlurkLabel(QString plurk_id) {
    ItemMap pMap = *(*dbPlurkMap)[plurk_id];
    QString owner_id = pMap["owner_id"];
    QString qual_trans = pMap["qualifier_translated"];
    QString content = pMap["content"];
    QString res_cnt = pMap["response_count"];
    ItemMap uMap = *(*dbUserMap)[owner_id];
    QString owner_name = uMap["display_name"];
    QString owner_image = uMap["profile"];
    QString owner_avatar = uMap["avatar"];

    QDateTime posted;
    posted.setTimeSpec(Qt::UTC);
    posted.setTime_t(pMap["posted"].toInt());
    posted = posted.toLocalTime();

    QString whole = "<table><tr><td height=\"45\" width=\"45\"><img "
                    "height=\"45\" width=\"45\" "
                    "name=\"avatar\" src=\"avatars/"
                    + (owner_image=="1" ? owner_id : "default")
                    + "-medium"
                    + (owner_avatar=="0" ? "" : owner_avatar)
                    + ".gif\"></img></td><td>";
    whole = whole + owner_name + " " + qual_trans + ": " + content +
            "</td></tr></table><font color=\"gray\">";
    whole = whole + "<table width=\"760\"><tr>";
    whole = whole + "<td align=\"left\">"
            + posted.toString("yyyy-MM-dd hh:mm:ss")
            + "</td>";
    whole = whole + "<td align=\"right\">"
           + res_cnt + " Responses";
    if(pMap["favorite"]=="true") {
        //Display favorite icon
        whole = whole + "&nbsp<img src=\":/fav.png\"></img>";
    }
    if(pMap["plurk_type"]=="1"||pMap["plurk_type"]=="3") {
        //Display private icon
        whole = whole + "&nbsp<img src=\":/private.png\"></img>";
    }
    whole = whole + "</td></tr></table></font>";

    if(plurkMap.contains(plurk_id)) {
        //Edit existing label
        plurkMap[plurk_id]->refreshText(whole);
        return;
    }

    //Add new label
    ClickLabel* tmpLabel = new ClickLabel(whole,plurk_id);
    tmpLabel->setWordWrap(true);
    tmpLabel->setOpenExternalLinks(true);
    tmpLabel->setObjectName(plurk_id);
    plurkMap[plurk_id] = tmpLabel;

    //Find proper position to insert
    for(int cnt=plurkLayout->count(),i=0;i<cnt;i++) {
        QLayoutItem *item = plurkLayout->itemAt(i);
        ClickLabel *iLab = (ClickLabel*)(item->widget());
        ItemMap iMap = *(*dbPlurkMap)[iLab->objectName()];
        if(iMap["posted"].toInt()>pMap["posted"].toInt()) continue;
        plurkLayout->insertWidget(i,tmpLabel);
        return;
    }

    plurkLayout->insertWidget(plurkLayout->count(),tmpLabel);
}

void PlurkView::setUserId(QString id) {
    this->userId = id;
}

void PlurkView::setUserName(QString name) {
    this->userName = name;
}

void PlurkView::displayAllPlurks() {
    foreach(ClickLabel* label, plurkMap) {
        label->setVisible(true);
    }
}

void PlurkView::displayMyPlurks() {
    QMap<QString,QString>* pmap;
    foreach(pmap, *dbPlurkMap) {
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
    foreach(pmap, *dbPlurkMap) {
        QMap<QString,QString> map = (*pmap);
        if(map["plurk_type"]=="1" || map["plurk_type"]=="3") {
            plurkMap[map["plurk_id"]]->setVisible(true);
        } else {
            plurkMap[map["plurk_id"]]->setVisible(false);
        }
    }
}

void PlurkView::displayResponded() {
    QMap<QString,QString>* pmap;
    foreach(pmap, *dbPlurkMap) {
        QMap<QString,QString> map = (*pmap);
        if(map["plurk_type"]=="2"||map["plurk_type"]=="3") {
            plurkMap[map["plurk_id"]]->setVisible(true);
        } else {
            plurkMap[map["plurk_id"]]->setVisible(false);
        }
    }
}

void PlurkView::displayLiked() {
    QMap<QString,QString>* pmap;
    foreach(pmap, *dbPlurkMap) {
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
    foreach(pmap, *dbPlurkMap) {
        QMap<QString,QString> map = (*pmap);
        if(map["is_unread"]=="1") {
            plurkMap[map["plurk_id"]]->setVisible(true);
        } else {
            plurkMap[map["plurk_id"]]->setVisible(false);
        }
    }
}

void PlurkView::loadPlurkFromDb() {
    refreshQueryMaps();

    QMap<QString,QString>* map;
    foreach(map, (*dbPlurkMap)) {
        QMap<QString,QString> tmpMap = *map;
        QMap<QString,QString> uMap = *(dbUserMap->value(tmpMap["owner_id"]));
        addPlurkLabel(tmpMap["plurk_id"]);
    }
}

void PlurkView::plurkAdd() {
    QString url = APIURL + TIMELINE_PLURK_ADD + "api_key=" + APIKEY;
    url = url + "&content=" + ui->contentEdit->text();
    url = url + "&qualifier=" + ui->qualifierCombo->currentText();
    networkManager->get(QNetworkRequest(QUrl(url)));
    connect(networkManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(plurkRequestFinished(QNetworkReply*)),Qt::UniqueConnection);

    ui->contentEdit->clear();
}
