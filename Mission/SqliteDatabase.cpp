#include "SqliteDatabase.h"
#include "GcsLineStage.h"
#include "log_category.h"

#include <QString>
#include <QVariant>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

bool GcsDatabase::dbFlag() { return _dbflag; }

GcsDatabase::GcsDatabase() : _db(nullptr), _databaseFile() {}

GcsDatabase::~GcsDatabase() {}

QString GcsDatabase::checkStage(int uid, const QString &table) {
    QSqlQuery query(*_db);
    QString queryStr = ("SELECT code FROM " + table + " WHERE uid = ? ");
    COMMON_INFO("find data {0}", queryStr.toStdString());
    query.prepare(queryStr);
    query.addBindValue(uid);

    if (query.exec()) {
        if (query.next()) {
            return query.value(0).toString();
        }
    }

    return QString();
}

QString GcsDatabase::findStage(const QString &name, const QString &table) {
    QSqlQuery query(*_db);
    QString queryStr = ("SELECT data FROM ? WHERE name = ?");
    COMMON_INFO("find data {0}", queryStr.toStdString());
    query.prepare(queryStr);

    query.addBindValue(table);
    query.addBindValue(name);

    if (query.exec()) {
        if (query.next()) {
            return query.value(0).toString();
        }
    }

    return QString();
}

void GcsDatabase::loadLine(QMap<GcsTreeNode *, std::shared_ptr<GcsTreeNode>> &line) {
    if (_db == nullptr || !_db->isOpen()) {
        COMMON_INFO("database not open");
        return;
    }

    QSqlQuery query(*_db);
    QString s = ("SELECT uid, name, code, data, udate FROM MissionLine");
    COMMON_INFO("load data {0}", s.toStdString());

    if (query.exec(s)) {
        while (query.next()) {
            std::shared_ptr<FLineController> ctl(new FLineController());
            ctl->uuid(query.value(("uid")).toInt());
            ctl->name(query.value(("name")).toString());
            ctl->json2Item(query.value(("data")).toByteArray());
            ctl->update(query.value(("udate")).toString());
            line.insert(ctl.get(), ctl);
        }
    }
}

bool GcsDatabase::updateLine(const FLineController *item, const QByteArray &data) {
    if (_db == nullptr || !_db->isOpen()) {
        COMMON_INFO("database not open");
        return false;
    }

    if (item == nullptr) {
        COMMON_INFO("invalid input parameter");
        return false;
    }

    QSqlQuery query(*_db);
    QString str = ("UPDATE MissionLine set data=?, udate=?, code=?, name=? WHERE uid=?");

    query.prepare(str);
    query.addBindValue(data);
    query.addBindValue(item->update());
    query.addBindValue(0);
    query.addBindValue(item->name());
    query.addBindValue(item->uuid());

    if (query.exec()) {
        COMMON_INFO("save mission stage {0}", data.toStdString().c_str());
        return true;
    } else {
        QString err = query.lastError().text();
        COMMON_WARN("save mission failed  {0} {1}", err.toStdString(), data.toStdString().c_str());
    }

    return false;
}

bool GcsDatabase::saveLine(const FLineController *item, const QByteArray &data) {

    if (_db == nullptr || !_db->isOpen()) {
        COMMON_INFO("database not open");
        return false;
    }

    if (item == nullptr) {
        COMMON_INFO("invalid input parameter");
        return false;
    }

    QSqlQuery query(*_db);
    query.prepare(("INSERT INTO MissionLine(uid, name, code, data, udate) VALUES(?, ?, ?, ?, ?)"));

    query.addBindValue(item->uuid());
    query.addBindValue(item->name());
    query.addBindValue(0);
    query.addBindValue(data);
    query.addBindValue(item->update());

    if (query.exec()) {
        COMMON_INFO("save mission stage {0}", data.toStdString().c_str());
        return true;
    } else {
        QString err = query.lastError().text();
        COMMON_WARN("save mission failed {0} {1}", err.toStdString(), data.toStdString().c_str());
    }

    return false;
}

