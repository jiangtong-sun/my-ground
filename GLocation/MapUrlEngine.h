#pragma once

#include "GCSTileSet.h"
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtNetwork/QNetworkRequest>

class MapProvider;

class MapUrlEngine : public QObject {
    Q_OBJECT

public:
    MapUrlEngine      ();
    ~MapUrlEngine     ();

    typedef QPair<QString, MapProvider*> ProviderPair;

    QNetworkRequest getTileURL          (const QString& type, int x, int y, int zoom);
    QNetworkRequest getTileURL          (int qtMapId, int x, int y, int zoom);

    QString         getImageFormat      (const QString& type, const QByteArray& image);
    QString         getImageFormat      (int qtMapId, const QByteArray& image);

    int long2tileX(const QString& mapType, double lon, int z);
    int lat2tileY (const QString& mapType, double lat, int z);

    QStringList     getProviderTypes                ();
    int             getQtMapIdFromProviderType      (const QString& type);
    QString         getProviderTypeFromQtMapId      (int qtMapId);
    MapProvider*    getMapProviderFromQtMapId       (int qtMapId);
    MapProvider*    getMapProviderFromProviderType  (const QString& type);
    int             hashFromProviderType            (const QString& type);
    QString         providerTypeFromHash            (int tileHash);

    GCSTileSet getTileCount(int zoom, double topleftLon, double topleftLat,
                            double bottomRightLon, double bottomRightLat,
                            const QString& mapType);

private:
    int                   _timeout;
    QList<ProviderPair>   _providers;
};
