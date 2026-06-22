#include "MissionItem.h"

QMissionItem::QMissionItem() {}

void QMissionItem::setLng(double v) {
    simple.lng = v / LNG_LAT_PRECISION;
    emit updatePosNotify();
    emit positionChanged();
}

void QMissionItem::setLat(double v) {
    simple.lat = v / LNG_LAT_PRECISION;
    emit updatePosNotify();
    emit positionChanged();
}

void QMissionItem::setAlt(double v) {
    simple.alt = v / ALTITUDE_PRECISION;
    emit updatePosNotify();
    emit positionChanged();
}

void QMissionItem::setPos(const QGeoCoordinate &coor) {
    simple.lng = coor.longitude() / LNG_LAT_PRECISION;
    simple.lat = coor.latitude() / LNG_LAT_PRECISION;
    simple.alt = coor.altitude() / ALTITUDE_PRECISION;
    emit positionChanged();
    emit updatePosNotify();
}

QGeoCoordinate QMissionItem::getPos() const {
    //
    return QGeoCoordinate(getLat(), getLng(), getAlt());
}

void QMissionItem::updatePosition(QGeoCoordinate coor) { setPos(coor); }

QString QMissionItem::toString() const {
    return QString("lat: %1, lng: %2, alt: %3, seq: %4")
        .arg(getLat())
        .arg(getLng())
        .arg(getAlt())
        .arg(getSeq());
}

QJsonObject QMissionItem::item2Json() {
    QJsonObject obj;
    obj["lat"] = simple.lat;
    obj["lng"] = simple.lng;
    obj["alt"] = simple.alt;
    obj["seq"] = seq;
    return obj;
}

void QMissionItem::json2Item(const QJsonObject &json) {

    if (!json["lng"].isUndefined()) {
        simple.lng = json["lng"].toInt();
    }

    if (!json["lat"].isUndefined()) {
        simple.lat = json["lat"].toInt();
    }

    if (!json["alt"].isUndefined()) {
        simple.alt = json["alt"].toInt();
    }

    if (!json["seq"].isUndefined()) {
        seq = json["seq"].toInt();
    }

    emit updatePosNotify();
    emit positionChanged();
}
