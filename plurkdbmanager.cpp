#include "plurkdbmanager.h"
#include <QDebug>

PlurkDbManager::PlurkDbManager()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("plurks.db");
    if(!db.open()) {
        QMessageBox::critical(0, QObject::trUtf8("Cannot open database"),
                              QObject::trUtf8("Unable to establish a database connection."), QMessageBox::Cancel);
        return;
    }

    QSqlQuery query;
    QString queryDummy = "";
    //Create table for plurks
    query.exec(queryDummy
               + "CREATE TABLE plurks("
               + "id INTEGER PRIMARY KEY ASC,"
               + "plurk_id UNIQUE,"
               + "plurk_type,"
               + "owner_id,"
               + "content,"
               + "is_unread,"
               + "favorite,"
               + "qualifier_translated,"
               + "responses_seen,"
               + "response_count,"
               + "posted)");

    //Create table for responses
    query.exec(queryDummy
               + "CREATE TABLE responses("
               + "id INTEGER PRIMARY KEY ASC,"
               + "plurk_id,"
               + "res_id,"
               + "user_id,"
               + "posted,"
               + "content)");

    //Create table for users that have posted or responsed
    query.exec(queryDummy
               + "CREATE TABLE users("
               + "id INTEGER PRIMARY KEY ASC,"
               + "user_id UNIQUE,"
               + "nick_name,"
               + "display_name,"
               + "has_profile_image,"
               + "avatar,"
               + "is_friend)");
}

PlurkDbManager::~PlurkDbManager() {
    db.removeDatabase("plurks.db");
    db.close();
}

void PlurkDbManager::addPlurk(QString plurk_id, QString plurk_type,
                              QString owner_id, QString content,
                              QString is_unread, QString favorite,
                              QString qual_trans, QString res_seen,
                              QString res_cnt, QString posted) {
    QSqlQuery query;
    QString dummy;
    query.exec("SELECT * FROM plurks WHERE plurk_id='" + plurk_id + "'");
    if(query.next()) {
        //Update record
        query.prepare(dummy + "UPDATE plurks SET plurk_type=:plurk_type,"
                      + "content=:content,"
                      + "is_unread=:is_unread,"
                      + "favorite=:favorite,"
                      + "reponses_seen=:res_seen,"
                      + "response_cnt=:res_cnt"
                      + " WHERE plurk_id=:plurk_id");
        query.bindValue(":plurk_type",plurk_type);
        query.bindValue(":content",content);
        query.bindValue(":is_unread",is_unread);
        query.bindValue(":favorite",favorite);
        query.bindValue(":res_seen",res_seen);
        query.bindValue(":res_cnt",res_cnt);
        query.bindValue(":plurk_id",plurk_id);
        query.exec();
    } else {
        //Add record
        query.prepare(dummy + "INSERT INTO plurks("
                      + "plurk_id,"
                      + "plurk_type,"
                      + "owner_id,"
                      + "content,"
                      + "is_unread,"
                      + "favorite,"
                      + "qualifier_translated,"
                      + "responses_seen,"
                      + "response_count,"
                      + "posted) VALUES("
                      + ":plurk_id,"
                      + ":plurk_type,"
                      + ":owner_id,"
                      + ":content,"
                      + ":is_unread,"
                      + ":favorite,"
                      + ":qual_trans,"
                      + ":res_seen,"
                      + ":res_cnt,"
                      + ":posted)");
        query.bindValue(":plurk_id",plurk_id);
        query.bindValue(":plurk_type",plurk_type);
        query.bindValue(":owner_id",owner_id);
        query.bindValue(":content",content);
        query.bindValue(":is_unread",is_unread);
        query.bindValue(":favorite",favorite);
        query.bindValue(":qual_trans",qual_trans);
        query.bindValue(":res_seen",res_seen);
        query.bindValue(":res_cnt",res_cnt);
        query.bindValue(":posted",posted);
        query.exec();
    }
}

void PlurkDbManager::addResponse(QString plurk_id, QString res_id,
                                 QString user_id, QString content,
                                 QString posted) {
    QSqlQuery query;
    query.exec("SELECT * FROM response WHERE res_id='" + res_id + "'");
    QString dummy;
    if(query.next()) {
        //No need to update
    } else {
        //Add record
        query.prepare(dummy + "INSERT INTO response(" +
                      + "plurk_id,"
                      + "res_id,"
                      + "user_id,"
                      + "content,"
                      + "posted) VALUES("
                      + ":plurk_id,"
                      + ":res_id,"
                      + ":user_id,"
                      + ":content,"
                      + ":posted)");
        query.bindValue(":plurk_id",plurk_id);
        query.bindValue(":res_id",res_id);
        query.bindValue(":user_id",user_id);
        query.bindValue(":content",content);
        query.bindValue(":posted",posted);
        query.exec();
    }

}

