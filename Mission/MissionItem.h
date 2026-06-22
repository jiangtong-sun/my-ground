#pragma once

#include <QGeoCoordinate>
#include <QJsonObject>
#include <QObject>
#include <QVariant>

#pragma pack(push, 1)
struct SimpleWaypoint {
    uint8_t absNo = 0;
    int32_t lng = 0;
    int32_t lat = 0;
    int32_t alt = 0;
};
#pragma pack(pop)

#define LNG_LAT_PRECISION  0.0000001
#define ALTITUDE_PRECISION 0.01
#define MISSION_PRECISION2 0.1

class QMissionItem : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(QMissionItem)

  public:
    QMissionItem();
    Q_PROPERTY(QGeoCoordinate pos READ getPos WRITE setPos NOTIFY positionChanged)
    Q_PROPERTY(double lat READ getLat WRITE setLat NOTIFY positionChanged)
    Q_PROPERTY(double lng READ getLng WRITE setLng NOTIFY positionChanged)
    Q_PROPERTY(double alt READ getAlt WRITE setAlt NOTIFY positionChanged)
    Q_PROPERTY(QString latStr READ latStr NOTIFY positionChanged)
    Q_PROPERTY(QString lngStr READ lngStr NOTIFY positionChanged)
    Q_PROPERTY(QString altStr READ altStr NOTIFY positionChanged)
    Q_PROPERTY(int seq READ getSeq CONSTANT)

    virtual QJsonObject item2Json();
    virtual void json2Item(const QJsonObject &);
    virtual void notifyChanged() { emit positionChanged(); }

    Q_INVOKABLE void updatePosition(QGeoCoordinate coor);

    QString latStr() { return QString::number(getLat(), 'f', 7); }
    QString lngStr() { return QString::number(getLng(), 'f', 7); }
    QString altStr() { return QString::number(getAlt(), 'f', 2); }
    double getLng(void) const { return simple.lng * LNG_LAT_PRECISION; }
    double getLat(void) const { return simple.lat * LNG_LAT_PRECISION; }
    double getAlt(void) const { return simple.alt * ALTITUDE_PRECISION; }

    int getSeq(void) const { return seq; }

    void setLng(double v);
    void setLat(double v);
    void setAlt(double v);
    void setSeq(int v) { seq = v; }

    QString toString() const;
    const SimpleWaypoint *pos() const { return &simple; }
    SimpleWaypoint &refPos() { return simple; }
    void setPos(const QGeoCoordinate &coor);
    QGeoCoordinate getPos() const;

  signals:
    void positionChanged();
    void altModeChanged();
    void updatePosNotify();

  protected:
    SimpleWaypoint simple;
    uint8_t seq = 0;
};
