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
               + "user_id,"
               + "posted,"
               + "content)");

    //Create table for users that have posted or responsed
    query.exec(queryDummy
               + "CREATE TABLE users("
               + "id INTEGER PRIMARY KEY ASC,"
               + "user_id UNIQUE,"
               + "nick_name,"
               + "display_name)");
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
    query.exec("SELECT * FROM plurks WHERE plurk_id='" + plurk_id + "'");
    if(query.next()) {
        //Update record
        query.exec("UPDATE plurks SET plurk_type='" + plurk_type + "',"
                   + "content='" + content + "',"
                   + "is_unread='" + is_unread + "',"
                   + "favorite='" + favorite + "',"
                   + "reponses_seen='" + res_seen + "',"
                   + "response_cnt='" + res_cnt +"'"
                   + "WHERE plurk_id='" + plurk_id + "'");
    } else {
        //Add record
        QString dummy;
        query.exec(dummy + "INSERT INTO plurks("
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
                   + "'" + plurk_id + "',"
                   + "'" + plurk_type + "',"
                   + "'" + owner_id + "',"
                   + "'" + content + "',"
                   + "'" + is_unread + "',"
                   + "'" + favorite + "',"
                   + "'" + qual_trans + "',"
                   + "'" + res_seen + "',"
                   + "'" + res_cnt + "',"
                   + "'" + posted + "')");
    }
}

void PlurkDbManager::addResponse(QString plurk_id, QString user_id,
                                 QString content, QString posted) {

}

void PlurkDbManager::addUser(QString user_id, QString nick_name,
                             QString display_name) {
    QSqlQuery query;
    query.exec("SELECT * FROM plurks WHERE plurk_id='" + user_id + "'");
    if(query.next()) {
        //Update record
        query.exec("UPDATE users SET display_name='" + display_name + "'"
                   + "WHERE user_id='" + user_id +"'");
    } else {
        //Add record
        QString dummy;
        query.exec(dummy + "INSERT INTO users(" +
                   + "user_id,"
                   + "nick_name,"
                   + "display_name) VALUES("
                   + "'" + user_id + "',"
                   + "'" + nick_name + "',"
                   + "'" + display_name +"')");
    }
}
