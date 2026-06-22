#include "CachedTileSet.h"
#include "GCSMapEngine.h"
#include "MapEngineData.h"
#include "MapUrlEngine.h"
#include "log_category.h"

#include <QtNetwork/QNetworkProxy>

#define TILE_BATCH_SIZE 256

CachedTileSet::CachedTileSet(const QString &name)
    : _name(name), _topleftLat(0.0), _topleftLon(0.0), _bottomRightLat(0.0), _bottomRightLon(0.0),
      _totalTileCount(0), _totalTileSize(0), _uniqueTileCount(0), _uniqueTileSize(0),
      _savedTileCount(0), _savedTileSize(0), _minZoom(3), _maxZoom(3), _defaultSet(false),
      _deleting(false), _downloading(false), _id(0), _type("Invalid"), _errorCount(0),
      _networkManager(nullptr), _noMoreTiles(false), _batchRequested(false), _selected(false) {}

CachedTileSet::~CachedTileSet() {
    delete _networkManager;
    _networkManager = nullptr;
}

QString CachedTileSet::errorCountStr() const { return GCSMapEngine::numberToString(_errorCount); }

QString CachedTileSet::totalTileCountStr() const {
    return GCSMapEngine::numberToString(_totalTileCount);
}

QString CachedTileSet::totalTilesSizeStr() const {
    return GCSMapEngine::bigSizeToString(_totalTileSize);
}

QString CachedTileSet::uniqueTileSizeStr() const {
    return GCSMapEngine::bigSizeToString(_uniqueTileSize);
}

QString CachedTileSet::uniqueTileCountStr() const {
    return GCSMapEngine::numberToString(_uniqueTileCount);
}

QString CachedTileSet::savedTileCountStr() const {
    return GCSMapEngine::numberToString(_savedTileCount);
}

QString CachedTileSet::savedTileSizeStr() const {
    return GCSMapEngine::bigSizeToString(_savedTileSize);
}

QString CachedTileSet::downloadStatus() {
    if (_defaultSet) {
        return totalTilesSizeStr();
    }

    if (_totalTileCount <= _savedTileCount) {
        return savedTileSizeStr();
    } else {
        return savedTileSizeStr() + " / " + totalTilesSizeStr();
    }
}

void CachedTileSet::createDownloadTask() {
    if (!_downloading) {
        _errorCount = 0;
        _downloading = true;
        _noMoreTiles = false;
        emit downloadingChanged();
        emit errorCountChanged();
    }

    TileDownloadListTask *task = new TileDownloadListTask(_id, TILE_BATCH_SIZE);

    connect(task, &TileDownloadListTask::tileListFetched, this, &CachedTileSet::_tileListFetched);

    getGCSMapEngine()->addTask(task, 0);

    emit totalTileCountChanged();
    emit totalTilesSizeChanged();

    _batchRequested = true;
}

void CachedTileSet::resumeDownloadTask() {
    TileDownloadStateTask *task = new TileDownloadStateTask(_id, GCSTile::StatePending, "*");
    getGCSMapEngine()->addTask(task, 0);
    createDownloadTask();
}

void CachedTileSet::cancelDownloadTask() {
    if (_downloading) {
        _downloading = false;
        emit downloadingChanged();
    }
}

void CachedTileSet::_tileListFetched(QList<GCSTile *> tiles) {
    _batchRequested = false;

    if (tiles.size() < TILE_BATCH_SIZE) {
        _noMoreTiles = true;
    }
    if (!tiles.size()) {
        _doneWithDownload();
        return;
    }

    if (!_networkManager) {
        _networkManager = new QNetworkAccessManager(this);
    }

    _tilesToDownload += tiles;

    _prepareDownload();
}

void CachedTileSet::_doneWithDownload() {
    if (!_errorCount) {
        _totalTileCount = _savedTileCount;
        _totalTileSize = _savedTileSize;

        quint32 avg;
        if (_savedTileSize != 0) {
            avg = _savedTileSize / _savedTileCount;
        } else {
            COMMON_WARN("_savedTileSize == 0");
            avg = 0;
        }

        _uniqueTileSize = _uniqueTileCount * avg;
    }

    emit totalTileCountChanged();
    emit totalTilesSizeChanged();
    emit savedTileSizeChanged();
    emit savedTileCountChanged();
    emit uniqueTileSizeChanged();

    _downloading = false;

    emit downloadingChanged();
    emit completeChanged();
}

