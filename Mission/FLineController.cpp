#include "FLineController.h"
#include "GcsLineStage.h"
#include "StageController.h"
#include "log_category.h"
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVector2D>

int64_t FlineStageDetail::currentTimeMs() {
    typedef std::chrono::milliseconds ms_precision;
    typedef std::chrono::time_point<std::chrono::system_clock, ms_precision> time_precision;
    time_precision tp = std::chrono::time_point_cast<ms_precision>(std::chrono::system_clock::now());
    return tp.time_since_epoch().count();
}

void FlineStageDetail::showNeareastStage(QGeoCoordinate coordinate, int distance) {
    int64_t cur = currentTimeMs();
    if (cur < uptNeareast + 200) {
        return;
    }

    uptNeareast = currentTimeMs();
    for (auto mx = cache_stages.begin(); mx != cache_stages.end(); mx++) {

        bool isnear = false;
        GcsListmodel *points = (*mx)->visualPoints();

        /// 判断这个航线段的航点与飞机当前位置的距离
        for (int ix = 0; ix < points->count(); ix++) {
            QVisualItem *next = points->value<QVisualItem *>(ix);
            if (next != nullptr) {
                double dis = coordinate.distanceTo(next->getPos());
                if ((dis < distance) && (dis >= 0)) {
                    isnear = true;
                    next->neareast(true);
                } else {
                    next->neareast(false);
                }
            }
        }

        (*mx)->distState(isnear);
    }
}

FlineStageDetail::FlineStageDetail(FLineController *fline, int code) : StageCodeName(code), flineCtrl(fline) {

    name(FTaskLineName(code));

    connect(this, &FlineStageDetail::removeWaypointSignal, this, &FlineStageDetail::doRemoveWaypoint);
}

int FlineStageDetail::totalPoint() {
    int counts = 0;
    auto stagesIter = cache_stages.begin();
    while (stagesIter != cache_stages.end()) {
        counts += (*stagesIter)->counts();
        ++stagesIter;
    }
    return counts;
}

void FlineStageDetail::assignWaypoint(ProtoNormWaypoint &waypoint, const QVisualItem *item, int idex) {
    const QNormalSItem *normal = dynamic_cast<const QNormalSItem *>(item);
    if (normal != nullptr) {
        memcpy(&waypoint.base, normal->pos(), sizeof(waypoint.base));
        memcpy(&waypoint.attr, &normal->normalInfo(), sizeof(waypoint.attr));
        waypoint.base.absNo = idex;
        waypoint.attr.crc8 = 0;
        return;
    }

    COMMON_WARN("assign waypoint error {} ", idex);
    // const QTakeoffItem *takeoff = dynamic_cast<const QTakeoffItem *>(item);
    // if (takeoff != nullptr) {
    //     memcpy(&waypoint.base, takeoff->pos(), sizeof(waypoint.base));
    //     waypoint.base.absNo = idex;
    //     waypoint.attr.att_a_0 = TakeoffPoint;
    //     waypoint.attr.att_b_6 = FlightHeightType::Relative;
    //     waypoint.attr.yaw_radius = takeoff->yawAngle();
    // }
}

bool FlineStageDetail::fillUpDataProto(QVector<MissionLineDataProto *> &protos, int index) {

    uint8_t stageSeq = 0;
    auto stagesIter = cache_stages.begin();
    while (stagesIter != cache_stages.end()) {

        ++stageSeq; // an index of array
        if ((*stagesIter)->uuid() == index) {
            MissionLineDataProto *pproto = new MissionLineDataProto();
            memset(pproto, 0, sizeof(MissionLineDataProto));

            int idxwaypoint = 0;
            int groupindex = 1;
            int pointindex = 0;

            int counts = (*stagesIter)->counts();
            pproto->stage = TaskLineCode(code());
            pproto->totalPoint = counts;
            pproto->groupNo = groupindex;
            pproto->number = stageSeq;

            while (pointindex < counts) {
                const QVisualItem *item = (*stagesIter)->at(pointindex);
                assignWaypoint(pproto->point[idxwaypoint], item, pointindex + 1);
                ProtoCRC8((uint8_t *) &pproto->point[idxwaypoint], sizeof(ProtoNormWaypoint), true);
                pproto->pointCount = ++idxwaypoint;
                if (idxwaypoint % 4 == 0) {
                    protos.append(pproto);
                    pproto = new MissionLineDataProto();
                    memset(pproto, 0, sizeof(MissionLineDataProto));
                    groupindex += 1;
                    idxwaypoint = 0;
                    pproto->stage = TaskLineCode(code());
                    pproto->totalPoint = counts;
                    pproto->groupNo = groupindex;
                    pproto->number = stageSeq; //
                }

                ++pointindex;
            }

            if (idxwaypoint > 0) {
                protos.append(pproto);
            } else {
                delete pproto;
            }

            return true;
        }
    }

    return false;
}

