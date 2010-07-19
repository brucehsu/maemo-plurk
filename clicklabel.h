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

class clickLabel : public QLabel
{
    Q_OBJECT
public:
    explicit clickLabel(QWidget *parent = 0);
    clickLabel(QString text,QString plid);
    ~clickLabel();

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

signals:
    void clicked();
public slots:
    void imgLoaded(QNetworkReply* reply);

};

#endif // CLICKLABEL_H
