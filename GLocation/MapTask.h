#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

class CachedTileSet;

class GCSTile
{
public:
    GCSTile()
        : _x(0)
        , _y(0)
        , _z(0)
        , _set(UINT64_MAX)
        , _type("Invalid")
    {
    }

    enum TyleState {
        StatePending = 0,
        StateDownloading,
        StateError,
        StateComplete
    };

    int                 x           () const { return _x; }
    int                 y           () const { return _y; }
    int                 z           () const { return _z; }
    qulonglong          set         () const { return _set;  }
    const QString       hash        () const { return _hash; }
    QString             type        () const { return _type; }

    void                setX        (int x) { _x = x; }
    void                setY        (int y) { _y = y; }
    void                setZ        (int z) { _z = z; }
    void                setTileSet  (qulonglong set) { _set = set;  }
    void                setHash     (const QString& hash) { _hash = hash; }
    void                setType     (QString type) { _type = type; }

private:
    int         _x;
    int         _y;
    int         _z;
    qulonglong  _set;
    QString     _hash;
    QString     _type;
};

class GCSCacheTile : public QObject
{
    Q_OBJECT
public:
    GCSCacheTile (const QString& hash, const QByteArray& img,
                 const QString& format, const QString& type, qulonglong set = UINT64_MAX)
        : _set(set)
        , _hash(hash)
        , _img(img)
        , _format(format)
        , _type(type)
    {
    }

    GCSCacheTile    (const QString hash, qulonglong set)
        : _set(set)
        , _hash(hash)
    {
    }

    qulonglong          set     () const{ return _set;   }
    QString             hash    () { return _hash;  }
    QByteArray          img     () { return _img;   }
    QString             format  () { return _format;}
    QString             type    () { return _type; }

private:

    qulonglong  _set;
    QString     _hash;
    QByteArray  _img;
    QString     _format;
    QString     _type;
};


class GCSMapTask : public QObject
{
    Q_OBJECT
public:

    enum TaskType {
        taskInit,
        taskCacheTile,
        taskFetchTile,
        taskFetchTileSets,
        taskCreateTileSet,
        taskGetTileDownloadList,
        taskUpdateTileDownloadState,
        taskDeleteTileSet,
        taskPruneCache,
        taskReset
    };

    GCSMapTask(TaskType type)
        : _type(type)
    {

    }

    virtual ~GCSMapTask()
    {

    }

    virtual TaskType type () { return _type; }

    void setError(QString errorString = QString())
    {
        emit error(_type, errorString);
    }

signals:
    void error (GCSMapTask::TaskType type, QString errorString);

private:
    TaskType    _type;
};


class GCSFetchTileSetTask : public GCSMapTask
{
    Q_OBJECT
public:
    GCSFetchTileSetTask()
        : GCSMapTask(GCSMapTask::taskFetchTileSets)
    {

    }

    void setTileSetFetched(CachedTileSet* tileSet)
    {
        emit tileSetFetched(tileSet);
    }

signals:
    void  tileSetFetched  (CachedTileSet* tileSet);
};

class GCSFetchTileTask : public GCSMapTask
{
    Q_OBJECT
public:
    GCSFetchTileTask(const QString hash)
        : GCSMapTask(GCSMapTask::taskFetchTile)
        , _hash(hash)
    {

    }

    ~GCSFetchTileTask()
    {
    }

    void setTileFetched(GCSCacheTile* tile)
    {
        emit tileFetched(tile);
    }

    QString hash() { return _hash; }

signals:
    void tileFetched (GCSCacheTile* tile);

private:
    QString _hash;
};