bool FlineStageDetail::fillUpDataProtoExt(QVector<MissionLineDataProto *> &protos) {
    auto stagesIter = cache_stages.begin();
    if (stagesIter == cache_stages.end()) {
        return false;
    }

    uint8_t stageSeq = 0;
    while (stagesIter != cache_stages.end()) {
        MissionLineDataProto *pproto = new MissionLineDataProto();
        memset(pproto, 0, sizeof(MissionLineDataProto));
        assert(pproto != nullptr);

        int counts = (*stagesIter)->counts();
        int idxwaypoint = 0;
        int groupindex = 1;

        pproto->stage = TaskLineCode(code());
        pproto->totalPoint = counts;
        pproto->number = ++stageSeq;
        pproto->groupNo = groupindex;
        int currindex = 0;

        while (currindex < counts) {

            const QVisualItem *item = (*stagesIter)->at(currindex);

            assignWaypoint(pproto->point[idxwaypoint], item, currindex + 1);
            ProtoCRC8((uint8_t *) &pproto->point[idxwaypoint], sizeof(ProtoNormWaypoint), true);
            pproto->pointCount = ++idxwaypoint;

            if (idxwaypoint % 4 == 0) {
                protos.append(pproto);
                pproto = new MissionLineDataProto();
                memset(pproto, 0, sizeof(MissionLineDataProto));
                groupindex += 1;
                idxwaypoint = 0;
                pproto->stage = TaskLineCode(code());
                pproto->totalPoint = counts;
                pproto->number = stageSeq;
                pproto->groupNo = groupindex;
            }

            ++currindex;
        }

        if (idxwaypoint > 0) {
            protos.append(pproto);
        } else {
            delete pproto;
        }

        stagesIter += 1;
    }

    return true;
}

bool FlineStageDetail::fillUpDataProto(QVector<MissionLineDataProto *> &protos) {
    auto stagesIter = cache_stages.begin();
    if (stagesIter == cache_stages.end()) {
        return false;
    }

    MissionLineDataProto *pproto = new MissionLineDataProto();
    assert(pproto != nullptr);

    memset(pproto, 0, sizeof(MissionLineDataProto));

    int idxwaypoint = 0;
    int groupindex = 1;
    int number = (code() >> 4) & 0xf;

    pproto->stage = TaskLineCode(code());
    pproto->totalPoint = totalPoint();
    pproto->groupNo = groupindex;
    pproto->number = number; //
    int totalindex = 1;

    while (stagesIter != cache_stages.end()) {

        int counts = (*stagesIter)->counts();
        int currindex = 0;

        while (currindex < counts) {
            const QVisualItem *item = (*stagesIter)->at(currindex);
            assignWaypoint(pproto->point[idxwaypoint], item, totalindex++);
            ProtoCRC8((uint8_t *) &pproto->point[idxwaypoint], sizeof(ProtoNormWaypoint), true);
            pproto->pointCount = ++idxwaypoint;

            if (idxwaypoint % 4 == 0) {
                protos.append(pproto);
                pproto = new MissionLineDataProto();
                memset(pproto, 0, sizeof(MissionLineDataProto));
                groupindex += 1;
                idxwaypoint = 0;
                pproto->stage = TaskLineCode(code());
                pproto->totalPoint = totalPoint();
                pproto->groupNo = groupindex;
                pproto->number = number; //
            }

            currindex += 1;
        }

        stagesIter += 1;
    }

    if (idxwaypoint > 0) {
        protos.append(pproto);
    } else {
        delete pproto;
    }

    return true;
}

void FlineStageDetail::connectStageLine(StageController *head, StageController *tail) {
    if (head == nullptr || tail == nullptr) {
        return;
    }
    const QVisualItem *first = head->last();
    const QVisualItem *last = tail->first();
    uint64_t path = 0;
    path += (first->stageCode() << 16);
    path += first->getSeq();
    path <<= 32;
    path += (last->stageCode() << 16);
    path += last->getSeq();
    QMissionPath *item = new QMissionPath(first, last, tail->color());
    // QMissionPath *item = new QMissionPath(first, last, "#f08010");
    waypath_map.insert(path, item);
    _waypointPath.append(item);
}

