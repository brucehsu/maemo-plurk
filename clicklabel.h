#ifndef CLICKLABEL_H
#define CLICKLABEL_H

#include <QLabel>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

class clickLabel : public QLabel
{
    Q_OBJECT
public:
    explicit clickLabel(QWidget *parent = 0);
    clickLabel(QString text);

protected:
    void mousePressEvent (QMouseEvent *ev);
    void mouseReleaseEvent ( QMouseEvent * ev );

private:

signals:
    void clicked();
public slots:

};

#endif // CLICKLABEL_H
