#include "CachedTileSet.h"
#include "GCSMapEngine.h"
#include "MapEngineData.h"
#include "MapUrlEngine.h"
#include "log_category.h"
#include "time.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtNetwork/QNetworkProxy>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

TileCacheWorker::TileCacheWorker(const QString &type)
    : _db(nullptr), _sessionName(type), _valid(false), _failed(false), _defaultSet(UINT64_MAX), _totalSize(0),
      _totalCount(0), _defaultSize(0), _defaultCount(0), _lastUpdate(0), _updateTimeout(20) {}

TileCacheWorker::~TileCacheWorker() {}

void TileCacheWorker::setDatabaseFile(const QString &path) { _databasePath = path; }

void TileCacheWorker::quit() {
    QMutexLocker lock(&_taskQueueMutex);
    while (_taskQueue.count()) {
        GCSMapTask *task = _taskQueue.dequeue();
        delete task;
    }
    lock.unlock();
    if (this->isRunning()) {
        _waitc.wakeAll();
    }
}

bool TileCacheWorker::enqueueTask(GCSMapTask *task) {
    if (!_valid && task->type() != GCSMapTask::taskInit) {
        task->setError("Database Not Initialized");
        task->deleteLater();
        return false;
    }

    QMutexLocker lock(&_taskQueueMutex);
    _taskQueue.enqueue(task);
    lock.unlock();

    if (this->isRunning()) {
        _waitc.wakeAll();
    } else {
        this->start(QThread::HighPriority);
    }

    return true;
}

void TileCacheWorker::run() {
    if (!_valid && !_failed) {
        _init();
    }

    if (_valid) {
        _connectDB();
    }

    //_deleteBingNoTileTiles();
    QMutexLocker lock(&_taskQueueMutex);
    while (true) {
        GCSMapTask *task;
        if (_taskQueue.count()) {
            task = _taskQueue.dequeue();

            lock.unlock();
            _runTask(task);
            lock.relock();

            task->deleteLater();
            if (!static_cast<size_t>(_taskQueue.count()) || (time(nullptr) - _lastUpdate > _updateTimeout)) {
                if (_valid) {
                    lock.unlock();
                    _updateTotals();
                    lock.relock();
                }
            }
        } else {
            unsigned long timeoutMilliseconds = 2000;
            _waitc.wait(lock.mutex(), timeoutMilliseconds);
            if (!_taskQueue.count()) {
                break;
            }
        }
    }

    lock.unlock();
    _disconnectDB();
}

void TileCacheWorker::_runTask(GCSMapTask *task) {
    switch (task->type()) {
    case GCSMapTask::taskInit:
        return;
    case GCSMapTask::taskCacheTile:
        _saveTile(task);
        return;
    case GCSMapTask::taskFetchTile:
        _getTile(task);
        return;
    case GCSMapTask::taskFetchTileSets:
        _getTileSets(task);
        return;
    case GCSMapTask::taskCreateTileSet:
        _createTileSet(task);
        return;
    case GCSMapTask::taskGetTileDownloadList:
        _getTileDownloadList(task);
        return;
    case GCSMapTask::taskUpdateTileDownloadState:
        _updateTileDownloadState(task);
        return;
    case GCSMapTask::taskDeleteTileSet:
        _deleteTileSet(task);
        return;
    case GCSMapTask::taskPruneCache:
        _pruneCache(task);
        return;
    case GCSMapTask::taskReset:
        _resetCacheDatabase(task);
        return;
    }

    COMMON_WARN("iven unhandled task type", (int) task->type());
}

// void TileCacheWorker::_deleteBingNoTileTiles()
// {
//     QByteArray noTileBytes;
//     QFile file(":/Resources/BingNoTileBytes.dat");
//     file.open(QFile::ReadOnly);
//     noTileBytes = file.readAll();
//     file.close();
//     QSqlQuery query(*_db);
//     QString s;
//     s = QString("SELECT tileID, tile, hash FROM Tiles WHERE LENGTH(tile) = ?");
//     query.addBindValue(noTileBytes.length());
//     QList<quint64> idsToDelete;
//     if (query.exec(s)) {
//         while(query.next()) {
//             if (query.value(1).toByteArray() == noTileBytes) {
//                 idsToDelete.append(query.value(0).toULongLong());
//                 COMMON_INFO("delete bing no tile tiles failed {0}",
//                          query.value(2).toString().toStdString().c_str());
//             }
//         }
//         for (const quint64 tileId: idsToDelete) {
//             s = QString("DELETE FROM Tiles WHERE tileID = %1").arg(tileId);
//             if (!query.exec(s)) {
//                 COMMON_WARN("delete bing no tile tiles failed {0}", s.toStdString().c_str());
//             }
//         }
//     }
//     else {
//         COMMON_WARN("delete bing no tile tiles failed");
//     }
// }