void FlineStageDetail::releseWaypointPath() {

    QMap<uint64_t, QMissionPath *>::Iterator wayIter;
    wayIter = waypath_map.begin();

    while (wayIter != waypath_map.end()) {
        QMissionPath *data = wayIter.value();
        wayIter = waypath_map.erase(wayIter);
        delete data;
    }

    _waypointPath.clear();
}

void FlineStageDetail::connStageWaypoint() {
    releseWaypointPath();
    auto stagesIter = cache_stages.begin();
    while (stagesIter != cache_stages.end()) {
        auto nextIter = ++stagesIter;
        if (nextIter != cache_stages.end()) {
            connectStageLine(*stagesIter, *nextIter);
        }
        stagesIter = nextIter;
    }
    emit connPathChanged();
}

int FlineStageDetail::loadStageFromFile(QString missionFile) {
    if (missionFile.isEmpty()) {
        return GcsLineOperate::InvalidParameter;
    }

    QString fileName = missionFile;
    if (!QFileInfo::exists(missionFile)) {
        QUrl url(missionFile);
        fileName = url.toLocalFile();
        if (!QFileInfo::exists(fileName)) {
            COMMON_WARN("check {0} is exist or not", missionFile.toStdString().c_str());
            return GcsLineOperate::InvalidParameter;
        }
    }

    QFile settings(fileName);
    StageController *stageCtrl = nullptr;
    if ((stageCtrl = newStage(GcsLineStage::Templine)) == nullptr) {
        return GcsLineOperate::CreateMissionFail;
    }

    static QRegularExpression mLineSplitExpress("[\\n ,;；]");
    if (settings.open(QIODevice::Text | QIODevice::ReadOnly)) {
        int itemIdx = 0;
        while (!settings.atEnd()) {
            QByteArray line = settings.readLine();
            QString lineData = QString::fromStdString(line.data());
            QStringList sets = lineData.split(mLineSplitExpress, Qt::SkipEmptyParts);
            if (sets.size() >= 4) {
                QGeoCoordinate waypoint;
                waypoint.setLatitude(sets[2].toDouble());
                waypoint.setLongitude(sets[1].toDouble());
                waypoint.setAltitude(sets[3].toDouble());
                stageCtrl->insertItem(waypoint);
            }
        }
        stageCtrl->recalAllWaypoint();
    }

    stageCtrl->canRMWaypoint(true); // 可移动点
    return GcsLineOperate::TaskOperationSuccess;
}

void FlineStageDetail::deftStage(StageController *stage) {
    // int sizeDefault = limit.defCount() - 1;
    // for (auto xm = stages.begin(); xm != stages.end(); xm++) {
    //     if ((*xm) == stage) {
    //         (*xm)->defNode(true);
    //     } else if ((*xm)->defNode()) {
    //         if (sizeDefault > 0) {
    //             sizeDefault -= 1;
    //         } else {
    //             (*xm)->defNode(false);
    //             sizeDefault -= 1;
    //         }
    //     }
    // }
}

void FlineStageDetail::doRemoveWaypoint(QObject *data) {
    assert(data != nullptr);

    QMap<QObject *, bool>::Iterator at = waypoints.find(data);
    QVisualItem *item = (QVisualItem *) (data);

    COMMON_INFO("try remove node {}, {}", item->stage(), item->getSeq());

    if (at != waypoints.end()) {
        for (auto mx = cache_stages.begin(); mx != cache_stages.end(); mx++) {
            (*mx)->removeWaypoint(item);
        }
    }

    if (flineCtrl != nullptr) {
        flineCtrl->doRemoveWaypoint(item);
    }
}

void FlineStageDetail::recalWaypointPath() {
    for (auto xm = cache_stages.begin(); xm != cache_stages.end(); xm++) {
        (*xm)->recalWaypointPath();
    }
}

void FlineStageDetail::mergeStage(QJsonObject &stage) {
    if ((code() == GcsTaskLine::FTaskLineA) || (code() == GcsTaskLine::FTaskLineB)) {
        StageController *stageAB = nullptr;
        QJsonArray jsonArray = stage["data"].toArray();
        for (int i = 0; i < jsonArray.count(); i++) {
            int stagecode = 0;
            QJsonObject mobj = jsonArray[i].toObject();
            if (mobj.contains("stage")) {
                stagecode = mobj["stage"].toInt();
            }
            if ((stages_mode.count() <= 0) || (stageAB == nullptr)) {
                stageAB = new StageController(stagecode);
                assert(stageAB != nullptr);
                if (stageAB == nullptr) {
                    COMMON_WARN("allocate memory failed, for stage {}", stagecode);
                    continue;
                }
                stageAB->json2Stage(mobj);
                stageAB->addRefer();
                stageAB->recalWaypointPath();

                cache_stages.append(stageAB);
                stages_mode.append(stageAB);

            } else {
                StageController ot(stagecode);
                ot.json2Stage(mobj); // temp data
                stageAB->appendPoints(ot.visualPoints());
                stageAB->recalWaypointPath();
            }
        }

        emit countStagesChanged();

    } else {

        json2Stage(stage);
    }
}

