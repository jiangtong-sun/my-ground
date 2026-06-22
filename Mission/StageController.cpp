#include "StageController.h"
#include "GcsLineStage.h"
#include "MissionPath.h"
#include "TerrainManager.h"
#include "log_category.h"
#include "stringtranslate.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QTextCodec>

static QStringList stageColor = {"#f0f0f0", "#1080f0", "#1080f0", "#10f010",
                                 "#f08080", "#10f010", "#f08080", "#1080f0"};

StageController::StageController(int stage)
    : GcsTreeNode(nullptr), _stage(stage), _manual(false), _visible(false), _action(true) {
    _color = stageColor[stage];
    _canRMWaypoint = false;
    _distState = true;
}

void StageController::initWaypointType() {
    if (_stage == GcsLineStage::Curise) {
        _stagePointType.append(new TypeWaypoint(transToUnicode("应急返场点"), GcsWaypointMode::RtnHomePoint));
    }
}

QString StageController::stageName() const {
    if (_stage == GcsLineStage::Depart) {
        return QString("D%1").arg(_uuid);
    } else if (_stage == GcsLineStage::Landing) {
        return QString("L%1").arg(_uuid);
    } else if (_stage == GcsLineStage::Ergency) {
        return QString("E%1").arg(_uuid);
    } else if (_stage == GcsLineStage::Curise) {
        return QString("C%1").arg(_uuid);
    } else if (_stage == GcsLineStage::Templine) {
        return QString("P%1").arg(_uuid);
    } else {
        return QString("N%1").arg(_uuid);
    }
}

QString StageController::stageAbbr() const {
    if (_stage == GcsLineStage::Depart) {
        return QString("D");
    } else if (_stage == GcsLineStage::Landing) {
        return QString("L");
    } else if (_stage == GcsLineStage::Ergency) {
        return QString("E");
    } else if (_stage == GcsLineStage::Curise) {
        return QString("C");
    } else if (_stage == GcsLineStage::Templine) {
        return QString("P");
    } else if (_stage == GcsLineStage::GisFence) {
        return QString("G");
    } else {
        return QString("N");
    }
}

QVariant StageController::columnValue(int column) const {
    if (column == 0) {
        return _uuid;
    } else if (column == 1) {
        return _name;
    } else if (column == 2) {
        return QString("Action");
    }
    return QVariant();
}

void StageController::removeWaypointPath() {
    _dispayPoints.clear();
    emit dispayPointsChanged();
}

QGeoCoordinate StageController::loadStageLine(QString missionFile) {
    QString fileName = missionFile;
    if (!QFileInfo::exists(missionFile)) {
        QUrl url(missionFile);
        fileName = url.toLocalFile();
        if (!QFileInfo::exists(fileName)) {
            COMMON_WARN("check {0} is exist or not", missionFile.toStdString().c_str());
            return QGeoCoordinate(22.461, 113.8919, 213.2);
        }
    }

    QFile settings(fileName);
    QGeoCoordinate first = QGeoCoordinate(22.461, 113.8919, 213.2);
    static QRegularExpression mLineSplitExpress("[\\n ,;；]");
    if (settings.open(QIODevice::Text | QIODevice::ReadOnly)) {
        while (!settings.atEnd()) {
            QByteArray line = settings.readLine();
            QString lineData = QString::fromStdString(line.data());
            QStringList sets = lineData.split(mLineSplitExpress, Qt::SkipEmptyParts);
            if (sets.size() >= 4) {
                double lat = sets[2].toDouble();
                double lng = sets[1].toDouble();
                double alt = sets[3].toDouble();
                first = QGeoCoordinate(lat, lng, alt);
                insertItem(first);
            }
        }

        recalWaypointPath();
    }
    return first;
}

void StageController::recalWaypointPath() { recalAllWaypoint(); }

