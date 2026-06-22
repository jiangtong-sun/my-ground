#pragma once

#include "MapProvider.h"

class BingMapProvider : public MapProvider
{
    Q_OBJECT

protected:
    BingMapProvider(const QString &mapName,
                    const QString &imageFormat,
                    QGeoMapType::MapStyle mapStyle,
                    QObject* parent = nullptr)
        : MapProvider(QStringLiteral("https://www.bing.com/maps/"), imageFormat, mapStyle, parent)
        , _mapName(mapName) {}

public:
    bool isBingProvider() const final { return true; }

private:
    QString _getURL(int x, int y, int zoom) final;

    const QString _mapName;
    const QString _mapUrl = QStringLiteral("http://ecn.t%1.tiles.virtualearth.net/tiles/%2%3.%4?g=%5&mkt=%6");
    const QString _versionBingMaps = QStringLiteral("2981");
};

class BingRoadMapProvider : public BingMapProvider
{
    Q_OBJECT

public:
    BingRoadMapProvider(QObject* parent = nullptr)
        : BingMapProvider(QStringLiteral("r"), QStringLiteral("png"), QGeoMapType::StreetMap, parent) {}
};

class BingSatelliteMapProvider : public BingMapProvider
{
    Q_OBJECT

public:
    BingSatelliteMapProvider(QObject* parent = nullptr)
        : BingMapProvider(QStringLiteral("a"), QStringLiteral("jpg"), QGeoMapType::SatelliteMapDay, parent) {}
};


class TiandiMapProvider: public MapProvider{
    Q_OBJECT

protected:
    TiandiMapProvider(const QString &mapName,
                    const QString &imageFormat,
                    QGeoMapType::MapStyle mapStyle,
                    QObject* parent = nullptr)
        : MapProvider(QStringLiteral("t0.tianditu.gov.cn"), imageFormat, mapStyle, parent)
        , _mapName(mapName) {}

private:
    QString _getURL(int x, int y, int zoom) final;

    int logprint_freq = 0;
    const QString _mapName;
    const QString _passKey = "a43ab498ba00e16e00e7ec0f91e63b6c";
    const QString _uinfo = "REQUEST=GetTile&VERSION=1.0.0&LAYER=img&STYLE=default&TILEMATRIXSET=w&FORMAT=tiles";
    const QString _mapUrl = QStringLiteral("http://t0.tianditu.gov.cn/%1/wmts?SERVICE=WMTS&%2&TILEMATRIX=%3&TILEROW=%4&TILECOL=%5&tk=%6");
};


class TiandiRoadMapProvider : public TiandiMapProvider
{
    Q_OBJECT

public:
    TiandiRoadMapProvider(QObject* parent = nullptr)
        : TiandiMapProvider(QStringLiteral("vec_w"), QStringLiteral("png"), QGeoMapType::StreetMap, parent) {}
};

class TiandituSatelliteMapProvider : public TiandiMapProvider
{
    Q_OBJECT

public:
    TiandituSatelliteMapProvider(QObject* parent = nullptr)
        : TiandiMapProvider(QStringLiteral("img_w"), QStringLiteral("jpg"), QGeoMapType::SatelliteMapDay, parent) {}
};