bool TileCacheWorker::_findTileSetID(const QString name, quint64 &setID) {
    QSqlQuery query(*_db);
    QString s = QString("SELECT setID FROM TileSets WHERE name = \"%1\"").arg(name);
    if (query.exec(s)) {
        if (query.next()) {
            setID = query.value(0).toULongLong();
            return true;
        }
    }
    return false;
}

quint64 TileCacheWorker::_getDefaultTileSet() {
    if (_defaultSet != UINT64_MAX) {
        return _defaultSet;
    }

    QSqlQuery query(*_db);
    QString s = QString("SELECT setID FROM TileSets WHERE defaultSet = 1");
    if (query.exec(s)) {
        if (query.next()) {
            _defaultSet = query.value(0).toULongLong();
            return _defaultSet;
        }
    }
    return 1L;
}

void TileCacheWorker::_saveTile(GCSMapTask *mtask) {
    if (_valid) {
        SaveTileTask *task = static_cast<SaveTileTask *>(mtask);
        QSqlQuery query(*_db);

        query.prepare("INSERT INTO Tiles(hash, format, tile, size, type, date) VALUES(?, ?, ?, ?, ?, ?)");
        query.addBindValue(task->tile()->hash());
        query.addBindValue(task->tile()->format());
        query.addBindValue(task->tile()->img());
        query.addBindValue(task->tile()->img().size());
        query.addBindValue(task->tile()->type());
        query.addBindValue(QDateTime::currentDateTime().toSecsSinceEpoch());

        if (query.exec()) {
            quint64 tileID = query.lastInsertId().toULongLong();
            quint64 setID = task->tile()->set() == UINT64_MAX ? _getDefaultTileSet() : task->tile()->set();
            QString s = QString("INSERT INTO SetTiles(tileID, setID) VALUES(%1, %2)").arg(tileID).arg(setID);
            query.prepare(s);

            if (!query.exec()) {
                COMMON_WARN("add tile into SetTiles  {0}", query.lastError().text().toStdString().c_str());
            }
        }
    }
}

void TileCacheWorker::_getTile(GCSMapTask *mtask) {
    if (!_testTask(mtask)) {
        return;
    }

    bool found = false;
    GCSFetchTileTask *task = static_cast<GCSFetchTileTask *>(mtask);
    QSqlQuery query(*_db);
    QString s = QString("SELECT tile, format, type FROM Tiles WHERE hash = \"%1\"").arg(task->hash());
    if (query.exec(s)) {
        if (query.next()) {
            const QByteArray &arrray = query.value(0).toByteArray();
            const QString &format = query.value(1).toString();
            const QString &type = query.value(2).toString();

            GCSCacheTile *tile = new GCSCacheTile(task->hash(), arrray, format, type);
            task->setTileFetched(tile);
            found = true;
        }
    }

    if (!found) {
        COMMON_CRITICAL("NOT in DB, HASH {0}", task->hash().toStdString().c_str());
        task->setError("Tile not in cache database");
    }
}

void TileCacheWorker::_getTileSets(GCSMapTask *mtask) {
    if (!_testTask(mtask)) {
        return;
    }

    GCSFetchTileSetTask *task = static_cast<GCSFetchTileSetTask *>(mtask);
    QSqlQuery query(*_db);

    QString s = QString("SELECT * FROM TileSets ORDER BY defaultSet DESC, name ASC");

    if (query.exec(s)) {
        while (query.next()) {
            QString name = query.value("name").toString();
            CachedTileSet *set = new CachedTileSet(name);
            set->setId(query.value("setID").toULongLong());
            set->setMapTypeStr(query.value("typeStr").toString());
            set->setTopleftLat(query.value("topleftLat").toDouble());
            set->setTopleftLon(query.value("topleftLon").toDouble());
            set->setBottomRightLat(query.value("bottomRightLat").toDouble());
            set->setBottomRightLon(query.value("bottomRightLon").toDouble());
            set->setMinZoom(query.value("minZoom").toInt());
            set->setMaxZoom(query.value("maxZoom").toInt());
            set->setType(getGCSMapEngine()->mapUrlEngine()->getProviderTypeFromQtMapId(query.value("type").toInt()));
            set->setTotalTileCount(query.value("numTiles").toUInt());
            set->setDefaultSet(query.value("defaultSet").toInt() != 0);
            set->setCreationDate(QDateTime::fromSecsSinceEpoch(query.value("date").toUInt()));
            _updateSetTotals(set);
            set->moveToThread(QCoreApplication::instance()->thread());
            task->tileSetFetched(set);
        }
    } else {
        task->setError("No tile set in database");
    }
}

