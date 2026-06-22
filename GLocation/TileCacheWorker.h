#pragma once

#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QQueue>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtSql/QSqlDatabase>
#include <QtCore/QLoggingCategory>

class GCSMapTask;
class CachedTileSet;

class TileCacheWorker : public QThread
{
    Q_OBJECT
public:
    TileCacheWorker (const QString&);
    ~TileCacheWorker ();

    void    quit            ();
    bool    enqueueTask     (GCSMapTask* task);
    void    setDatabaseFile (const QString& path);

protected:
    void    run             ();

private:
    void        _runTask                (GCSMapTask* task);

    void        _saveTile               (GCSMapTask* mtask);
    void        _getTile                (GCSMapTask* mtask);
    void        _getTileSets            (GCSMapTask* mtask);
    void        _createTileSet          (GCSMapTask* mtask);
    void        _getTileDownloadList    (GCSMapTask* mtask);
    void        _updateTileDownloadState(GCSMapTask* mtask);
    void        _deleteTileSet          (GCSMapTask* mtask);
    void        _resetCacheDatabase     (GCSMapTask* mtask);
    void        _pruneCache             (GCSMapTask* mtask);
    bool        _testTask               (GCSMapTask* mtask);
    //void      _deleteBingNoTileTiles  ();

    quint64     _findTile               (const QString hash);
    bool        _findTileSetID          (const QString name, quint64& setID);
    void        _updateSetTotals        (CachedTileSet* set);

    bool        _init                   ();
    bool        _connectDB              ();
    bool        _createDB               (QSqlDatabase& db);

    void        _disconnectDB           ();
    quint64     _getDefaultTileSet      ();
    void        _updateTotals           ();

    void        _deleteTileSet          (qulonglong id);

signals:
    void        updateTotals            (quint32 totaltiles, quint64 totalsize, quint32 defaulttiles, quint64 defaultsize);

private:
    QQueue<GCSMapTask*>             _taskQueue;
    QMutex                          _taskQueueMutex;
    QWaitCondition                  _waitc;
    QScopedPointer<QSqlDatabase>    _db;
    QString                         _databasePath;
    QString                         _sessionName;
    std::atomic_bool                _valid;
    bool                            _failed;
    quint64                         _defaultSet;
    quint64                         _totalSize;
    quint32                         _totalCount;
    quint64                         _defaultSize;
    quint32                         _defaultCount;
    time_t                          _lastUpdate;
    int                             _updateTimeout;
};
