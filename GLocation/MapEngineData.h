#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

#include "MapTask.h"

class CreateTileSetTask : public GCSMapTask
{
    Q_OBJECT
public:
    CreateTileSetTask(CachedTileSet* tileSet)
        : GCSMapTask(GCSMapTask::taskCreateTileSet)
        , _tileSet(tileSet)
        , _saved(false)
    {}

    ~CreateTileSetTask() ;

    CachedTileSet*  tileSet () { return _tileSet; }

    void setTileSetSaved()
    {
        _saved = true;
        emit tileSetSaved(_tileSet);
    }

signals:
    void tileSetSaved   (CachedTileSet* tileSet);

private:
    CachedTileSet* _tileSet;
    bool              _saved;
};

class SaveTileTask : public GCSMapTask
{
    Q_OBJECT
public:
    SaveTileTask(GCSCacheTile* tile)
        : GCSMapTask(GCSMapTask::taskCacheTile)
        , _tile(tile)
    {}

    ~SaveTileTask()
    {
        delete _tile;
        _tile = nullptr;
    }

    GCSCacheTile*   tile() { return _tile; }

private:
    GCSCacheTile*   _tile;
};

class TileDownloadListTask : public GCSMapTask
{
    Q_OBJECT
public:
    TileDownloadListTask(qulonglong setID, int count)
        : GCSMapTask(GCSMapTask::taskGetTileDownloadList)
        , _setID(setID)
        , _count(count)
    {}

    qulonglong  setID() const{ return _setID; }
    int         count() const{ return _count; }

    void setTileListFetched(QList<GCSTile*> tiles)
    {
        emit tileListFetched(tiles);
    }

signals:
    void            tileListFetched  (QList<GCSTile*> tiles);

private:
    qulonglong  _setID;
    int         _count;
};

class TileDownloadStateTask : public GCSMapTask
{
    Q_OBJECT
public:
    TileDownloadStateTask(qulonglong setID, GCSTile::TyleState state, const QString& hash)
        : GCSMapTask(GCSMapTask::taskUpdateTileDownloadState)
        , _setID(setID)
        , _state(state)
        , _hash(hash)
    {}

    QString             hash    () { return _hash; }
    qulonglong          setID   () const{ return _setID; }
    GCSTile::TyleState  state   () { return _state; }

private:
    qulonglong          _setID;
    GCSTile::TyleState  _state;
    QString             _hash;
};

class DeleteTileSetTask : public GCSMapTask
{
    Q_OBJECT
public:
    DeleteTileSetTask(qulonglong setID)
        : GCSMapTask(GCSMapTask::taskDeleteTileSet)
        , _setID(setID)
    {}

    qulonglong  setID() const{ return _setID; }

    void setTileSetDeleted()
    {
        emit tileSetDeleted(_setID);
    }

signals:
    void tileSetDeleted(qulonglong setID);

private:
    qulonglong  _setID;
};

class PruneCacheTask : public GCSMapTask
{
    Q_OBJECT
public:
    PruneCacheTask(quint64 amount)
        : GCSMapTask(GCSMapTask::taskPruneCache)
        , _amount(amount)
    {}

    quint64  amount() const{ return _amount; }

    void setPruned()
    {
        emit pruned();
    }

signals:
    void pruned();

private:
    quint64  _amount;
};

class ResetTask : public GCSMapTask
{
    Q_OBJECT
public:
    ResetTask()
        : GCSMapTask(GCSMapTask::taskReset)
    {}

    void setResetCompleted()
    {
        emit resetCompleted();
    }

signals:
    void resetCompleted();
};