bool GcsDatabase::removeLine(const int uid) {
    if (_db == nullptr || !_db->isOpen()) {
        COMMON_INFO("database not open");
        return false;
    }

    QSqlQuery query(*_db);
    QString s = ("DELETE From MissionLine WHERE uid = ?");

    COMMON_INFO("remove data {0}", s.toStdString());

    query.prepare(s);
    query.addBindValue(uid);

    if (query.exec()) {
        COMMON_INFO("remove mission line {0}", uid);
        return true;
    } else {
        COMMON_WARN("remove mission line {0}", query.lastError().text().toStdString());
        return false;
    }
}

QMap<int, QString> &GcsDatabase::stageMapName() {
    static QMap<int, QString> gStageName = {
        {GcsLineStage::GisFence, ("GisFenceTable")}, {GcsLineStage::Curise, ("CuriseTable")},
        {GcsLineStage::Landing, ("LandingTable")},   {GcsLineStage::Ergency, ("ErgencyTable")},
        {GcsLineStage::Depart, ("EnterTable")},      {GcsLineStage::TaskNorm, ("MissionStage")},
    };

    return gStageName;
}

QString GcsDatabase::stageTable(int code) {
    QMap<int, QString> &mapName = stageMapName();
    if (mapName.find(code) != mapName.end()) {
        return mapName.find(code).value();
    }

    return QString();
}

bool GcsDatabase::saveStage(const StageController *item, const QByteArray &data) {
    if (_db == nullptr || !_db->isOpen()) {
        COMMON_INFO("database not open");
        return false;
    }

    QString table = stageTable(item->stage());
    QString value = checkStage(item->uuid(), table);

    if (value.isEmpty()) {
        QString queryStr = ("INSERT INTO ") + table;
        queryStr += ("(uid, name, code, data, udate) VALUES(?, ?, ?, ?, ?)");
        int val = ((int) item->defNode() << 8);

        QSqlQuery query(*_db);
        query.prepare(queryStr);
        query.addBindValue(item->uuid());
        query.addBindValue(item->name());
        query.addBindValue(val);
        query.addBindValue(data);
        query.addBindValue(item->update());

        if (query.exec()) {
            COMMON_INFO("save mission stage {0}", data.toStdString().c_str());
            return true;
        } else {
            QString err = query.lastError().text();
            COMMON_WARN("save mission failed {0} {1}", err.toStdString().c_str(), data.toStdString().c_str());
        }
    } else {
        QSqlQuery query(*_db);
        QString queryStr = ("UPDATE " + table + " SET data=?, udate = ?, name = ?, code = ? WHERE uid = ?");
        int val = ((int) item->defNode() << 8);
        query.prepare(queryStr);
        query.addBindValue(data);
        query.addBindValue(item->update());
        query.addBindValue(item->name());
        query.addBindValue(val);
        query.addBindValue(item->uuid());

        if (query.exec()) {
            COMMON_INFO("save mission stage {0}", data.toStdString().c_str());
            return true;
        } else {
            QString err = query.lastError().text();
            COMMON_WARN("save mission failed {0} {1}", err.toStdString().c_str(), data.toStdString().c_str());
        }
    }

    return false;
}

bool GcsDatabase::removeStage(const QString &name, int stage, int id) {
    if (_db == nullptr || !_db->isOpen()) {
        COMMON_INFO("database not open");
        return false;
    }

    QSqlQuery query(*_db);
    QString table = stageTable(stage);
    QString s = ("DELETE From " + table + " WHERE uid = ?");
    COMMON_INFO("remove data {0}", s.toStdString());

    query.prepare(s);
    query.addBindValue(id);

    if (query.exec()) {
        COMMON_INFO("remove mission stage {0}", name.toStdString());
        return true;
    } else {
        COMMON_WARN("udate mission failed {0} {1}", name.toStdString(), query.lastError().text().toStdString());
        return false;
    }
}

void GcsDatabase::loadStage(GcsListmodel &stageList, int stage) {
    if (_db == nullptr || !_db->isOpen()) {
        COMMON_INFO("database not open");
        return;
    }

    QSqlQuery query(*_db);
    QString table = stageTable(stage);
    QString s = ("SELECT uid, name, code, data, udate FROM ") + table;
    COMMON_INFO("load data {0}", s.toStdString());

    query.prepare(s);

    if (query.exec()) {
        while (query.next()) {
            StageController *ctl = new StageController(stage);
            int val = query.value(("code")).toInt();
            ctl->defNode((val >> 8) & 0xff);
            ctl->addRefer();
            ctl->uuid(query.value(("uid")).toInt());
            ctl->setName(query.value(("name")).toString());
            ctl->json2Waypoint(query.value(("data")).toByteArray());
            ctl->setUpdate(query.value(("udate")).toString());
            stageList.append(ctl);
        }
    }
}