void FlineStageDetail::json2Stage(QJsonObject &stage) {
    QJsonArray jsonArray = stage["data"].toArray();
    for (int i = 0; i < jsonArray.count(); i++) {
        int stagecode = 0;
        QJsonObject mobj = jsonArray[i].toObject();
        if (mobj.contains("stage")) {
            stagecode = mobj["stage"].toInt();
        }

        StageController *stage = new StageController(stagecode);
        assert(stage != nullptr);
        if (stage == nullptr) {
            COMMON_WARN("allocate memory failed, for stage {}", stagecode);
            continue;
        }

        stage->json2Stage(mobj);
        stage->addRefer();
        stage->recalWaypointPath();

        if (stagecode == GcsLineStage::Templine || stagecode == GcsLineStage::GisFence) {
            stage->canRMWaypoint(true);
        }

        cache_stages.append(stage);
        stages_mode.append(stage);
        emit countStagesChanged();
    }
}

void FlineStageDetail::stage2Json(QJsonObject &stage) {
    QJsonArray jsonArray;
    for (auto xm = cache_stages.begin(); xm != cache_stages.end(); xm++) {
        (*xm)->stage2Json(jsonArray);
    }
    stage["code"] = _code;
    stage["name"] = _name;
    stage["data"] = jsonArray;
}

void FlineStageDetail::removeAllStages() {
    while (!cache_stages.empty()) {
        auto xelem = cache_stages.begin();
        if (xelem != cache_stages.end()) {
            if ((*xelem)->canRemove()) {
                delete (*xelem);
            }

            cache_stages.erase(xelem);
        }
    }

    cache_stages.clear();
    stages_mode.clearAll();
    emit countStagesChanged();
}

int FlineStageDetail::appendRemoveStage(StageController *stage) {
    int type = 0;
    if (stage == nullptr) {
        COMMON_INFO("invalid param");
        return GcsLineOperate::InvalidParameter;
    }

    for (auto mx = cache_stages.begin(); mx != cache_stages.end(); mx++) {
        if ((*mx)->stage() == stage->stage() && ((*mx)->uuid() == stage->uuid())) {
            return removeStage(stage);
        }
    }

    return appendStage(stage);
}

int FlineStageDetail::removeStage(StageController *stage) {
    if (stage == nullptr) {
        COMMON_INFO("invalid param");
        return GcsLineOperate::InvalidParameter;
    }

    bool rtnget = false;
    for (auto xm = cache_stages.begin(); xm != cache_stages.end(); xm++) {
        if ((*xm) == stage) {
            stages_mode.removeOne(stage);
            cache_stages.erase(xm);
            rtnget = true;
            break;
        }
    }

    stage->checked(false);

    if (!rtnget) {
        COMMON_INFO("unknow stage");
        return GcsLineOperate::InvalidParameter;
    }

    bool rtn = stage->canRemove();

    COMMON_INFO("remove {} stage {}, uuid {}, count {}", (int) (stage), stage->stage(), stage->uuid(),
                stage->referCount());

    if ((stage != nullptr) && rtn) {
        delete stage;
    }

    emit countStagesChanged();
    return GcsLineOperate::TaskOperationSuccess;
}

int FlineStageDetail::appendStage(StageController *stage) {
    int type = 0;
    if (stage == nullptr) {
        COMMON_INFO("invalid param");
        return GcsLineOperate::InvalidParameter;
    }

    for (auto mx = cache_stages.begin(); mx != cache_stages.end(); mx++) {
        if ((*mx)->stage() == stage->stage()) {
            type += 1;
        }
    }

    int rnt = FlightStageLimit::instance()->sameStageLimit(code(), stage->stage(), type);
    if (rnt != 0) {
        return rnt;
    }

    stage->checked(true);

    stage->addRefer();
    stage->recalWaypointPath();

    stages_mode.append(stage);
    cache_stages.append(stage);

    emit countStagesChanged();

    return GcsLineOperate::TaskOperationSuccess;
}