void StageController::recalAllWaypoint() {
    QString colr = color();
    _dispayPoints.clear();

    int sz = _currWaypoint.count();
    for (int i = 0; (stage() != GcsLineStage::Ergency) && (i < sz); i++) {
        QVisualItem *curr = nullptr;
        curr = static_cast<QVisualItem *>(_currWaypoint.get(i));
        _dispayPoints.append(QVariant::fromValue(curr->getPos()));
    }

    emit dispayPointsChanged();

    TerrainThread::instance().elevation(this);
}

void StageController::updateWaypointColor(QString color) {
    QMap<uint64_t, QObject *>::Iterator pitr;
    pitr = _path_map.begin();
    setColor(color);

    while (pitr != _path_map.end()) {
        QMissionPath *item = (QMissionPath *) (pitr.value());
        item->setLineColor(color);
        ++pitr;
    }
}

void StageController::releaseWaypointResource() {
    QVisualItem *a = nullptr;
    while (_currWaypoint.count() > 0) {
        a = (QVisualItem *) _currWaypoint.removeAt(0);
        delete a;
    }
}

void StageController::releasePathResource() {
    QMap<uint64_t, QObject *>::Iterator wayIter;
    wayIter = _path_map.begin();
    while (wayIter != _path_map.end()) {
        QMissionPath *data = (QMissionPath *) wayIter.value();
        wayIter = _path_map.erase(wayIter);
        delete data;
    }
}

void StageController::setStage(int code) {
    _color = stageColor[code];
    _stage = code;
    emit stageChanged();
}

int StageController::addRefer() {
    _manual.fetch_add(1);
    return _manual.load();
}

StageController::~StageController() {
    removeDisplayResource();
    releasePathResource();
    releaseWaypointResource();
}

// void StageController::takeoffConnect(QTakeoffItem *stage) {
//     connect(stage, &QTakeoffItem::updatePosNotify, this, &StageController::doUpdatePos);
//     connect(stage, &QTakeoffItem::positionChanged, this, &StageController::doUpdatePos);
//     connect(stage, &QMissionItem::positionChanged, this, &StageController::takeoffPosChanged);
//     connect(stage, &QTakeoffItem::yawAngleChanged, this, &StageController::takeoffPosChanged);
//     connect(stage, &QTakeoffItem::relativeHightChanged, this, &StageController::takeoffPosChanged);
//     connect(stage, &QTakeoffItem::runwayLenChanged, this, &StageController::takeoffPosChanged);
// }

void StageController::json2Waypoint(const QByteArray &datajson) {
    QJsonDocument doc = QJsonDocument::fromJson(datajson);
    QJsonArray arry = doc.array();
    bool firstNode = true;

    for (int xm = 0; xm < arry.count(); xm++) {
        QNormalSItem *stage = new QNormalSItem(_stage);
        stage->json2Item(arry.at(xm).toObject());
        connect(stage, &QNormalSItem::updatePosNotify, this, &StageController::doUpdatePos);
        _currWaypoint.append(stage);
    }
}

StageController *StageController::clone() {
    QJsonArray value;
    const QList<QObject *> &objectList = _currWaypoint.objectList();
    for (auto xm = objectList.begin(); xm != objectList.end(); xm++) {
        QVisualItem *item = (QVisualItem *) (*xm);
        value.push_back(item->item2Json());
    }

    QJsonObject stage;
    stage["uuid"] = _uuid;
    stage["stage"] = _stage;
    stage["name"] = _name;
    stage["code"] = ((int) _defNode << 8);
    stage["update"] = _udate;
    stage["data"] = value;

    StageController *newNode = new StageController();
    newNode->json2Stage(stage);
    newNode->addRefer();
    return newNode;
}