void GcsDatabase::setDatabasePath(const QString &path) { _databaseFile = path; }

bool GcsDatabase::connectDB() {

    QString sqlite = ("QSQLITE");
    QString mission = ("MissionSession");

    _db.reset(new QSqlDatabase(QSqlDatabase::addDatabase(sqlite, mission)));

    if (_databaseFile.isEmpty()) {
        _databaseFile = ("GcsMission");
    }

    _db->setDatabaseName(_databaseFile); // database file
    _db->setConnectOptions(("QSQLITE_ENABLE_SHARED_CACHE"));

    if (!_db->open()) {
        COMMON_CRITICAL("connect database  {0}", _db->lastError().text().toStdString());
        return false;
    }

    return true;
}

bool GcsDatabase::createDB() {
    bool res = true;
    QSqlQuery query(*_db);

    bool rtn = query.exec(("CREATE TABLE IF NOT EXISTS MissionLine ("
                           "uid INTEGER PRIMARY KEY, "
                           "name char(20) NOT NULL, "
                           "code char(20) NOT NULL, "
                           "udate char(20) NOT NULL, "
                           "data TEXT)"));
    if (!rtn) {
        COMMON_CRITICAL("create database table {0}", query.lastError().text().toStdString());
        res = false;
    }

    rtn = query.exec(("CREATE TABLE IF NOT EXISTS GisFenceTable ("
                      "uid INTEGER PRIMARY KEY, "
                      "name char(20) NOT NULL, "
                      "code char(20) NOT NULL, "
                      "udate char(20) NOT NULL, "
                      "data TEXT)"));
    if (!rtn) {
        COMMON_CRITICAL("create database table {0}", query.lastError().text().toStdString());
        res = false;
    }

    rtn = query.exec(("CREATE TABLE IF NOT EXISTS CuriseTable ("
                      "uid INTEGER PRIMARY KEY, "
                      "name char(20) NOT NULL, "
                      "code char(20) NOT NULL, "
                      "udate char(20) NOT NULL, "
                      "data TEXT)"));
    if (!rtn) {
        COMMON_CRITICAL("create database table {0}", query.lastError().text().toStdString());
        res = false;
    }

    rtn = query.exec(("CREATE TABLE IF NOT EXISTS LandingTable ("
                      "uid INTEGER PRIMARY KEY, "
                      "name char(20) NOT NULL, "
                      "code char(20) NOT NULL, "
                      "udate char(20) NOT NULL, "
                      "data TEXT)"));
    if (!rtn) {
        COMMON_CRITICAL("create database table {0}", query.lastError().text().toStdString());
        res = false;
    }

    rtn = query.exec(("CREATE TABLE IF NOT EXISTS ErgencyTable ("
                      "uid INTEGER PRIMARY KEY, "
                      "name char(20) NOT NULL, "
                      "code char(20) NOT NULL, "
                      "udate char(20) NOT NULL, "
                      "data TEXT)"));
    if (!rtn) {
        COMMON_CRITICAL("create database table {0}", query.lastError().text().toStdString());
        res = false;
    }

    rtn = query.exec(("CREATE TABLE IF NOT EXISTS EnterTable ("
                      "uid INTEGER PRIMARY KEY, "
                      "name char(20) NOT NULL, "
                      "code char(20) NOT NULL, "
                      "udate char(20) NOT NULL, "
                      "data TEXT)"));
    if (!rtn) {
        COMMON_CRITICAL("create database table {0}", query.lastError().text().toStdString());
        res = false;
    }

    if (res) {
        _dbflag = true;
    } else {
        QFile file(_databaseFile);
        file.remove();
    }

    return res;
}

void GcsDatabase::disconnectDB() {
    if (_db) {
        _db.reset();
        QSqlDatabase::removeDatabase(("GcsMission"));
    }
}