void CachedTileSet::_prepareDownload() {
    if (!_tilesToDownload.count()) {

        if (_noMoreTiles) {
            _doneWithDownload();
        } else {
            if (!_batchRequested)
                createDownloadTask();
        }
        return;
    }

    for (int i = _replies.count(); i < 8; i++) {
        if (_tilesToDownload.count()) {

            GCSTile *tile = _tilesToDownload.first();
            _tilesToDownload.removeFirst();

            QNetworkRequest request = getGCSMapEngine()->mapUrlEngine()->getTileURL(
                tile->type(), tile->x(), tile->y(), tile->z());

            request.setAttribute(QNetworkRequest::User, tile->hash());

            QNetworkProxy proxy = _networkManager->proxy();
            QNetworkProxy tProxy;
            tProxy.setType(QNetworkProxy::DefaultProxy);
            _networkManager->setProxy(tProxy);

            QNetworkReply *reply = _networkManager->get(request);
            reply->setParent(0);

            const bool sslLibraryBuildIs1x =
                (QSslSocket::sslLibraryBuildVersionNumber() & 0xf0000000) == 0x10000000;

            const bool sslLibraryIs3x =
                (QSslSocket::sslLibraryVersionNumber() & 0xf0000000) == 0x30000000;

            if (sslLibraryBuildIs1x && sslLibraryIs3x) {

                COMMON_WARN("Ignoring ssl certificates due to OpenSSL version mismatch");

                QList<QSslError> errorsThatCanBeIgnored;
                errorsThatCanBeIgnored << QSslError(QSslError::NoPeerCertificate);
                reply->ignoreSslErrors(errorsThatCanBeIgnored);
            }

            connect(reply, &QNetworkReply::finished, this, &CachedTileSet::_networkReplyFinished);
            connect(reply, &QNetworkReply::errorOccurred, this, &CachedTileSet::_networkReplyError);
            _replies.insert(tile->hash(), reply);

            _networkManager->setProxy(proxy);

            delete tile;
            if (!_batchRequested && !_noMoreTiles && _tilesToDownload.count() < (8 * 10)) {
                createDownloadTask();
            }
        }
    }
}

void CachedTileSet::_networkReplyFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(QObject::sender());
    if (!reply) {
        COMMON_WARN("NULL Reply");
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        const QString hash = reply->request().attribute(QNetworkRequest::User).toString();
        if (!hash.isEmpty()) {

            if (_replies.contains(hash)) {
                _replies.remove(hash);
            } else {
                COMMON_WARN("Tile not in list {0} ", hash.toStdString());
            }

            QByteArray image = reply->readAll();
            QString type = getGCSMapEngine()->tileHashToType(hash);
            QString format = getGCSMapEngine()->mapUrlEngine()->getImageFormat(type, image);

            if (!format.isEmpty()) {

                //-- Cache tile
                getGCSMapEngine()->cacheTile(type, hash, image, format, _id);
                TileDownloadStateTask *task =
                    new TileDownloadStateTask(_id, GCSTile::StateComplete, hash);
                getGCSMapEngine()->addTask(task, 0);

                //-- Updated cached (downloaded) data
                _savedTileSize += image.size();
                _savedTileCount++;
                emit savedTileSizeChanged();
                emit savedTileCountChanged();

                //-- Update estimate
                if (_savedTileCount % 10 == 0) {
                    quint32 avg = _savedTileSize / _savedTileCount;
                    _totalTileSize = avg * _totalTileCount;
                    _uniqueTileSize = avg * _uniqueTileCount;
                    emit totalTilesSizeChanged();
                    emit uniqueTileSizeChanged();
                }
            }

            _prepareDownload();
        } else {
            COMMON_WARN("Empty Hash");
        }
    }
    reply->deleteLater();
}

void CachedTileSet::_networkReplyError(QNetworkReply::NetworkError error) {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(QObject::sender());
    if (!reply) {
        return;
    }

    _errorCount++;
    emit errorCountChanged();
    QString hash = reply->request().attribute(QNetworkRequest::User).toString();

    COMMON_INFO("Error fetching tile {0}", reply->errorString().toStdString());

    if (!hash.isEmpty()) {

        if (_replies.contains(hash)) {
            _replies.remove(hash);
        } else {
            COMMON_WARN("Reply not in list {0} ", hash.toStdString());
        }

        if (error != QNetworkReply::OperationCanceledError) {
            COMMON_WARN("Error {0}", reply->errorString().toStdString());
        }

        TileDownloadStateTask *task = new TileDownloadStateTask(_id, GCSTile::StateError, hash);
        getGCSMapEngine()->addTask(task, 0);
    } else {
        COMMON_WARN("Empty Hash");
    }

    _prepareDownload();
    reply->deleteLater();
}

void CachedTileSet::setSelected(bool sel) {
    _selected = sel;
    emit selectedChanged();
}