void StageController::json2Stage(const QJsonObject &json) {
    _uuid = json["uuid"].toInt();
    _stage = json["stage"].toInt();
    _name = json["name"].toString();
    _udate = json["update"].toString();
    int val = json["code"].toInt();
    _defNode = (val >> 8) & 0xff;
    QJsonArray arry = json["data"].toArray();
    for (int xm = 0; xm < arry.count(); xm++) {
        QNormalSItem *stage = new QNormalSItem(_stage);
        stage->json2Item(arry.at(xm).toObject());
        connect(stage, &QVisualItem::updatePosNotify, this, &StageController::doUpdatePos);
        _currWaypoint.append(stage);
    }
}

void StageController::stage2Json(QJsonArray &jsonArr) {
    QJsonArray value;
    const QList<QObject *> &objectList = _currWaypoint.objectList();
    for (auto xm = objectList.begin(); xm != objectList.end(); xm++) {
        QVisualItem *item = (QVisualItem *) (*xm);
        value.push_back(item->item2Json());
    }

    QJsonObject stage;
    stage["uuid"] = _uuid;
    stage["stage"] = _stage;
    stage["name"] = _name;
    stage["code"] = ((int) _defNode << 8);
    stage["update"] = _udate;
    stage["data"] = value;
    jsonArr.append(stage);
}

QByteArray StageController::waypoint2Json() const {
    QJsonArray value;
    for (int xm = 0; xm < _currWaypoint.count(); xm++) {
        QVisualItem *item = (QVisualItem *) _currWaypoint[xm];
        value.push_back(item->item2Json());
    }

    QJsonDocument document;
    document.setArray(value);
    return document.toJson(QJsonDocument::Compact);
}

void StageController::syncStateI(int type, int value) {
    const QList<QObject *> &objectList = _currWaypoint.objectList();
    for (auto xm = objectList.begin(); xm != objectList.end(); xm++) {
        QNormalSItem *item = (QNormalSItem *) (*xm);
        if (type == 1) {
            item->setHeightMode(value);
        }
    }
}

void StageController::syncStateF(int type, float value) {
    const QList<QObject *> &objectList = _currWaypoint.objectList();
    for (auto xm = objectList.begin(); xm != objectList.end(); xm++) {
        QNormalSItem *item = (QNormalSItem *) (*xm);
        if (type == 2) {
            item->setTurnRadius(value);
        } else if (type == 3) {
            item->setSpeed(value);
        } else if (type == 4) {
            item->setAlt(value);
        }
    }
}

void StageController::syncHeight(QString value) {
    const QList<QObject *> &objectList = _currWaypoint.objectList();
    for (auto xm = objectList.begin(); xm != objectList.end(); xm++) {
        QVisualItem *item = (QVisualItem *) (*xm);
        item->setAlt(value.toDouble());
    }
}

bool StageController::removeWaypoint(QObject *itm) {
    if (_currWaypoint.count() <= 0) {
        return false;
    }

    QVisualItem *aa = dynamic_cast<QVisualItem *>(itm);
    if (aa == nullptr) {
        COMMON_ERROR("unable trans object {}", (int) itm);
        return false;
    }

    if (stage() == GcsLineStage::Landing) {
        return false;
    } else if ((stage() == GcsLineStage::Curise)) {
        QObject *data1 = _currWaypoint.get(0);
        if (data1 == itm) {
            return false;
        }
    } else if (stage() == GcsLineStage::Depart) {
        QObject *data1 = _currWaypoint.get(0);
        QObject *data2 = _currWaypoint.get(1);
        QObject *data3 = _currWaypoint.get(_currWaypoint.count() - 1);
        if (itm == data1 || itm == data2 || itm == data3) {
            return false;
        }
    }

    int xm = _currWaypoint.indexOf(itm);
    if (xm >= 0 && xm < _currWaypoint.count()) {
        COMMON_INFO("try to remove waypoint {}, stage {}", aa->getSeq(), stage());
        _currWaypoint.removeAt(xm);
    }

    recalWaypointPath();

    emit visualPointsChanged();

    delete aa;

    return true;
}

