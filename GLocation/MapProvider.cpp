#include "MapProvider.h"
#include <QtCore/QLocale>
#include <QtNetwork/QNetworkRequest>

MapProvider::MapProvider(
    const QString &referrer,
    const QString &imageFormat,
    QGeoMapType::MapStyle mapStyle,
    QObject* parent)
    : QObject(parent)
    , _referrer(referrer)
    , _imageFormat(imageFormat)
    , _mapStyle(mapStyle)
    , _language(!QLocale::system().uiLanguages().isEmpty() ? QLocale::system().uiLanguages().constFirst() : "en") {

}

QNetworkRequest MapProvider::getTileURL(int x, int y, int zoom)
{
    QNetworkRequest request;
    QString url = _getURL(x, y, zoom);
    if (url.isEmpty()) {
        return request;
    }
    request.setUrl(QUrl(url));
    request.setRawHeader(QByteArrayLiteral("Accept"),
                         QByteArrayLiteral("text/html,image/webp,image/apng,*/*"));
    request.setRawHeader(QByteArrayLiteral("Accept-Encoding"), QByteArrayLiteral("gzip"));
    request.setRawHeader(QByteArrayLiteral("User-Agent"), QByteArrayLiteral("Mozilla/5.0"));
    request.setRawHeader(QByteArrayLiteral("Connection"), QByteArrayLiteral("keep-alive"));
    request.setRawHeader(QByteArrayLiteral("Host"), _referrer.toUtf8());
    return request;
}

QString MapProvider::getImageFormat(QByteArray image) const
{
    if (image.size() < 3) {
        return QString();
    }

    static QByteArray pngSignature("\x89\x50\x4E\x47\x0D\x0A\x1A\x0A");
    if (image.startsWith(pngSignature)) {
        return QStringLiteral("png");
    }

    static QByteArray jpegSignature("\xFF\xD8\xFF");
    if (image.startsWith(jpegSignature)) {
        return QStringLiteral("jpg");
    }

    static QByteArray gifSignature("\x47\x49\x46\x38");
    if (image.startsWith(gifSignature)) {
        return QStringLiteral("gif");
    }

    return _imageFormat;
}

QString MapProvider::_tileXYToQuadKey(int tileX, int tileY, int levelOfDetail) const
{
    QString quadKey;
    for (int i = levelOfDetail; i > 0; i--) {
        char digit = '0';
        const int mask = 1 << (i - 1);

        if ((tileX & mask) != 0) {
            digit++;
        }
        if ((tileY & mask) != 0) {
            digit += 2;
        }

        (void) quadKey.append(digit);
    }

    return quadKey;
}

int MapProvider::_getServerNum(int x, int y, int max) const
{
    return (x + 2 * y) % max;
}

int MapProvider::long2tileX(double lon, int z) const
{
    return static_cast<int>(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}

int MapProvider::lat2tileY(double lat, int z) const
{
    static const double M_PI =  3.1415926;
    double lvalue = tan(lat * M_PI / 180.0);
    double cvalue = cos(lat * M_PI / 180.0);
    return static_cast<int>(floor((1.0 - log(lvalue + 1.0 / cvalue) / M_PI) / 2.0 * pow(2.0, z)));
}

GCSTileSet MapProvider::getTileCount(int zoom, double topleftLon,
                                     double topleftLat, double bottomRightLon,
                                     double bottomRightLat) const
{
    GCSTileSet set;
    set.tileX0 = long2tileX(topleftLon, zoom);
    set.tileY0 = lat2tileY(topleftLat, zoom);
    set.tileX1 = long2tileX(bottomRightLon, zoom);
    set.tileY1 = lat2tileY(bottomRightLat, zoom);
    set.tileCount = (static_cast<quint64>(set.tileX1) -
                     static_cast<quint64>(set.tileX0) + 1) *
                    (static_cast<quint64>(set.tileY1) -
                     static_cast<quint64>(set.tileY0) + 1);
    set.tileSize = 32768 * set.tileCount;
    return set;
}