int FlineStageDetail::stageNumber() {
    int64_t rtn = 0;
    auto xelem = cache_stages.begin();
    while (xelem != cache_stages.end()) {
        rtn |= (1 << (*xelem)->uuid());
        xelem += 1;
    }
    for (int i = 1; i < 64; i++) {
        if (((rtn >> i) & 0x1) == 0) {
            return i;
        }
    }
    return 1;
}

StageController *FlineStageDetail::isExist(int uuid, int stage) {
    auto xelem = cache_stages.begin();
    while (xelem != cache_stages.end()) {
        if (uuid == (*xelem)->uuid()) {
            return *xelem;
        }
        ++xelem;
    }

    if (newFlineStage() == GcsLineOperate::TaskOperationSuccess) {
        select_stage->uuid(uuid); // update uuid
        return select_stage;
    }

    return nullptr;
}

StageController *FlineStageDetail::newStage(GcsLineStage::LineStage stage) {
    StageController *cstage = new StageController(stage);
    assert(cstage != nullptr);
    cstage->canRMWaypoint(true);
    cstage->uuid(stageNumber());
    cstage->addRefer();
    cstage->visible(true);
    cstage->newElem(true);
    select_stage = cstage;
    cache_stages.append(cstage);
    stages_mode.append(select_stage);
    COMMON_INFO("new stage {}, uuid {}", (int) stage, cstage->uuid());
    emit countStagesChanged();
    emit selectStageChanged();
    return cstage;
}

QPoint FlineStageDetail::getLandingSetpoints(QVariantList &maps) {
    if (cache_stages.size() <= 0) {
        return QPoint(0, 0);
    }

    QPoint rtn(0, 0);
    StageController *first = nullptr;
    if ((first = cache_stages.at(0)) != nullptr) {
        GcsListmodel *points = first->visualPoints();
        if (points != nullptr) {
            int startPos = -1;
            int endPos = -1;

            for (int ix = 0; (endPos <= 0) && (ix < points->count()); ix++) {
                QNormalSItem *current = nullptr;
                current = points->value<QNormalSItem *>(ix);
                if ((startPos < 0) && current->pointType() == GcsWaypointMode::TurnRoundPoint) {
                    startPos = ix - 1;
                } else if ((endPos < 0) && current->pointType() == GcsWaypointMode::LandingPoint) {
                    endPos = ix;
                }
            }

            int firstX = 1;
            QNormalSItem *before = nullptr;
            maps.append(QPoint(1, 0));
            if (endPos > 0 && endPos < points->count()) {
                before = points->value<QNormalSItem *>(endPos);
            }

            for (int dx = endPos - 1; before && (dx >= startPos); dx--) {
                QNormalSItem *current = nullptr;
                current = points->value<QNormalSItem *>(dx);
                if (current != nullptr) {
                    double distance = before->getPos().distanceTo(current->getPos());
                    maps.append(QPoint((int) distance + firstX, current->getAlt()));
                    if (current->pointType() == GcsWaypointMode::SlidingPoint) {
                        rtn = QPoint((int) distance + firstX, current->getAlt());
                    }
                    COMMON_INFO("landing pos {} {}", (int) ((int) distance + firstX), current->getAlt());
                    firstX = (int) distance + firstX;
                    before = current;
                }
            }
        }
    }

    return rtn;
}

int FlineStageDetail::newFlineStage() {
    if (code() == GcsTaskLine::FTaskTempline) {
        int tmpLimit = FlightStageLimit::instance()->ftaskTempline().counts();
        if (cache_stages.count() >= tmpLimit) {
            return GcsLineOperate::LineStageIsFull;
        }
        if (newStage(GcsLineStage::Templine)) {
            return GcsLineOperate::TaskOperationSuccess;
        }
    } else if (code() == GcsTaskLine::FTaskFence) {
        int tmpLimit = FlightStageLimit::instance()->ftaskFence().counts();
        if (cache_stages.count() >= tmpLimit) {
            return GcsLineOperate::LineStageIsFull;
        }
        if (newStage(GcsLineStage::GisFence)) {
            return GcsLineOperate::TaskOperationSuccess;
        }
    } else if (code() == GcsTaskLine::FTaskLineA) {
        int tmpLimit = FlightStageLimit::instance()->ftaskNormalA().counts();
        if (cache_stages.count() >= tmpLimit) {
            return GcsLineOperate::LineStageIsFull;
        }
        if (newStage(GcsLineStage::Curise)) {
            return GcsLineOperate::TaskOperationSuccess;
        }
    } else if (code() == GcsTaskLine::FTaskLineB) {
        int tmpLimit = FlightStageLimit::instance()->ftaskNormalB().counts();
        if (cache_stages.count() >= tmpLimit) {
            return GcsLineOperate::LineStageIsFull;
        }
        if (newStage(GcsLineStage::Curise)) {
            return GcsLineOperate::TaskOperationSuccess;
        }
    } else if (code() == GcsTaskLine::FTaskErgency) {
        int tmpLimit = FlightStageLimit::instance()->ftaskErgency().counts();
        if (cache_stages.count() >= tmpLimit) {
            return GcsLineOperate::LineStageIsFull;
        }
        if (newStage(GcsLineStage::Ergency)) {
            return GcsLineOperate::TaskOperationSuccess;
        }
    }

    return GcsLineOperate::UnableUpdateStage;
}

