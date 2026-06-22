#pragma once

#include <QtLocation/private/qgeomaptype_p.h>
#include <QtCore/QByteArray>
#include <QtCore/QString>

#include "GCSTileSet.h"

class QNetworkRequest;

class MapProvider : public QObject
{
    Q_OBJECT

public:
    MapProvider(const QString& referrer, const QString& imageFormat,
                QGeoMapType::MapStyle mapStyle = QGeoMapType::CustomMap, QObject* parent = nullptr);

    QString getImageFormat(QByteArray image) const;

    QGeoMapType::MapStyle getMapStyle() const { return _mapStyle; }

    virtual QNetworkRequest getTileURL(int x, int y, int zoom);

    virtual int long2tileX(double lon, int z) const;

    virtual int lat2tileY(double lat, int z) const;

    virtual bool isBingProvider() const { return false; }

    virtual GCSTileSet getTileCount(int zoom, double topleftLon,
                                    double topleftLat, double bottomRightLon,
                                    double bottomRightLat) const;

protected:
    QString _tileXYToQuadKey(int tileX, int tileY, int levelOfDetail) const;
    int _getServerNum(int x, int y, int max) const;

    virtual QString _getURL(int x, int y, int zoom) = 0;

    const QString _referrer;
    const QString _imageFormat;
    const QGeoMapType::MapStyle _mapStyle;
    const QString _language;
};
