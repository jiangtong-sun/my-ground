#pragma once

#include "GCSTileSet.h"
#include "MapTask.h"
#include "TileCacheWorker.h"
#include <QtCore/QString>

class MapUrlEngine;

class GCSMapEngine : public QObject
{
    Q_OBJECT
public:
    GCSMapEngine                ();
    ~GCSMapEngine               ();

    static GCSMapEngine* instance();

    void                        init                ();
    void                        addTask             (GCSMapTask *task, int type);
    void                        cacheTile           (const QString& type, int x, int y, int z, const QByteArray& image, const QString& format, qulonglong set = UINT64_MAX);
    void                        cacheTile           (const QString& type, const QString& hash, const QByteArray& image, const QString& format, qulonglong set = UINT64_MAX);
    GCSFetchTileTask*           createFetchTileTask (const QString& type, int x, int y, int z);
    QStringList                 getMapNameList      ();
    const QString               userAgent           () { return _userAgent; }
    void                        setUserAgent        (const QString& ua) { _userAgent = ua; }
    QString                     tileHashToType      (const QString& tileHash);
    QString                     getTileHash         (const QString& type, int x, int y, int z);
    quint32                     getMaxDiskCache     (); //5GB
    quint32                     getMaxMemCache      (); //50MB
    void                        setMaxMemCache      (quint32 size);
    const QString               getCachePath        () { return _cachePath; }
    void                        testInternet        ();
    bool                        wasCacheReset       () const{ return _cacheWasReset; }
    void                        resetMapType        (bool va) {_mapType = va;}

    MapUrlEngine*               mapUrlEngine        () { return _MapUrlEngine; }

    static GCSTileSet           getTileCount        (int zoom, double topleftLon, double topleftLat, double bottomRightLon, double bottomRightLat, const QString& mapType);
    static QString              getTypeFromName     (const QString& name);
    static QString              bigSizeToString     (quint64 size);
    static QString              storageFreeSizeToString(quint64 size_MB);
    static QString              numberToString      (quint64 number);

private slots:
    void _updateTotals          (quint32 totaltiles, quint64 totalsize, quint32 defaulttiles, quint64 defaultsize);
    void _pruned                ();

signals:
    void updateTotals           (quint32 totaltiles, quint64 totalsize, quint32 defaulttiles, quint64 defaultsize);

private:
    MapUrlEngine*           _MapUrlEngine;
    TileCacheWorker          _workerA;
    TileCacheWorker          _workerB;
    QString                 _cachePath;
    QString                 _userAgent;
    bool                    _mapType;
    bool                    _prunning;
    bool                    _cacheWasReset;
};

extern GCSMapEngine*    getGCSMapEngine();