bool FlineStageDetail::isfull() {
    if (code() == GcsTaskLine::FTaskLineA) {
        int tmpLimit = FlightStageLimit::instance()->ftaskNormalA().counts();
        if (cache_stages.count() > tmpLimit) {
            return true;
        }
    } else if (code() == GcsTaskLine::FTaskLineB) {
        int tmpLimit = FlightStageLimit::instance()->ftaskNormalB().counts();
        if (cache_stages.count() > tmpLimit) {
            return true;
        }
    } else if (code() == GcsTaskLine::FTaskDepart) {
        int tmpLimit = FlightStageLimit::instance()->ftaskDepart().counts();
        if (cache_stages.count() > tmpLimit) {
            return true;
        }
    } else if (code() == GcsTaskLine::FTaskErgency) {
        int tmpLimit = FlightStageLimit::instance()->ftaskErgency().counts();
        if (cache_stages.count() > tmpLimit) {
            return true;
        }
    }

    return false;
}

bool FlineStageDetail::isExist(StageController *const stage) const {
    for (auto xm = cache_stages.begin(); xm != cache_stages.end(); xm++) {
        if ((*xm) == stage) {
            return true;
        }
    }

    return false;
}

bool FlineStageDetail::removeWaypoint(QVisualItem *item) const {
    for (auto xm = cache_stages.begin(); xm != cache_stages.end(); xm++) {
        if ((*xm)->removeWaypoint(item)) {
            return true;
        }
    }

    return false;
}

void FlineStageDetail::tryRemoveWaypoint(QObject *data) {
    if (data == nullptr) {
        return;
    }

    emit removeWaypointSignal(data);
}

FLineController::FLineController() : _name("") {
    std::shared_ptr<FlineStageDetail> line1(new FlineStageDetail(this, GcsTaskLine::FTaskLineA));
    std::shared_ptr<FlineStageDetail> line2(new FlineStageDetail(this, GcsTaskLine::FTaskLineB));
    std::shared_ptr<FlineStageDetail> depart(new FlineStageDetail(this, GcsTaskLine::FTaskDepart));
    std::shared_ptr<FlineStageDetail> tmp(new FlineStageDetail(this, GcsTaskLine::FTaskTempline));
    std::shared_ptr<FlineStageDetail> fence(new FlineStageDetail(this, GcsTaskLine::FTaskFence));
    std::shared_ptr<FlineStageDetail> ergcy(new FlineStageDetail(this, GcsTaskLine::FTaskErgency));

    stage_lines_map.insert(GcsTaskLine::FTaskLineA, line1);
    stage_lines_map.insert(GcsTaskLine::FTaskLineB, line2);
    stage_lines_map.insert(GcsTaskLine::FTaskErgency, ergcy);
    stage_lines_map.insert(GcsTaskLine::FTaskDepart, depart);
    stage_lines_map.insert(GcsTaskLine::FTaskTempline, tmp);
    stage_lines_map.insert(GcsTaskLine::FTaskFence, fence);

    _taskStages.append(line1.get());
    _taskStages.append(line2.get());
    _taskStages.append(ergcy.get());
    _taskStages.append(depart.get());
    _taskStages.append(fence.get());

    _subNodes.append(line1.get());
    _subNodes.append(line2.get());
    _subNodes.append(ergcy.get());
    _subNodes.append(depart.get());
    _subNodes.append(tmp.get());
    _subNodes.append(fence.get());

    _gmapStages.append(line1.get());
    _gmapStages.append(line2.get());
    _gmapStages.append(ergcy.get());
    _gmapStages.append(depart.get());
    _gmapStages.append(tmp.get());
    _gmapStages.append(fence.get());
}

bool FLineController::fillUpDataProto(QVector<MissionLineDataProto *> &proto, int type, int index) {
    auto stagesIter = stage_lines_map.find(type);
    if (stagesIter != stage_lines_map.end()) {
        return stagesIter.value()->fillUpDataProto(proto, index);
    }
    return false;
}

