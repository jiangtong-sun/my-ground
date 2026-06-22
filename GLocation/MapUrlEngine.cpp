#include "MapUrlEngine.h"
#include "TBMapProvider.h"
#include "log_category.h"

MapUrlEngine::MapUrlEngine() : _timeout(5 * 1000) {
    _providers.append(ProviderPair("Tianditu Satellite", new TiandituSatelliteMapProvider(this)));
    _providers.append(ProviderPair("Tianditu Street", new TiandiRoadMapProvider(this)));
    // _providers.append(ProviderPair("Bing Satellite", new BingSatelliteMapProvider(this)));
    // _providers.append(ProviderPair("Bing Road", new BingRoadMapProvider(this)));
}

MapUrlEngine::~MapUrlEngine() {}

QString MapUrlEngine::getImageFormat(int qtMapId, const QByteArray &image) {
    MapProvider *provider = getMapProviderFromQtMapId(qtMapId);

    if (provider) {
        return provider->getImageFormat(image);
    } else {
        COMMON_WARN("map id not found {0}", qtMapId);
        return "";
    }
}

QString MapUrlEngine::getImageFormat(const QString &type, const QByteArray &image) {
    MapProvider *provider = getMapProviderFromProviderType(type);

    if (provider) {
        return provider->getImageFormat(image);
    } else {
        COMMON_WARN("type not found  {0}", type.toStdString());
        return "";
    }
}
QNetworkRequest MapUrlEngine::getTileURL(int qtMapId, int x, int y, int zoom) {
    MapProvider *provider = getMapProviderFromQtMapId(qtMapId);
    if (provider) {
        return provider->getTileURL(x, y, zoom);
    } else {
        COMMON_WARN("map id not found  {0}", qtMapId);
        return QNetworkRequest(QUrl());
    }
}

QNetworkRequest MapUrlEngine::getTileURL(const QString &type, int x, int y, int zoom) {
    MapProvider *provider = getMapProviderFromProviderType(type);
    if (provider) {
        return provider->getTileURL(x, y, zoom);
    } else {
        COMMON_WARN("type not found  {0}", type.toStdString());
        return QNetworkRequest(QUrl());
    }
}

QString MapUrlEngine::getProviderTypeFromQtMapId(int qtMapId) {
    if (qtMapId >= 1 && qtMapId <= _providers.count()) {
        return _providers.at(qtMapId - 1).first;
    } else {
        COMMON_WARN("map id not found  {0}", qtMapId);
        return _providers.at(0).first;
    }
}

MapProvider *MapUrlEngine::getMapProviderFromProviderType(const QString &type) {
    for (qsizetype i = 0; i < _providers.count(); i++) {
        if (_providers.at(i).first == type) {
            return _providers.at(i).second;
        }
    }

    return nullptr;
}

MapProvider *MapUrlEngine::getMapProviderFromQtMapId(int qtMapId) {
    if (qtMapId >= 1 && qtMapId <= _providers.count()) {
        return _providers.at(qtMapId - 1).second;
    } else {
        return nullptr;
    }
}

int MapUrlEngine::getQtMapIdFromProviderType(const QString &type) {
    for (qsizetype i = 0; i < _providers.count(); i++) {
        if (_providers.at(i).first == type) {
            return i + 1;
        }
    }

    COMMON_WARN("type not found  {0}", type.toStdString());
    return 1;
}

int MapUrlEngine::long2tileX(const QString &mapType, double lon, int z) {
    MapProvider *provider = getMapProviderFromProviderType(mapType);
    if (provider) {
        return provider->long2tileX(lon, z);
    } else {
        COMMON_WARN("type not found  {0}", mapType.toStdString());
        return 0;
    }
}

int MapUrlEngine::lat2tileY(const QString &mapType, double lat, int z) {
    MapProvider *provider = getMapProviderFromProviderType(mapType);
    if (provider) {
        return provider->lat2tileY(lat, z);
    } else {
        COMMON_WARN("type not found  {0}", mapType.toStdString());
        return 0;
    }
}

GCSTileSet MapUrlEngine::getTileCount(int zoom, double topleftLon, double topleftLat,
                                      double bottomRightLon, double bottomRightLat,
                                      const QString &mapType) {
    MapProvider *provider = getMapProviderFromProviderType(mapType);
    if (provider) {
        return provider->getTileCount(zoom, topleftLon, topleftLat, bottomRightLon, bottomRightLat);
    } else {
        COMMON_WARN("type not found  {0}", mapType.toStdString());
        return GCSTileSet();
    }
}

QStringList MapUrlEngine::getProviderTypes() {
    QStringList types;
    for (qsizetype i = 0; i < _providers.count(); i++) {
        types.append(_providers.at(i).first);
    }

    return types;
}

int MapUrlEngine::hashFromProviderType(const QString &type) { return (int) (qHash(type) >> 1); }

QString MapUrlEngine::providerTypeFromHash(int hash) {
    for (qsizetype i = 0; i < _providers.count(); i++) {
        if (int(qHash(_providers.at(i).first) >> 1) == hash) {
            return _providers.at(i).first;
        }
    }

    COMMON_WARN("provider not found from hash  {0}", hash);
    return "";
}