void PlurkDbManager::addUser(QString user_id, QString nick_name,
                             QString display_name, QString profile,
                             QString avatar) {
    QSqlQuery query;
    query.exec("SELECT * FROM users WHERE user_id='" + user_id + "'");
    QString dummy;
    if(query.next()) {
        //Update record
        query.prepare(dummy + "UPDATE users SET display_name=:display_name,"
                      + "has_profile_image=:profie,"
                      + "avatar=:avatar"
                      + " WHERE user_id=:user_id");
        query.bindValue(":display_name",display_name);
        query.bindValue(":profile",profile);
        query.bindValue(":avatar",avatar);
        query.bindValue(":user_id",user_id);
        query.exec();
    } else {
        //Add record
        query.prepare(dummy + "INSERT INTO users(" +
                      + "user_id,"
                      + "nick_name,"
                      + "display_name,"
                      + "has_profile_image,"
                      + "avatar,"
                      + "is_friend"
                      + ") VALUES("
                      + ":user_id,"
                      + ":nick_name,"
                      + ":display_name,"
                      + ":profile,"
                      + ":avatar,'0')");
        query.bindValue(":user_id",user_id);
        query.bindValue(":nick_name",nick_name);
        query.bindValue(":display_name",display_name);
        query.bindValue(":profile",profile);
        query.bindValue(":avatar",avatar);
        query.exec();
    }
}

QMap<QString, ItemMap*>* PlurkDbManager::getAllPlurks(QMap<QString, ItemMap*> *oldList) {
    QSqlQuery query;
    QMap<QString, ItemMap*>* list = (oldList==0) ? new QMap<QString, ItemMap*>() : oldList;
    query.exec("SELECT "
               "id,"
               "plurk_id,"
               "plurk_type,"
               "owner_id,"
               "content,"
               "is_unread,"
               "favorite,"
               "qualifier_translated,"
               "responses_seen,"
               "response_count,"
               "posted" " FROM plurks ORDER BY posted ASC");

    QMap<QString,QString> map;
    while(query.next()) {
        map["id"] = query.value(0).toString();
        map["plurk_id"] = query.value(1).toString();
        map["plurk_type"] = query.value(2).toString();
        map["owner_id"] = query.value(3).toString();
        map["content"] = query.value(4).toString();
        map["is_unread"] = query.value(5).toString();
        map["favorite"] = query.value(6).toString();
        map["qualifier_translated"] = query.value(7).toString();
        map["responses_seen"] = query.value(8).toString();
        map["response_count"] = query.value(9).toString();
        map["posted"] = query.value(10).toString();
        list->insert(map["plurk_id"],new QMap<QString,QString>(map));
    }
    return list;
}

QMap<QString, ItemMap*>* PlurkDbManager::getAllUsers(QMap<QString, ItemMap*> *oldList) {
    QSqlQuery query;
    QMap<QString, ItemMap*>* list = (oldList==0) ? new QMap<QString, ItemMap*>() : oldList;
    query.exec("SELECT "
               "id,"
               "user_id,"
               "nick_name,"
               "display_name,"
               "has_profile_image,"
               "avatar"
               " FROM users");

    QMap<QString,QString> map;
    while(query.next()) {
        map["id"] = query.value(0).toString();
        map["user_id"] = query.value(1).toString();
        map["nick_name"] = query.value(2).toString();
        map["display_name"] = query.value(3).toString();
        map["profile"] = query.value(4).toString();
        map["avatar"] = query.value(5).toString();
        list->insert(map["user_id"],new QMap<QString,QString>(map));
    }
    return list;
}

QString PlurkDbManager::getUserNameById(QString userId) {
    QSqlQuery query;
    QString dummy;
    query.exec("SELECT display_name FROM users WHERE user_id='" + userId + "'");
    if(query.next()) {
        return query.value(0).toString();
    } else {
        return QString("");
    }
}

void PlurkDbManager::markAllAsUnread() {
    QSqlQuery query;
    query.exec("UPDATE plurks SET is_unread='0'");
}

QDateTime PlurkDbManager::getLatestPosted() {
    QSqlQuery query;
    query.exec("SELECT posted FROM plurks ORDER BY posted ASC");
    if(query.next()) {
        QDateTime latest;
        latest.setTimeSpec(Qt::UTC);
        latest.setTime_t(query.value(0).toInt());
        return latest;
    } else {
        //Should not happen
    }
}

bool PlurkDbManager::setAsFriend(QString user_id) {
    QSqlQuery query;
    query.exec("SELECT id FROM users WHERE user_id='" + user_id + "'");
    if(query.next()) {
        return query.exec("UPDATE users SET is_friend='1' WHERE user_id='" + user_id + "'");
    }

    //No such user in database
    return false;
}

int PlurkDbManager::getFriendsCount() {
    QSqlQuery query;
    query.exec("SELECT is_friend FROM users WHERE is_friend='1'");
    return query.size();
}