void StageController::saveAs(int vehId) {
    QString tm = QDate::currentDate().toString("yyyy-MM-dd");
    setUpdate(tm); // 当前文件的更新时间

    QString taskData("TaskData");
    QDate cur = QDateTime::currentDateTime().date();
    QString datetm = QString("%1/%2-%3-%4").arg(taskData).arg(cur.year()).arg(cur.month()).arg(cur.day());
    QString fileName = QString("%1/%2_%3_%4.json").arg(datetm).arg(vehId).arg(stage()).arg(uuid());

    QDir filedir(taskData);
    if (!filedir.exists(taskData)) {
        _mkdir(taskData.toStdString().c_str());
    }

    QDir filedir2(datetm);
    if (!filedir2.exists(datetm)) {
        _mkdir(datetm.toStdString().c_str());
    }

    std::ofstream file(fileName.toStdString());
    if (file.is_open()) {
        QJsonArray jsonArray;
        stage2Json(jsonArray);
        QJsonDocument document;
        document.setArray(jsonArray);
        QByteArray byteArr = document.toJson(QJsonDocument::Compact);
        file.write(byteArr, byteArr.length());
        file.close();
    } else {
        COMMON_WARN("open file failed {}", fileName.toStdString().c_str());
    }
}

void StageController::removeDisplayResource() {
    releasePathResource();
    _dispayPoints.clear();
    if (_currWaypoint.count() > 0) {
        _currWaypoint.clearAll();
        emit visualPointsChanged();
    }
}

const QVisualItem *StageController::at(int i) {
    if (_currWaypoint.count() < i) {
        COMMON_WARN("invalid input parameter {}", i);
        return nullptr;
    }

    return (QVisualItem *) _currWaypoint.get(i);
}

void StageController::doUpdatePos() {
    unsaved(true);
    recalWaypointPath();
}

void StageController::takeoffPosChanged() {
    if (_currWaypoint.count() > 2) {
        QTakeoffItem *takeoff = (QTakeoffItem *) _currWaypoint.get(0);
        QTakeoffItem *offground = (QTakeoffItem *) _currWaypoint.get(1);
        QTakeoffItem *airItem = (QTakeoffItem *) _currWaypoint.get(2);
        QGeoCoordinate coordinate = takeoff->getPos();
        int height = takeoff->relativeHight();
        int distance = takeoff->runwayLen();
        int yaw = takeoff->yawAngle();
        QGeoCoordinate next = coordinate.atDistanceAndAzimuth(distance, yaw);
        QGeoCoordinate air = next.atDistanceAndAzimuth(height * 10, yaw, height);
        offground->setPos(next);
        airItem->setPos(air);
        recalWaypointPath();
    }
}

void StageController::addWaypoint(QGeoCoordinate &coordinate, int pointType) {
    QNormalSItem *item = nullptr;
    item = new QNormalSItem(stage());
    assert(item != nullptr);

    item->setSeq(getMaxId());
    item->setPos(coordinate);
    item->stageCode(uuid());
    item->setSpeed(0);
    item->setTurnMode(0);
    item->setNextMode(0);
    item->setHeightMode(0);
    item->setPointType(pointType);
    item->setClampSlide(0);

    if (pointType == TurningsPoint) {
        item->setTurnRadius(0);
    } else {
        item->setYawAngle(0);
    }

    unsaved(true);
    connect(item, &QVisualItem::updatePosNotify, this, &StageController::doUpdatePos);
    _currWaypoint.append(item);

    recalWaypointPath();
}

bool StageController::templineItem(QGeoCoordinate &coordinate) {
    const StageLimit &limit = FlightStageLimit::instance()->GcsStageConfig(stage());
    if (_currWaypoint.count() >= limit.counts()) {
        COMMON_INFO("templine stage more than 5 waypoint ");
        return false;
    }

    addWaypoint(coordinate, GcsWaypointMode::NormalPoint);
    return true;
}

