
#pragma once

#include <QtNetwork/QNetworkReply>
#include <QtLocation/private/qgeotiledmapreply_p.h>
#include <QtCore/QTimer>
#include "MapTask.h"

class GeoTiledMapReply : public QGeoTiledMapReply
{
    Q_OBJECT
public:
    GeoTiledMapReply(QNetworkAccessManager*  networkManager,
                         const QNetworkRequest& request,
                         const QGeoTileSpec &spec,
                         QObject *parent = 0);
    ~GeoTiledMapReply();
    bool internetState();
    void abort();

private slots:
    void networkReplyFinished   ();
    void networkReplyError      (QNetworkReply::NetworkError error);
    void cacheReply             (GCSCacheTile* tile);
    void cacheError             (GCSMapTask::TaskType type, QString errorString);
    void timeout                ();

private:
    void _clearReply            ();

private:
    QNetworkReply*          _reply;
    QNetworkRequest         _request;
    QNetworkAccessManager*  _networkManager;
    QByteArray              _badMapbox;
    QByteArray              _badTile;
    QTimer                  _timer;
    static QByteArray _bingNoTileImage;
};
