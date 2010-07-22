#include "clicklabel.h"
#include <QDebug>

const QString border = "border: 2px solid white;border-radius: 4px";

ClickLabel::ClickLabel(QWidget *parent) :
    QLabel(parent)
{
    this->setStyleSheet(border);
}

ClickLabel::ClickLabel(QString text,QString plid) {
    this->plid = plid;
    this->imgCount = 0;

    //Download source images in <img> tags.
    manager = new QNetworkAccessManager();
    page = new QWebPage();
    page->mainFrame()->setHtml(text);
    eles = page->mainFrame()->findAllElements("img");
    foreach (QWebElement ele, eles) {
        QString src = ele.attribute("src");
        req = new QNetworkRequest(QUrl(src));
        manager->get(*req);
        connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(imgLoaded(QNetworkReply*)));
    }

    this->setText(page->mainFrame()->toHtml());
    this->setStyleSheet(border);
}

ClickLabel::~ClickLabel() {
    //Delete all temporary image files
    foreach(QString lname, imgList) {
        QFile *local = new QFile(lname);
        local->remove();
    }

    delete page;
}

void ClickLabel::mousePressEvent(QMouseEvent *ev) {
    this->setStyleSheet(border + "; background-color: blue");
    QLabel::mousePressEvent(ev);
}

void ClickLabel::mouseReleaseEvent( QMouseEvent * ev ) {
    this->setStyleSheet(border);
    emit clicked();
    QLabel::mouseReleaseEvent(ev);
}

void ClickLabel::imgLoaded(QNetworkReply *reply) {
    //Write downloaded image to filesystem
    QString imgUrl = reply->url().toString();
    QString imgType = imgUrl.right(imgUrl.length() - imgUrl.lastIndexOf('.') - 1);
    QString localUri = plid + "_" + QString::number(++imgCount) + imgType;
    imgList << localUri;
    QFile *local = new QFile(localUri);
    local->open(QIODevice::WriteOnly);
    local->write(reply->readAll());
    local->flush();
    local->close();

    //Replace url in <img> tag
    foreach(QWebElement ele, eles) {
        if(ele.attribute("src")==imgUrl) {
            ele.setAttribute("src",localUri);
            this->setText(page->mainFrame()->toHtml());
            break;
        }
    }
}