bool StageController::ergencyItem(QGeoCoordinate &coordinate) {
    const StageLimit &limit = FlightStageLimit::instance()->GcsStageConfig(stage());
    if (_currWaypoint.count() >= limit.counts()) {
        COMMON_INFO("ergency stage more than 5 waypoint ");
        return false;
    }

    addWaypoint(coordinate, GcsWaypointMode::ErgencyPoint);
    return true;
}

// bool StageController::takeoffItem(QGeoCoordinate &coordinate) {
//     const StageLimit &limit = FlightStageLimit::instance()->GcsStageConfig(stage());
//     if (_currWaypoint.count() >= limit.counts()) {
//         COMMON_INFO("takeoff stage more than 1 waypoint ");
//         return false;
//     }
//     QTakeoffItem *takeoff = nullptr;
//     takeoff = new QTakeoffItem(stage());
//     takeoff->setSeq(getMaxId());
//     takeoff->setPos(coordinate);
//     takeoff->stageCode(0);
//     takeoffConnect(takeoff);
//     _currWaypoint.append(takeoff);
//     int height = takeoff->relativeHight();
//     int distance = takeoff->runwayLen();
//     int yaw = takeoff->yawAngle();
//     QGeoCoordinate next = coordinate.atDistanceAndAzimuth(distance, yaw);
//     QGeoCoordinate air = next.atDistanceAndAzimuth(height * 10, yaw, height);
//     QTakeoffItem *offground = nullptr;
//     offground = new QTakeoffItem(stage());
//     offground->setSeq(getMaxId());
//     offground->setPos(next);
//     offground->stageCode(0);
//     offground->canDrag(false);
//     _currWaypoint.append(offground);
//     QTakeoffItem *airItem = nullptr;
//     airItem = new QTakeoffItem(stage());
//     airItem->setSeq(getMaxId());
//     airItem->setPos(air);
//     airItem->stageCode(0);
//     airItem->canDrag(false);
//     _currWaypoint.append(airItem);
//     unsaved(true);
//     recalWaypointPath();
//     return true;
// }

void StageController::enterDepartBase(QGeoCoordinate &coordinate) {
    QNormalSItem *stop = nullptr;
    stop = new QNormalSItem(stage());
    stop->setSeq(getMaxId());
    stop->setPos(coordinate.atDistanceAndAzimuth(-500, 0));
    stop->stageCode(0);
    stop->setPointType(GcsWaypointMode::StopPoint);
    connect(stop, &QVisualItem::updatePosNotify, this, &StageController::doUpdatePos);
    _currWaypoint.append(stop);

    QNormalSItem *endpointA = nullptr;
    endpointA = new QNormalSItem(stage());
    endpointA->setSeq(0);
    endpointA->setPos(coordinate.atDistanceAndAzimuth(-500, 10));
    endpointA->stageCode(0);
    endpointA->setPointType(GcsWaypointMode::WayPointA);
    connect(endpointA, &QVisualItem::updatePosNotify, this, &StageController::doUpdatePos);

    _currWaypoint.append(endpointA);

    QNormalSItem *endpointB = nullptr;
    endpointB = new QNormalSItem(stage());
    endpointB->setSeq(1);
    endpointB->setPos(coordinate.atDistanceAndAzimuth(500, 10));
    endpointB->stageCode(0);
    endpointB->setPointType(GcsWaypointMode::WayPointB);
    connect(endpointB, &QVisualItem::updatePosNotify, this, &StageController::doUpdatePos);

    _currWaypoint.append(endpointB);
}

