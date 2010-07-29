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

    manager = new QNetworkAccessManager();
    page = new QWebPage();
    this->setStyleSheet(border);

    refreshText(text);
}

void ClickLabel::refreshText(QString text) {
    deleteImages();

    //Download source images in <img> tags.
    page->mainFrame()->setHtml(text);
    eles = page->mainFrame()->findAllElements("img");
    foreach (QWebElement ele, eles) {
        QString src = ele.attribute("src");
        QString filename = src.right(src.length()-src.lastIndexOf("/") - 1);
        if(!src.startsWith("http://")&&!src.startsWith("https://")) {
            //Ignore local urls
            continue;
        }
        if(ele.attribute("name")=="avatar") {
            continue;
        }
        manager->get(QNetworkRequest(QUrl(src)));
        connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(imgLoaded(QNetworkReply*)));
    }

    this->setText(page->mainFrame()->toHtml());
    this->repaint();
}

void ClickLabel::deleteImages() {
    this->imgCount = 0;

    //Delete all temporary image files
    foreach(QString lname, imgList) {
        QFile *local = new QFile(lname);
        local->remove();
        delete local;
    }
    imgList.clear();
}

ClickLabel::~ClickLabel() {
    deleteImages();

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
    if(reply->error()!=QNetworkReply::NoError) {
        //Stop loading when file doesn't exist
        if(reply->error()==QNetworkReply::ContentNotFoundError) {
            return;
        }

        //Reload image
        manager->get(QNetworkRequest(reply->url()));
        connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(imgLoaded(QNetworkReply*)));
        return;
    }

    QString imgUrl = reply->url().toString();
    QString imgType = imgUrl.right(imgUrl.length() - imgUrl.lastIndexOf('.') - 1);
    QString localUri;

    //Replace url in <img> tag
    foreach(QWebElement ele, eles) {
        if(ele.attribute("src")==imgUrl) {
            if(ele.attribute("name")=="avatar") {
                QString owner_id = reply->url().toString();
                owner_id = owner_id.right(owner_id.length()-owner_id.lastIndexOf("/") - 1);
                localUri = owner_id;
            } else {
                localUri = plid + "_" + QString::number(++imgCount) + imgType;
            }
            //Write downloaded image to filesystem
            imgList << localUri;
            QFile *local = new QFile(localUri);
            local->open(QIODevice::WriteOnly);
            local->write(reply->readAll());
            local->flush();
            local->close();
            delete local;

            ele.setAttribute("src",localUri);
            this->setText(page->mainFrame()->toHtml());
            break;
        }
    }
}
