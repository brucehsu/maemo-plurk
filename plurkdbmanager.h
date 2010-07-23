#ifndef PLURKDBMANAGER_H
#define PLURKDBMANAGER_H
#include <QMessageBox>
#include <QObject>
#include <QVariant>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

class PlurkDbManager
{
public:
    PlurkDbManager();
    ~PlurkDbManager();
    void addPlurk(QString plurk_id, QString plurk_type, QString owner_id,
                   QString content, QString is_unread, QString favorite,
                   QString qual_trans, QString res_seen,QString res_cnt,
                   QString posted);
    void addResponse(QString plurk_id, QString res_id, QString user_id, QString content,
                     QString posted);
    void addUser(QString user_id, QString nick_name, QString display_name);
    QList<QMap<QString,QString>*>* getAllPlurks();
    QList<QMap<QString,QString>*>* getAllUsers();
    QString getUserNameById(QString userId);
    void markAllAsUnread();

private:
    QSqlDatabase db;
};

#endif // PLURKDBMANAGER_H