bool StageController::enterDepartItem(QGeoCoordinate &coordinate) {
    const StageLimit &limit = FlightStageLimit::instance()->GcsStageConfig(stage());
    if (_currWaypoint.count() >= limit.counts()) {
        COMMON_INFO("enter depart stage more than 20 waypoint");
        return false;
    }

    if (_currWaypoint.count() <= 0) {
        enterDepartBase(coordinate);
        recalWaypointPath();
        return false;
    }

    QNormalSItem *item = nullptr;
    item = new QNormalSItem(stage());
    item->setSeq(getMaxId());
    item->setPos(coordinate);
    item->stageCode(uuid());
    item->setPointType(GcsWaypointMode::NormalPoint);

    unsaved(true);

    connect(item, &QVisualItem::updatePosNotify, this, &StageController::doUpdatePos);

    _currWaypoint.insert(_currWaypoint.count() - 2, item);

    recalWaypointPath();
    return true;
}

bool StageController::landingItem(QGeoCoordinate &coordinate) {
    const StageLimit &limit = FlightStageLimit::instance()->GcsStageConfig(stage());
    if (_currWaypoint.count() >= limit.counts()) {
        COMMON_INFO("land stage more than 4 waypoint ");
        return false;
    }

    QNormalSItem *level = nullptr;
    level = new QNormalSItem(stage());
    level->setSeq(getMaxId());
    level->setPointType(GcsWaypointMode::NormalPoint);
    level->setPos(coordinate.atDistanceAndAzimuth(-3000, 0));
    level->stageCode(0);
    _currWaypoint.append(level);

    QNormalSItem *turn = nullptr;
    turn = new QNormalSItem(stage());
    turn->setSeq(getMaxId());
    turn->setPos(coordinate.atDistanceAndAzimuth(-2000, 0));
    turn->stageCode(0);
    turn->setPointType(GcsWaypointMode::TurnRoundPoint);
    _currWaypoint.append(turn);

    QNormalSItem *step = nullptr;
    step = new QNormalSItem(stage());
    step->setSeq(getMaxId());
    step->setPos(coordinate.atDistanceAndAzimuth(-1000, 0));
    step->stageCode(0);
    step->setPointType(GcsWaypointMode::SlidingPoint);
    _currWaypoint.append(step);

    QNormalSItem *landing = nullptr;
    landing = new QNormalSItem(stage());
    landing->setSeq(getMaxId());
    landing->setPos(coordinate);
    landing->stageCode(0);
    landing->setPointType(GcsWaypointMode::LandingPoint);
    _currWaypoint.append(landing);

    connect(landing, &QVisualItem::updatePosNotify, this, &StageController::doUpdatePos);
    connect(step, &QVisualItem::updatePosNotify, this, &StageController::doUpdatePos);
    connect(level, &QVisualItem::updatePosNotify, this, &StageController::doUpdatePos);
    connect(turn, &QVisualItem::updatePosNotify, this, &StageController::doUpdatePos);

    unsaved(true);

    recalWaypointPath();
    return true;
}

void StageController::removePoints(GcsListmodel *points) {
    if (points == nullptr) {
        return;
    }

    for (int ix = 0; ix < points->count(); ix++) {
        QNormalSItem *pitem = points->value<QNormalSItem *>(ix);
        for (int mx = 0; mx < _currWaypoint.count(); mx++) {
            QNormalSItem *citem = _currWaypoint.value<QNormalSItem *>(mx);
            if ((citem->stage() == pitem->stage()) && (citem->getSeq() == pitem->getSeq())) {
                _dispayPoints.removeAt(mx);
                _currWaypoint.removeAt(mx);
                break;
            }
        }
    }
}

void StageController::appendPoints(GcsListmodel *points) {
    if (points == nullptr) {
        return;
    }

    for (int ix = 0; ix < points->count(); ix++) {
        QNormalSItem *pitem = points->value<QNormalSItem *>(ix);
        QNormalSItem *item = new QNormalSItem(pitem->stage());
        assert(item != nullptr);
        if (item != nullptr) {
            memcpy(&item->refPos(), &pitem->refPos(), sizeof(pitem->refPos()));
            memcpy(&item->refNormal(), &pitem->refNormal(), sizeof(pitem->refNormal()));
            connect(item, &QVisualItem::updatePosNotify, this, &StageController::doUpdatePos);
            item->setSeq(getMaxId()); // point sequence
            _dispayPoints.append(QVariant::fromValue(item->getPos()));
            _currWaypoint.append(item);
        } else {
            COMMON_INFO("allocate memory failed");
        }
    }
}