void TileCacheWorker::_updateSetTotals(CachedTileSet *set) {
    if (set->defaultSet()) {
        _updateTotals();
        set->setSavedTileCount(_totalCount);
        set->setSavedTileSize(_totalSize);
        set->setTotalTileCount(_defaultCount);
        set->setTotalTileSize(_defaultSize);
        return;
    }

    QSqlQuery subquery(*_db);
    QString sq = QString("SELECT COUNT(size), SUM(size) FROM Tiles A INNER JOIN SetTiles B on "
                         "A.tileID = B.tileID WHERE B.setID = %1")
                     .arg(set->id());
    COMMON_INFO("update set  {0}", sq.toStdString().c_str());

    if (subquery.exec(sq)) {
        if (subquery.next()) {
            set->setSavedTileCount(subquery.value(0).toUInt());
            set->setSavedTileSize(subquery.value(1).toULongLong());

            quint64 avg = 32768;
            if (set->totalTileCount() <= set->savedTileCount()) {
                set->setTotalTileSize(set->savedTileSize());
            } else {
                if (set->savedTileCount() > 2 && set->savedTileSize()) {
                    avg = set->savedTileSize() / set->savedTileCount();
                }
                set->setTotalTileSize(avg * set->totalTileCount());
            }
            quint32 ucount = 0;
            quint64 usize = 0;
            QString mm = QString("SELECT A.tileID FROM SetTiles A join SetTiles B on A.tileID = B.tileID "
                                 "WHERE B.setID = %1 GROUP by A.tileID HAVING COUNT(A.tileID) = 1")
                             .arg(set->id());
            sq = QString("SELECT COUNT(size), SUM(size) FROM Tiles WHERE tileID IN (%1)").arg(mm);
            if (subquery.exec(sq)) {
                if (subquery.next()) {
                    ucount = subquery.value(0).toUInt();
                    usize = subquery.value(1).toULongLong();
                }
            }
            quint32 expectedUcount = set->totalTileCount() - set->savedTileCount();
            if (!ucount) {
                usize = expectedUcount * avg;
            } else {
                expectedUcount = ucount;
            }
            set->setUniqueTileCount(expectedUcount);
            set->setUniqueTileSize(usize);
        }
    }
}

void TileCacheWorker::_updateTotals() {
    QSqlQuery query(*_db);
    QString s = QString("SELECT COUNT(size), SUM(size) FROM Tiles");

    if (query.exec(s)) {
        if (query.next()) {
            _totalCount = query.value(0).toUInt();
            _totalSize = query.value(1).toULongLong();
        }
    }

    QString mm = QString("SELECT A.tileID FROM SetTiles A join SetTiles B on A.tileID = B.tileID "
                         "WHERE B.setID = %1 GROUP by A.tileID HAVING COUNT(A.tileID) = 1")
                     .arg(_getDefaultTileSet());
    s = QString("SELECT COUNT(size), SUM(size) FROM Tiles WHERE tileID IN (%1)").arg(mm);
    if (query.exec(s)) {
        if (query.next()) {
            _defaultCount = query.value(0).toUInt();
            _defaultSize = query.value(1).toULongLong();
        }
    }

    emit updateTotals(_totalCount, _totalSize, _defaultCount, _defaultSize);
    _lastUpdate = time(nullptr);
}

quint64 TileCacheWorker::_findTile(const QString hash) {
    quint64 tileID = 0;
    QSqlQuery query(*_db);
    QString s = QString("SELECT tileID FROM Tiles WHERE hash = \"%1\"").arg(hash);

    if (query.exec(s)) {
        if (query.next()) {
            tileID = query.value(0).toULongLong();
        }
    }

    return tileID;
}

