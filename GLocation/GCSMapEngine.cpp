#include "GCSMapEngine.h"
#include "CachedTileSet.h"
#include "MapEngineData.h"
#include "MapUrlEngine.h"
#include "log_category.h"
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

Q_DECLARE_METATYPE(GCSMapTask::TaskType)
Q_DECLARE_METATYPE(GCSTile)
Q_DECLARE_METATYPE(QList<GCSTile *>)

GCSMapEngine *GCSMapEngine::instance() {
    static GCSMapEngine s_mapEngine;
    return &s_mapEngine;
}

GCSMapEngine *getGCSMapEngine() { return GCSMapEngine::instance(); }

GCSMapEngine::GCSMapEngine()
    : _MapUrlEngine(new MapUrlEngine()), _workerA(QStringLiteral("AMapWorkerSession")),
      _workerB(QStringLiteral("BMapWorkerSession")), _userAgent("Mozilla/5.0"), _mapType(true), _prunning(false),
      _cacheWasReset(false) {
    qRegisterMetaType<GCSMapTask::TaskType>();
    qRegisterMetaType<GCSTile>();
    qRegisterMetaType<QList<GCSTile *>>();

    connect(&_workerA, &TileCacheWorker::updateTotals, this, &GCSMapEngine::_updateTotals);
    connect(&_workerB, &TileCacheWorker::updateTotals, this, &GCSMapEngine::_updateTotals);
}

GCSMapEngine::~GCSMapEngine() {
    _workerA.quit();
    _workerA.wait();
    _workerB.quit();
    _workerB.wait();
    delete _MapUrlEngine;
    _MapUrlEngine = nullptr;
}

void GCSMapEngine::init() {
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    cacheDir.append(QStringLiteral("/GCSMapCache"));

    if (!QDir::root().mkpath(cacheDir)) {
        COMMON_WARN("Could not create mapping disk cache directory  {0}", cacheDir.toStdString());
        cacheDir = QDir::currentPath() + QStringLiteral("/GCSMapCache");

        if (!QDir::root().mkpath(cacheDir)) {
            COMMON_WARN("Could not create mapping disk cache directory  {0}", cacheDir.toStdString());
            cacheDir.clear();
        }
    }

    _cachePath = cacheDir;

    if (!_cachePath.isEmpty()) {
        QString _cacheFile = "AMapCache.db";
        _workerA.setDatabaseFile(_cachePath + "/" + _cacheFile);
        COMMON_INFO("map cache  {0} {1}", _cachePath.toStdString(), _cacheFile.toStdString());
        GCSMapTask *task = new GCSMapTask(GCSMapTask::taskInit);
        _workerA.enqueueTask(task);
    } else {
        COMMON_CRITICAL("Could not find suitable map cache directory");
    }

    if (!_cachePath.isEmpty()) {
        QString _cacheFile = "BMapCache.db";
        _workerB.setDatabaseFile(_cachePath + "/" + _cacheFile);
        COMMON_INFO("map cache  {0} {1}", _cachePath.toStdString(), _cacheFile.toStdString());
        GCSMapTask *task = new GCSMapTask(GCSMapTask::taskInit);
        _workerB.enqueueTask(task);
    } else {
        COMMON_CRITICAL("Could not find suitable map cache directory");
    }
}

void GCSMapEngine::addTask(GCSMapTask *task, int type) {
    Q_UNUSED(type);
    if (_mapType) {
        _workerB.enqueueTask(task);
    } else {
        _workerA.enqueueTask(task);
    }
}

void GCSMapEngine::cacheTile(const QString &type, int x, int y, int z, const QByteArray &image, const QString &format,
                             qulonglong set) {
    QString hash = getTileHash(type, x, y, z);
    cacheTile(type, hash, image, format, set);
}

void GCSMapEngine::cacheTile(const QString &type, const QString &hash, const QByteArray &image, const QString &format,
                             qulonglong set) {
    SaveTileTask *task = new SaveTileTask(new GCSCacheTile(hash, image, format, type, set));
    if (_mapType) {
        _workerB.enqueueTask(task);
    } else {
        _workerA.enqueueTask(task);
    }
}

QString GCSMapEngine::getTileHash(const QString &type, int x, int y, int z) {
    int hash = mapUrlEngine()->hashFromProviderType(type);
    return QString::asprintf("%010d%08d%08d%03d", hash, x, y, z);
}

QString GCSMapEngine::tileHashToType(const QString &tileHash) {
    int providerHash = tileHash.mid(0, 10).toInt();
    return mapUrlEngine()->providerTypeFromHash(providerHash);
}

GCSFetchTileTask *GCSMapEngine::createFetchTileTask(const QString &type, int x, int y, int z) {
    QString hash = getTileHash(type, x, y, z);
    GCSFetchTileTask *task = new GCSFetchTileTask(hash);
    return task;
}

GCSTileSet GCSMapEngine::getTileCount(int zoom, double topleftLon, double topleftLat, double bottomRightLon,
                                      double bottomRightLat, const QString &mapType) {
    if (zoom < 1)
        zoom = 1;
    if (zoom > 20)
        zoom = 20;
    return getGCSMapEngine()->mapUrlEngine()->getTileCount(zoom, topleftLon, topleftLat, bottomRightLon, bottomRightLat,
                                                           mapType);
}

QStringList GCSMapEngine::getMapNameList() { return getGCSMapEngine()->mapUrlEngine()->getProviderTypes(); }

quint32 GCSMapEngine::getMaxDiskCache() { return 5 * 1024; }

quint32 GCSMapEngine::getMaxMemCache() { return 50; }

QString GCSMapEngine::bigSizeToString(quint64 size) {
    quint64 val = 1024;
    val *= 1024 * 1024 * 1024;

    if (size < 1024)
        return QString("%d").arg(size);
    else if (size < 1024 * 1024)
        return QString("%dkB").arg(size / 1024);
    else if (size < 1024 * 1024 * 1024)
        return QString("%dMB").arg(size / (1024 * 1024));
    else if (size < val)
        return QString("%dGB").arg(size / (1024 * 1024 * 1024));
    else
        return QString("%ldTB").arg(size / val);
}

QString GCSMapEngine::storageFreeSizeToString(quint64 size_MB) {
    if (size_MB < 1024)
        return QString("%dMB").arg(size_MB);
    else if (size_MB < 1024.0 * 1024.0)
        return QString("%dGB").arg(size_MB / 1024);
    else
        return QString("%dTB").arg(size_MB / (1024 * 1024));
}

QString GCSMapEngine::numberToString(quint64 number) { return QString("%ld").arg(number); }

void GCSMapEngine::_updateTotals(quint32 totaltiles, quint64 totalsize, quint32 defaulttiles, quint64 defaultsize) {
    emit updateTotals(totaltiles, totalsize, defaulttiles, defaultsize);
    quint64 maxSize = static_cast<quint64>(getMaxDiskCache()) * 1024L * 1024L;
    if (!_prunning && defaultsize > maxSize) {
        _prunning = true;
        PruneCacheTask *task = new PruneCacheTask(defaultsize - maxSize);
        connect(task, &PruneCacheTask::pruned, this, &GCSMapEngine::_pruned);
        getGCSMapEngine()->addTask(task, 0);
    }
}

void GCSMapEngine::_pruned() { _prunning = false; }

CreateTileSetTask::~CreateTileSetTask() {
    if (!_saved && _tileSet)
        delete _tileSet;
}