void StageController::addOrUpdate(ProtoNormWaypoint &point, int idx) {
    if (_currWaypoint.count() > idx) {
        QNormalSItem *item = _currWaypoint.value<QNormalSItem *>(idx);
        assert(item != nullptr);
        if (item != nullptr) {
            item->setSeq(idx); //
            memcpy(&item->refPos(), &point.base, sizeof(point.base));
            memcpy(&item->refNormal(), &point.attr, sizeof(point.attr));
            item->notifyChanged();
        } else {
            COMMON_INFO("allocate memory failed {}", idx);
        }

    } else {
        QNormalSItem *item = new QNormalSItem(stage());
        assert(item != nullptr);
        if (item != nullptr) {
            item->setSeq(idx); //
            memcpy(&item->refPos(), &point.base, sizeof(point.base));
            memcpy(&item->refNormal(), &point.attr, sizeof(point.attr));
            connect(item, &QVisualItem::updatePosNotify, this, &StageController::doUpdatePos);
            _currWaypoint.append(item);
        } else {
            COMMON_INFO("allocate memory failed {}", idx);
        }
    }
}

bool StageController::insertItem(QGeoCoordinate coordinate) {
    unsaved(true);
    if (stage() == GcsLineStage::Landing) {
        return landingItem(coordinate);
    } else if (stage() == GcsLineStage::Depart) {
        return enterDepartItem(coordinate);
    } else if (stage() == GcsLineStage::Ergency) {
        return ergencyItem(coordinate);
    } else if (stage() == GcsLineStage::Templine) {
        return templineItem(coordinate);
    } else if (stage() == GcsLineStage::Curise || stage() == GcsLineStage::GisFence) {
        const StageLimit &limit = FlightStageLimit::instance()->GcsStageConfig(stage());
        if (stage() == GcsLineStage::Curise && _currWaypoint.count() >= limit.counts()) {
            COMMON_INFO("line stage more than 250 waypoint ");
            return false;
        }

        int pointType = GcsWaypointMode::NormalPoint;
        if ((stage() == GcsLineStage::Curise) && (_currWaypoint.count() <= 0)) {
            COMMON_INFO("add takeoff point for curise");
            pointType = GcsWaypointMode::TakeoffPoint;
        }

        addWaypoint(coordinate, pointType);

        return true;
    }

    return false;
}

int StageController::getMaxId() {
    const QVisualItem *ft = last();
    return (ft) ? ft->getSeq() + 1 : 1;
}

const QVisualItem *StageController::first() {
    if (_currWaypoint.count() > 0) {
        return (QVisualItem *) _currWaypoint.get(0);
    }
    return nullptr;
}

const QVisualItem *StageController::last() {
    if (_currWaypoint.count() > 0) {
        return (QVisualItem *) _currWaypoint.get(_currWaypoint.count() - 1);
    }

    return nullptr;
}

void StageController::terrain(QJsonArray &elemArr, QMap<int, QVisualItem *> &tempMap) {
    const QList<QObject *> &objectList = _currWaypoint.objectList();
    for (auto i = objectList.begin(); (i != objectList.end()); i++) {
        QVisualItem *item = (QVisualItem *) (*i);
        if (item->terrain() != 0) {
            QJsonObject elemObj;
            elemObj["seq"] = item->getSeq();
            elemObj["lat"] = item->getLat();
            elemObj["lng"] = item->getLng();
            elemArr.append(elemObj);
            tempMap.insert(item->getSeq(), item);
        }
    }
}
