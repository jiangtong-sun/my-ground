#include "GeoTileFetcher.h"
#include "GCSMapEngine.h"

#include "GeoTiledMapReply.h"
#include "MapUrlEngine.h"

#include <QtNetwork/QNetworkRequest>
#include <QtLocation/private/qgeotilespec_p.h>
#include <QtLocation/private/qgeotiledmappingmanagerengine_p.h>

GeoTileFetcher::GeoTileFetcher(QGeoTiledMappingManagerEngine *parent)
    : QGeoTileFetcher(parent)
    , _networkManager(new QNetworkAccessManager(this))
{

}

GeoTileFetcher::~GeoTileFetcher()
{

}

QGeoTiledMapReply* GeoTileFetcher::getTileImage(const QGeoTileSpec &spec)
{
    QNetworkRequest request = getGCSMapEngine()->mapUrlEngine()->getTileURL(
        spec.mapId(), spec.x(), spec.y(), spec.zoom());

    if ( ! request.url().isEmpty() ) {
        return new GeoTiledMapReply(_networkManager, request, spec);
    }
    else {
        return nullptr;
    }
}
