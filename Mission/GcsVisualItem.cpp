#include "GcsVisualItem.h"
#include "GcsLineStage.h"
#include "log_category.h"

QVisualItem::QVisualItem(int stage)
    : QMissionItem(), _stage(stage), _stageCode(0), _canDrag(true), _terrainState(false), _neareast(false) {}

QString QNormalSItem::typeName() {
    int cod = normal.att_a_0;
    if (cod == GcsWaypointMode::TakeoffPoint) {
        return QString("T");
    } else if (cod == GcsWaypointMode::LandingPoint || cod == GcsWaypointMode::SlidingPoint ||
               cod == GcsWaypointMode::LevelPoint || cod == GcsWaypointMode::TurnRoundPoint) {
        return QString("L");
    } else if (cod == GcsWaypointMode::RtnHomePoint) {
        return QString("R");
    } else if (cod == GcsWaypointMode::ErgencyPoint) {
        return QString("E");
    } else if (cod == GcsWaypointMode::StopPoint) {
        return QString("S");
    } else if (cod == GcsWaypointMode::WayPointA || cod == GcsWaypointMode::WayPointB) {
        return QString("W");
    } else {
        if (_stage == GcsLineStage::Depart) {
            return QString("D");
        } else if (_stage == GcsLineStage::Landing) {
            return QString("L");
        } else if (_stage == GcsLineStage::Ergency) {
            return QString("E");
        } else if (_stage == GcsLineStage::GisFence) {
            return QString("G");
        } else if (_stage == GcsLineStage::Curise) {
            return QString("C");
        } else if (_stage == GcsLineStage::Templine) {
            return QString("P");
        } else {
            COMMON_WARN("unknow type {}", cod);
            return QString("N");
        }
    }
}

QNormalSItem::QNormalSItem(int stage) : QVisualItem(stage) {
    _runwayLen = 0;
    _relativeHight = 0;
    memset(&normal, 0, sizeof(normal));
    connect(this, &QNormalSItem::yawAngleChanged, this, &QNormalSItem::takeoffChanged);
    connect(this, &QNormalSItem::runwayLenChanged, this, &QNormalSItem::takeoffChanged);
}

void QNormalSItem::takeoffChanged() {
    float yaw = yawAngle();
    int distance = runwayLen();
    QGeoCoordinate start = getPos();
    distance = (distance < 800) ? 800 : distance;
    _nextPos = start.atDistanceAndAzimuth(distance, yaw);
    emit nextPosChanged(_nextPos);
}

QJsonObject QNormalSItem::item2Json() {
    QJsonObject obj;
    obj["lat"] = simple.lat;
    obj["lng"] = simple.lng;
    obj["alt"] = simple.alt;
    obj["len"] = _runwayLen;
    obj["hgt"] = _relativeHight;
    obj["seq"] = seq;
    obj["rtl"] = normal.rth_height;
    obj["rad"] = normal.yaw_radius;
    obj["spd"] = normal.speed;
    obj["TM"] = normal.att_b_2;
    obj["NM"] = normal.att_b_3;
    obj["CS"] = normal.att_b_4;
    obj["WT"] = normal.att_a_0;
    obj["HT"] = normal.att_b_6;
    return obj;
}

void QNormalSItem::json2Item(const QJsonObject &json) {
    if (!json["len"].isUndefined()) {
        _runwayLen = json["len"].toInt();
    }

    if (!json["hgt"].isUndefined()) {
        _relativeHight = json["hgt"].toInt();
    }

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

    if (!json["spd"].isUndefined()) {
        normal.speed = json["spd"].toInt();
    }

    if (!json["TM"].isUndefined()) {
        normal.att_b_2 = json["TM"].toInt();
    }

    if (!json["NM"].isUndefined()) {
        normal.att_b_3 = json["NM"].toInt();
    }

    if (!json["rad"].isUndefined()) {
        normal.yaw_radius = json["rad"].toInt();
    }

    if (!json["CS"].isUndefined()) {
        normal.att_b_4 = json["CS"].toInt();
    }

    if (!json["HT"].isUndefined()) {
        normal.att_b_6 = json["HT"].toInt();
    }

    if (!json["WT"].isUndefined()) {
        normal.att_a_0 = json["WT"].toInt();
    }

    if (!json["rtl"].isUndefined()) {
        normal.rth_height = json["rtl"].toInt();
    }

    if ((int) normal.att_a_0 == (int) TakeoffPoint) {
        emit yawAngleChanged();
    }

    emit updatePosNotify();
    emit positionChanged();
}

QJsonObject QTakeoffItem::item2Json() {
    QJsonObject obj;
    obj["lat"] = simple.lat;
    obj["lng"] = simple.lng;
    obj["alt"] = simple.alt;
    obj["seq"] = seq;
    obj["yaw"] = yawAngle();
    obj["height"] = _relativeHight;
    obj["lenSpd"] = _runwayLen;
    return obj;
}

void QTakeoffItem::json2Item(const QJsonObject &json) {

    if (!json["lng"].isUndefined()) {
        simple.lng = json["lng"].toDouble();
    }

    if (!json["lat"].isUndefined()) {
        simple.lat = json["lat"].toDouble();
    }

    if (!json["alt"].isUndefined()) {
        simple.alt = json["alt"].toDouble();
    }

    if (!json["seq"].isUndefined()) {
        seq = json["seq"].toInt();
    }

    if (!json["yaw"].isUndefined()) {
        setYawAngle(json["yaw"].toDouble());
    }

    if (!json["height"].isUndefined()) {
        _relativeHight = json["height"].toDouble();
    }

    if (!json["lenSpd"].isUndefined()) {
        _runwayLen = json["lenSpd"].toInt();
    }

    emit updatePosNotify();
    emit positionChanged();
}
