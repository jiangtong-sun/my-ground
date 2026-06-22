#pragma once

#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/private/qgeotiledmap_p.h>
#include <QtLocation/private/qgeotiledmappingmanagerengine_p.h>


class GCSGeoTiledMap : public QGeoTiledMap
{
    Q_OBJECT
public:
    GCSGeoTiledMap(QGeoTiledMappingManagerEngine *engine, QObject *parent = 0);
};

class GeoTileFetcher;

class GCSTiledMappingManagerEngine : public QGeoTiledMappingManagerEngine
{
    Q_OBJECT
public:
    GCSTiledMappingManagerEngine(const QVariantMap &parameters,
                                     QGeoServiceProvider::Error *error,
                                     QString *errorString);
    ~GCSTiledMappingManagerEngine();
    QGeoMap *createMap();

private:
    void _setCache(const QVariantMap &parameters);
};