void TileCacheWorker::_createTileSet(GCSMapTask *mtask) {
    if (_valid) {
        CreateTileSetTask *task = static_cast<CreateTileSetTask *>(mtask);
        QSqlQuery query(*_db);
        query.prepare("INSERT INTO TileSets("
                      "name, typeStr, topleftLat, topleftLon, bottomRightLat, bottomRightLon, "
                      "minZoom, maxZoom, type, numTiles, date"
                      ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        query.addBindValue(task->tileSet()->name());
        query.addBindValue(task->tileSet()->mapTypeStr());
        query.addBindValue(task->tileSet()->topleftLat());
        query.addBindValue(task->tileSet()->topleftLon());
        query.addBindValue(task->tileSet()->bottomRightLat());
        query.addBindValue(task->tileSet()->bottomRightLon());
        query.addBindValue(task->tileSet()->minZoom());
        query.addBindValue(task->tileSet()->maxZoom());
        query.addBindValue(getGCSMapEngine()->mapUrlEngine()->getQtMapIdFromProviderType(task->tileSet()->type()));
        query.addBindValue(task->tileSet()->totalTileCount());
        query.addBindValue(QDateTime::currentDateTime().toSecsSinceEpoch());

        if (!query.exec()) {
            COMMON_WARN("add tileSet into TileSets  {0}", query.lastError().text().toStdString().c_str());
        } else {
            quint64 setID = query.lastInsertId().toULongLong();
            task->tileSet()->setId(setID);
            _db->transaction();

            for (int z = task->tileSet()->minZoom(); z <= task->tileSet()->maxZoom(); z++) {
                GCSTileSet set = GCSMapEngine::getTileCount(
                    z, task->tileSet()->topleftLon(), task->tileSet()->topleftLat(), task->tileSet()->bottomRightLon(),
                    task->tileSet()->bottomRightLat(), task->tileSet()->type());
                QString type = task->tileSet()->type();

                int actual_count = 0;
                for (int x = set.tileX0; x <= set.tileX1; x++) {
                    for (int y = set.tileY0; y <= set.tileY1; y++) {
                        QString hash = getGCSMapEngine()->getTileHash(type, x, y, z);
                        quint64 tileID = _findTile(hash);

                        if (!tileID) {
                            query.prepare("INSERT OR IGNORE INTO TilesDownload(setID, hash, type, "
                                          "x, y, z, state) VALUES(?, ?, ?, ?, ? ,? ,?)");
                            query.addBindValue(setID);
                            query.addBindValue(hash);
                            query.addBindValue(getGCSMapEngine()->mapUrlEngine()->getQtMapIdFromProviderType(type));
                            query.addBindValue(x);
                            query.addBindValue(y);
                            query.addBindValue(z);
                            query.addBindValue(0);

                            if (!query.exec()) {
                                COMMON_WARN("add tile into TilesDownload  {0}",
                                            query.lastError().text().toStdString().c_str());
                                mtask->setError("Error creating tile set download list");
                                return;
                            } else {
                                actual_count++;
                            }
                        } else {
                            QString s = QString("INSERT OR IGNORE INTO SetTiles(tileID, setID) VALUES(%1, %2)")
                                            .arg(tileID)
                                            .arg(setID);
                            query.prepare(s);

                            if (!query.exec()) {
                                COMMON_WARN("add tile into SetTiles  {0}",
                                            query.lastError().text().toStdString().c_str());
                            }
                        }
                    }
                }
            }
            _db->commit();
            _updateSetTotals(task->tileSet());
            task->setTileSetSaved();
            return;
        }
    }

    mtask->setError("Error saving tile set");
}

void TileCacheWorker::_getTileDownloadList(GCSMapTask *mtask) {
    if (!_testTask(mtask)) {
        return;
    }

    QList<GCSTile *> tiles;
    TileDownloadListTask *task = static_cast<TileDownloadListTask *>(mtask);

    QSqlQuery query(*_db);
    QString s = QString("SELECT hash, type, x, y, z FROM TilesDownload WHERE setID = %1 AND state = 0 LIMIT %2")
                    .arg(task->setID())
                    .arg(task->count());

    if (query.exec(s)) {
        while (query.next()) {
            GCSTile *tile = new GCSTile;
            tile->setHash(query.value("hash").toString());
            tile->setType(getGCSMapEngine()->mapUrlEngine()->getProviderTypeFromQtMapId(query.value("type").toInt()));
            tile->setX(query.value("x").toInt());
            tile->setY(query.value("y").toInt());
            tile->setZ(query.value("z").toInt());
            tiles.append(tile);
        }
        for (int i = 0; i < tiles.size(); i++) {
            s = QString("UPDATE TilesDownload SET state = %1 WHERE setID = %2 and hash = \"%3\"")
                    .arg(static_cast<int>(GCSTile::StateDownloading))
                    .arg(task->setID())
                    .arg(tiles[i]->hash());
            if (!query.exec(s)) {
                COMMON_WARN("set TilesDownload state  {0}", query.lastError().text().toStdString().c_str());
            }
        }
    }

    task->setTileListFetched(tiles);
}

void TileCacheWorker::_updateTileDownloadState(GCSMapTask *mtask) {
    if (!_testTask(mtask)) {
        return;
    }

    TileDownloadStateTask *task = static_cast<TileDownloadStateTask *>(mtask);
    QSqlQuery query(*_db);
    QString s;

    if (task->state() == GCSTile::StateComplete) {
        s = QString("DELETE FROM TilesDownload WHERE setID = %1 AND hash = \"%2\"")
                .arg(task->setID())
                .arg(task->hash());
    } else {
        if (task->hash() == "*") {
            s = QString("UPDATE TilesDownload SET state = %1 WHERE setID = %2")
                    .arg(static_cast<int>(task->state()))
                    .arg(task->setID());
        } else {
            s = QString("UPDATE TilesDownload SET state = %1 WHERE setID = %2 AND hash = \"%3\"")
                    .arg(static_cast<int>(task->state()))
                    .arg(task->setID())
                    .arg(task->hash());
        }
    }

    if (!query.exec(s)) {
        COMMON_WARN("update TilesDownload state  {0}", query.lastError().text().toStdString().c_str());
    }
}

void TileCacheWorker::_pruneCache(GCSMapTask *mtask) {
    if (!_testTask(mtask)) {
        return;
    }

    PruneCacheTask *task = static_cast<PruneCacheTask *>(mtask);
    QSqlQuery query(*_db);
    QString s;

    QString mm = QString("SELECT A.tileID FROM SetTiles A join SetTiles B on A.tileID = B.tileID "
                         "WHERE B.setID = %1 GROUP by A.tileID HAVING COUNT(A.tileID) = 1")
                     .arg(_getDefaultTileSet());
    s = QString("SELECT tileID, size, hash FROM Tiles WHERE tileID IN (%1) ORDER BY DATE ASC LIMIT 128").arg(mm);
    qint64 amount = (qint64) task->amount();
    QList<quint64> tlist;

    if (query.exec(s)) {
        while (query.next() && amount >= 0) {
            tlist << query.value(0).toULongLong();
            amount -= query.value(1).toULongLong();
        }
        while (tlist.count()) {
            s = QString("DELETE FROM Tiles WHERE tileID = %1").arg(tlist[0]);
            tlist.removeFirst();
            if (!query.exec(s))
                break;
        }
        task->setPruned();
    }
}

void TileCacheWorker::_deleteTileSet(GCSMapTask *mtask) {
    if (!_testTask(mtask)) {
        return;
    }
    DeleteTileSetTask *task = static_cast<DeleteTileSetTask *>(mtask);
    _deleteTileSet(task->setID());
    task->setTileSetDeleted();
}

void TileCacheWorker::_deleteTileSet(qulonglong id) {
    QSqlQuery query(*_db);
    QString s;

    QString mm = QString("SELECT A.tileID FROM SetTiles A JOIN SetTiles B ON A.tileID = B.tileID "
                         "WHERE B.setID = %1 GROUP BY A.tileID HAVING COUNT(A.tileID) = 1")
                     .arg(id);
    s = QString("DELETE FROM Tiles WHERE tileID IN (%1)").arg(mm);
    query.exec(s);
    s = QString("DELETE FROM TilesDownload WHERE setID = %1").arg(id);
    query.exec(s);
    s = QString("DELETE FROM TileSets WHERE setID = %1").arg(id);
    query.exec(s);
    s = QString("DELETE FROM SetTiles WHERE setID = %1").arg(id);
    query.exec(s);
    _updateTotals();
}

void TileCacheWorker::_resetCacheDatabase(GCSMapTask *mtask) {
    if (!_testTask(mtask)) {
        return;
    }

    ResetTask *task = static_cast<ResetTask *>(mtask);
    QSqlQuery query(*_db);
    QString s;
    s = QString("DROP TABLE Tiles");
    query.exec(s);
    s = QString("DROP TABLE TileSets");
    query.exec(s);
    s = QString("DROP TABLE SetTiles");
    query.exec(s);
    s = QString("DROP TABLE TilesDownload");
    query.exec(s);
    _valid = _createDB(*_db);
    task->setResetCompleted();
}

bool TileCacheWorker::_testTask(GCSMapTask *mtask) {
    if (!_valid) {
        mtask->setError("No Cache Database");
        return false;
    }

    return true;
}

bool TileCacheWorker::_init() {
    _failed = false;
    if (!_databasePath.isEmpty()) {
        COMMON_INFO("Mapping cache directory {0}", _databasePath.toStdString().c_str());

        if (_connectDB()) {
            _valid = _createDB(*_db);
            if (!_valid) {
                _failed = true;
            }
        } else {
            COMMON_INFO("open db error {0}", _db->lastError().text().toStdString().c_str());
            _failed = true;
        }

        _disconnectDB();

    } else {
        COMMON_CRITICAL("Could not find suitable cache directory");
        _failed = true;
    }

    return _failed;
}

bool TileCacheWorker::_connectDB() {
    _db.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", _sessionName)));
    _db->setDatabaseName(_databasePath);
    _db->setConnectOptions("QSQLITE_ENABLE_SHARED_CACHE");
    _valid = _db->open();
    return _valid;
}

bool TileCacheWorker::_createDB(QSqlDatabase &db) {
    bool res = false;
    QSqlQuery query(db);

    if (!query.exec("CREATE TABLE IF NOT EXISTS Tiles ("
                    "tileID INTEGER PRIMARY KEY NOT NULL, "
                    "hash TEXT NOT NULL UNIQUE, "
                    "format TEXT NOT NULL, "
                    "tile BLOB NULL, "
                    "size INTEGER, "
                    "type INTEGER, "
                    "date INTEGER DEFAULT 0)")) {
        COMMON_WARN("create Tiles db  {0}", query.lastError().text().toStdString().c_str());
    } else {
        query.exec("CREATE INDEX IF NOT EXISTS hash ON Tiles ( hash, size, type ) ");

        if (!query.exec("CREATE TABLE IF NOT EXISTS TileSets ("
                        "setID INTEGER PRIMARY KEY NOT NULL, "
                        "name TEXT NOT NULL UNIQUE, "
                        "typeStr TEXT, "
                        "topleftLat REAL DEFAULT 0.0, "
                        "topleftLon REAL DEFAULT 0.0, "
                        "bottomRightLat REAL DEFAULT 0.0, "
                        "bottomRightLon REAL DEFAULT 0.0, "
                        "minZoom INTEGER DEFAULT 3, "
                        "maxZoom INTEGER DEFAULT 3, "
                        "type INTEGER DEFAULT -1, "
                        "numTiles INTEGER DEFAULT 0, "
                        "defaultSet INTEGER DEFAULT 0, "
                        "date INTEGER DEFAULT 0)")) {
            COMMON_WARN("create TileSets db  {0}", query.lastError().text().toStdString().c_str());
        } else {
            if (!query.exec("CREATE TABLE IF NOT EXISTS SetTiles ("
                            "setID INTEGER, "
                            "tileID INTEGER)")) {
                COMMON_WARN("create SetTiles db  {0}", query.lastError().text().toStdString().c_str());
            } else {
                if (!query.exec("CREATE TABLE IF NOT EXISTS TilesDownload ("
                                "setID INTEGER, "
                                "hash TEXT NOT NULL UNIQUE, "
                                "type INTEGER, "
                                "x INTEGER, "
                                "y INTEGER, "
                                "z INTEGER, "
                                "state INTEGER DEFAULT 0)")) {
                    COMMON_WARN("create TilesDownload db  {0}", query.lastError().text().toStdString().c_str());
                } else {
                    res = true;
                }
            }
        }
    }

    if (!res) {
        QFile file(_databasePath);
        file.remove();
    }

    return res;
}

void TileCacheWorker::_disconnectDB() {
    if (_db) {
        _db.reset();
        QSqlDatabase::removeDatabase(_sessionName);
    }
}
