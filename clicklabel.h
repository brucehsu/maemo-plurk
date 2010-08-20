#ifndef CLICKLABEL_H
#define CLICKLABEL_H

#include <QLabel>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebElementCollection>

class ClickLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickLabel(QWidget *parent = 0);
    ClickLabel(QString text,QString plid);
    ~ClickLabel();
    void refreshText(QString text);

protected:
    void mousePressEvent (QMouseEvent *ev);
    void mouseReleaseEvent ( QMouseEvent * ev );

private:
    QNetworkAccessManager *manager;
    QNetworkRequest *req;
    QString plid;
    int imgCount;
    QWebPage *page;
    QWebElementCollection eles;
    QStringList imgList;
    void deleteImages();

signals:
    void clicked(QString);
public slots:
    void imgLoaded(QNetworkReply* reply);

};

#endif // CLICKLABEL_H
