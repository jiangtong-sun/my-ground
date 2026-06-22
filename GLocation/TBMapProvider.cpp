#include "TBMapProvider.h"
// #include "log_category.h"
QString BingMapProvider::_getURL(int x, int y, int zoom) {
    const QString key = _tileXYToQuadKey(x, y, zoom);
    QString rtn = _mapUrl.arg(_getServerNum(x, y, 4))
                      .arg(_mapName, key, _imageFormat, _versionBingMaps, _language);
    return rtn;
}

QString TiandiMapProvider::_getURL(int x, int y, int zoom) {
    QString rtn = _mapUrl.arg(_mapName, _uinfo).arg(zoom).arg(y).arg(x).arg(_passKey);
    // if (logprint_freq++ % 10 == 0) {
    //     COMMON_INFO("mapurl: {}", rtn.toStdString());
    // }
    return rtn;
}