bool FLineController::fillUpDataProtoExt(QVector<MissionLineDataProto *> &proto, int type) {
    auto stagesIter = stage_lines_map.find(type);
    if (stagesIter != stage_lines_map.end()) {
        return stagesIter.value()->fillUpDataProtoExt(proto);
    }

    return false;
}

bool FLineController::fillUpDataProto(QVector<MissionLineDataProto *> &proto, int type) {
    auto stagesIter = stage_lines_map.find(type);
    if (stagesIter != stage_lines_map.end()) {
        return stagesIter.value()->fillUpDataProto(proto);
    }

    return false;
}

void FLineController::fillUpReqProto(MissionUploadReqProto *proto) {
    assert(proto != nullptr);
    // int val1 = FlightStageLimit::instance()->ftaskNormalA().counts();
    // int val2 = FlightStageLimit::instance()->ftaskNormalB().counts();
    auto stagesIter = stage_lines_map.find(GcsTaskLine::FTaskLineA);
    if (stagesIter != stage_lines_map.end()) {
        proto->normal += stagesIter.value()->countStage() >= 1 ? 1 : 0;
    }
    stagesIter = stage_lines_map.find(GcsTaskLine::FTaskLineB);
    if (stagesIter != stage_lines_map.end()) {
        proto->normal += stagesIter.value()->countStage() >= 1 ? 1 : 0;
    }
    stagesIter = stage_lines_map.find(GcsTaskLine::FTaskDepart);
    if (stagesIter != stage_lines_map.end()) {
        proto->departarrive += stagesIter.value()->countStage();
    }
    stagesIter = stage_lines_map.find(GcsTaskLine::FTaskErgency);
    if (stagesIter != stage_lines_map.end()) {
        proto->ergency += stagesIter.value()->countStage();
    }
}

QVariant FLineController::columnValue(int column) const {
    if (column == 0) {
        return _uuid;
    } else if (column == 1) {
        return _name;
    } else if (column == 2) {
        return QString("Action");
    }
    return QVariant();
}

FLineController::~FLineController() { clearStageResource(); }

void FLineController::removeStageResource() {
    auto stagesIter = stage_lines_map.begin();
    for (; stagesIter != stage_lines_map.end(); stagesIter++) {
        if (stagesIter.value()->code() != GcsTaskLine::FTaskTempline) {
            stagesIter.value()->removeAllStages();
        }
    }
}

void FLineController::clearStageResource() {
    auto stagesIter = stage_lines_map.begin();
    for (; stagesIter != stage_lines_map.end(); stagesIter++) {
        stagesIter.value()->removeAllStages();
    }
}

std::shared_ptr<FlineStageDetail> FLineController::getStageDetial(int type) {
    auto stagesIter = stage_lines_map.find(type);
    if (stagesIter != stage_lines_map.end()) {
        return stagesIter.value();
    }

    return nullptr;
}

QVector<StageController *> &FLineController::getTaskLines(int stage) const {
    static QVector<StageController *> nullStage;
    auto stagesIter = stage_lines_map.find(stage);
    if (stagesIter != stage_lines_map.end()) {
        return stagesIter.value()->allStages();
    }

    return nullStage;
}

int FLineController::counts(int stage) const {
    auto stagesIter = stage_lines_map.find(stage);
    if (stagesIter != stage_lines_map.end()) {
        return stagesIter.value()->countStage();
    }
    return 0;
}

int FLineController::getTaskLineCode(StageController *const stage) {
    assert(stage != nullptr);
    if (stage == nullptr) {
        return GcsTaskLine::FTaskInvalid;
    }

    auto stagesIter = stage_lines_map.begin();
    for (; (stagesIter != stage_lines_map.end()); stagesIter++) {
        if (stagesIter.value()->isExist(stage)) {
            return stagesIter.key();
        }
    }

    return GcsTaskLine::FTaskInvalid;
}

bool FLineController::isExist(StageController *const stage) const {
    assert(stage != nullptr);
    if (stage == nullptr) {
        return false;
    }
    bool state = false;
    for (auto stagesIter = stage_lines_map.begin(); !state && (stagesIter != stage_lines_map.end()); stagesIter++) {
        state = stagesIter.value()->isExist(stage);
    }
    return state;
}

void FLineController::doRemoveWaypoint(QVisualItem *item) {
    //从中waypath_map删除与Item关联的信息
}

