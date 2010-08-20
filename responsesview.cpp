#include "responsesview.h"
#include "ui_responsesview.h"
#include <QDebug>

ResponsesView::ResponsesView(QWidget *parent) :
    ui(new Ui::ResponsesView)
{
    manager = new QNetworkAccessManager();
    plurkLayout = new QVBoxLayout();
    plurkLabel = 0;

    ui->setupUi(this);
    ui->plurkFrame->setLayout(plurkLayout);
    plurkLayout->setMargin(4);
    this->setVisible(false);
    this->parentWidget = parent;
}

ResponsesView::~ResponsesView()
{
    delete ui;
    delete manager;
    if(plurkLabel!=0) delete plurkLabel;
}

void ResponsesView::setMaps(ItemMap *plurkItemMap,
                            QMap<QString, ItemMap *> *userMap) {
    this->plurkItemMap = plurkItemMap;
    this->userMap = userMap;
    this->refreshView();
}

void ResponsesView::setDbManager(PlurkDbManager *dbManager) {
    this->dbManager = dbManager;
}

void ResponsesView::refreshView() {
    if(plurkLabel!=0) {
        plurkLayout->removeWidget(plurkLabel);
        delete plurkLabel;
    }
    ItemMap pMap = *(plurkItemMap);
    QString owner_id = pMap["owner_id"];
    QString qual_trans = pMap["qualifier_translated"];
    QString content = pMap["content"];
    QString res_cnt = pMap["response_count"];
    ItemMap uMap = *(*userMap)[owner_id];
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
    whole = whole + "<div align=\"right\">"
            + posted.toString("yyyy-MM-dd hh:mm:ss");
    if(pMap["favorite"]=="true") {
        //Display favorite icon
        whole = whole + "&nbsp<img src=\":/fav.png\"></img>";
    }
    if(pMap["plurk_type"]=="1"||pMap["plurk_type"]=="3") {
        //Display private icon
        whole = whole + "&nbsp<img src=\":/private.png\"></img>";
    }
    whole = whole + "</div></font>";

    plurkLabel = new ClickLabel(whole,"res");
    plurkLabel->setWordWrap(true);
    ui->plurkFrame->setMaximumWidth(782);
    plurkLabel->setMaximumWidth(780);
    plurkLayout->addWidget(plurkLabel);
}

void ResponsesView::closeEvent(QCloseEvent *ev) {
    if(this->isActiveWindow() || !this->isVisible()) {
        ev->ignore();
        this->setVisible(false);
    } else ev->accept();
}
