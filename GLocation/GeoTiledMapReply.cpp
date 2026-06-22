#include "GeoTiledMapReply.h"
#include "GCSMapEngine.h"
#include "MapProvider.h"
#include "MapUrlEngine.h"

#include "log_category.h"

#include <QtCore/QFile>
#include <QtLocation/private/qgeotilespec_p.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkProxy>

// QByteArray GeoTiledMapReply::_bingNoTileImage;

GeoTiledMapReply::GeoTiledMapReply(QNetworkAccessManager *networkManager,
                                   const QNetworkRequest &request, const QGeoTileSpec &spec,
                                   QObject *parent)
    : QGeoTiledMapReply(spec, parent), _reply(nullptr), _request(request),
      _networkManager(networkManager) {

    if (_request.url().isEmpty()) {
        if (!_badMapbox.size()) {
            QFile b(":/res/map-notfound.jpg");
            if (b.open(QFile::ReadOnly)) {
                _badMapbox = b.readAll();
            }
        }

        setMapImageData(_badMapbox);
        setMapImageFormat("png");
        setFinished(true);
        setCached(false);
    }

    else {
        GCSFetchTileTask *task = getGCSMapEngine()->createFetchTileTask(
            getGCSMapEngine()->mapUrlEngine()->getProviderTypeFromQtMapId(spec.mapId()), spec.x(),
            spec.y(), spec.zoom());

        connect(task, &GCSFetchTileTask::tileFetched, this, &GeoTiledMapReply::cacheReply);
        connect(task, &GCSMapTask::error, this, &GeoTiledMapReply::cacheError);

        getGCSMapEngine()->addTask(task, spec.mapId());
    }
}

GeoTiledMapReply::~GeoTiledMapReply() { _clearReply(); }

void GeoTiledMapReply::_clearReply() {
    _timer.stop();
    if (_reply) {
        _reply->deleteLater();
        _reply = nullptr;
    }
}

void GeoTiledMapReply::abort() {
    _timer.stop();
    if (_reply)
        _reply->abort();
    emit aborted();
}

void GeoTiledMapReply::networkReplyFinished() {
    _timer.stop();
    if (!_reply) {
        emit aborted();
        return;
    }

    if (_reply->error() != QNetworkReply::NoError) {
        emit aborted();
        return;
    }

    QByteArray a = _reply->readAll();
    MapUrlEngine *mapUrlEngine = getGCSMapEngine()->mapUrlEngine();
    QString format = mapUrlEngine->getImageFormat(tileSpec().mapId(), a);

    MapProvider *mapProvider = mapUrlEngine->getMapProviderFromQtMapId(tileSpec().mapId());
    // if (mapProvider
    //     && mapProvider->isBingProvider()
    //     && a.size()
    //     && _bingNoTileImage.size()
    //     && a == _bingNoTileImage) {
    //     // Bing doesn't return an error if you request a tile above supported zoom level
    //     // It instead returns an image of a missing tile graphic. We need to detect that
    //     // and error out so Qt will deal with zooming correctly even if it doesn't have the tile.
    //     // This allows us to zoom up to level 23 even though the tiles don't actually exist
    //     setError(QGeoTiledMapReply::CommunicationError, "Bing tile above zoom level");
    // }
    // else
    {
        setMapImageData(a);
        if (!format.isEmpty()) {
            setMapImageFormat(format);
            getGCSMapEngine()->cacheTile(
                getGCSMapEngine()->mapUrlEngine()->getProviderTypeFromQtMapId(tileSpec().mapId()),
                tileSpec().x(), tileSpec().y(), tileSpec().zoom(), a, format);
        }
    }

    setFinished(true);
    _clearReply();
}

void GeoTiledMapReply::networkReplyError(QNetworkReply::NetworkError error) {
    _timer.stop();
    if (!_reply) {
        return;
    }

    if (error != QNetworkReply::OperationCanceledError) {
        COMMON_WARN("Fetch tile error  {0}", _reply->errorString().toStdString());
        setError(QGeoTiledMapReply::CommunicationError, _reply->errorString());
    }

    setFinished(true);
    _clearReply();
}

bool GeoTiledMapReply::internetState() {
    // if(QNetworkInformation::availableBackends().isEmpty()) return false;
    // if(!QNetworkInformation::loadDefaultBackend()) return false;
    // if(!QNetworkInformation::instance()->supports(QNetworkInformation::Feature::Reachability))
    // {
    //     if(!QNetworkInformation::loadBackendByFeatures(QNetworkInformation::Feature::Reachability))
    //     return false;
    // }
    // const QNetworkInformation::Reachability reachability =
    // QNetworkInformation::instance()->reachability(); return (reachability ==
    // QNetworkInformation::Reachability::Online);

    return true;
}

void GeoTiledMapReply::cacheError(GCSMapTask::TaskType type, QString errorString) {
    if (!internetState()) {
        setError(QGeoTiledMapReply::CommunicationError, "Network not available");
        setFinished(true);
    } else {
        if (type != GCSMapTask::taskFetchTile) {
            COMMON_WARN("wrong task");
        }

        QNetworkProxy proxy = _networkManager->proxy();
        QNetworkProxy tProxy;

        tProxy.setType(QNetworkProxy::DefaultProxy);
        _networkManager->setProxy(tProxy);

        _reply = _networkManager->get(_request);
        _reply->setParent(nullptr);

        connect(_reply, &QNetworkReply::finished, this, &GeoTiledMapReply::networkReplyFinished);
        connect(_reply, &QNetworkReply::errorOccurred, this, &GeoTiledMapReply::networkReplyError);

        _networkManager->setProxy(proxy);
        connect(&_timer, &QTimer::timeout, this, &GeoTiledMapReply::timeout);
        _timer.setSingleShot(true);
        _timer.start(5000);
    }
}

void GeoTiledMapReply::cacheReply(GCSCacheTile *tile) {
    setMapImageData(tile->img());
    setMapImageFormat(tile->format());
    setFinished(true);
    setCached(true);

    tile->deleteLater();
}

void GeoTiledMapReply::timeout() {
    if (_reply) {
        _reply->abort();
    }

    emit aborted();
}