void FLineController::getLandingSetpoints(int typeLine) {
    _landMapNode.clear();
    std::shared_ptr<FlineStageDetail> stageDetail = nullptr;
    stageDetail = getStageDetial(typeLine);
    if (stageDetail != nullptr) {
        QPoint point = stageDetail->getLandingSetpoints(_landMapNode);
        emit flineLandChanged(point);
    }
}

int FLineController::loadFlightLine(QString filename, bool resave) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return GcsLineOperate::MissionLoadFailed;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    QString str = in.readAll();
    file.close();

    COMMON_INFO("missiondata load \n{}", str.toStdString());
    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject curobj = doc.object();

    if (curobj.contains("data")) {
        clearStageResource(); //
        if (!resave) {        // manual save flag
            if (curobj.contains("uuid")) {
                uuid(curobj["uuid"].toInt());
            }
            if (curobj.contains("name")) {
                name(curobj["name"].toString());
            }
            if (curobj.contains("updat")) {
                update(curobj["updat"].toString());
            }
        }

        QJsonObject arry = curobj["data"].toObject();
        for (auto mx = stage_lines_map.begin(); mx != stage_lines_map.end(); mx++) {
            QJsonObject elem = arry[mx.value()->name()].toObject();
            mx.value()->json2Stage(elem);
        }

        return GcsLineOperate::MissionLoadSuccess;
    }

    return GcsLineOperate::CreateMissionFail;
}

int FLineController::newTemplineStage() {
    auto xm = stage_lines_map.find(GcsTaskLine::FTaskTempline);
    if (xm == stage_lines_map.end()) {
        return GcsLineOperate::InvalidParameter;
    }

    int count = xm.value()->countStage();
    StageController *templine_stage = new StageController(GcsLineStage::Templine);
    assert(templine_stage != nullptr);
    if (templine_stage != nullptr) {
        templine_stage->setName(FlightStageLimit::stageTypeName(GcsLineStage::Templine));
        templine_stage->uuid(count + 1);
        templine_stage->action(true);

        xm.value()->appendStage(templine_stage);
        return GcsLineOperate::TaskOperationSuccess;
    }

    return GcsLineOperate::CreateMissionFail;
}

void FLineController::showErgencyLine(QGeoCoordinate coordinate, int distance) {
    for (auto xm = stage_lines_map.begin(); xm != stage_lines_map.end(); xm++) {
        if (xm.value()->code() == GcsTaskLine::FTaskErgency) {
            xm.value()->showNeareastStage(coordinate, distance);
        }
    }
}

int FLineController::exportFlightLine(QString filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return GcsLineOperate::MissionExportFailed;
    }

    QJsonObject exportObject;
    exportObject["uuid"] = uuid();
    exportObject["name"] = name();
    exportObject["updat"] = update();
    exportObject["data"] = item2Json();

    QJsonDocument document;
    document.setObject(exportObject);

    QByteArray text = document.toJson(QJsonDocument::Compact);
    COMMON_INFO("missiondata export \n{}", text.toStdString());

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << text;
    file.close();

    return GcsLineOperate::MissionExportSuccess;
}

void FLineController::calcWaypointPath() {
    for (auto ix = stage_lines_map.begin(); ix != stage_lines_map.end(); ++ix) {
        (*ix)->recalWaypointPath();
    }
}

QJsonObject FLineController::item2Json() const {
    QJsonObject flineObjStr;
    for (auto iter = stage_lines_map.begin(); iter != stage_lines_map.end(); ++iter) {
        QJsonObject jsonArray;
        (*iter)->stage2Json(jsonArray);
        flineObjStr[iter.value()->name()] = jsonArray;
    }
    return flineObjStr;
}

QByteArray FLineController::fline2Json() const {
    QJsonDocument document;
    document.setObject(item2Json());
    return document.toJson(QJsonDocument::Compact);
}

void FLineController::mergeItem(const QByteArray &datajson) {
    QJsonDocument doc = QJsonDocument::fromJson(datajson);
    QJsonObject arry = doc.object();

    for (auto mx = stage_lines_map.begin(); mx != stage_lines_map.end(); mx++) {
        QJsonObject elem = arry[mx.value()->name()].toObject();
        mx.value()->mergeStage(elem);
    }
}

void FLineController::json2Item(const QByteArray &datajson) {
    QJsonDocument doc = QJsonDocument::fromJson(datajson);
    QJsonObject arry = doc.object();

    for (auto mx = stage_lines_map.begin(); mx != stage_lines_map.end(); mx++) {
        QJsonObject elem = arry[mx.value()->name()].toObject();
        mx.value()->json2Stage(elem);
    }
}
