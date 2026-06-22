#pragma once

#include <QtLocation/private/qgeotilefetcher_p.h>

class QGeoTiledMappingManagerEngine;
class QNetworkAccessManager;

class GeoTileFetcher : public QGeoTileFetcher
{
    Q_OBJECT
public:
    explicit GeoTileFetcher (QGeoTiledMappingManagerEngine *parent = nullptr);
    ~GeoTileFetcher();
private:
    QGeoTiledMapReply* getTileImage (const QGeoTileSpec &spec);
private:
    QNetworkAccessManager*  _networkManager;
};
