#include "MissionManager.h"
#include "GcsLineStage.h"
#include "MissionLine.h"
#include "SimpleTerrainPlot.h"
#include "TerrainManager.h"
#include "customplot/grid.h"
#include "customplot/simpleplot.h"
#include "customplot/ticker.h"
#include "log_category.h"
#include "stringtranslate.h"
#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMetaMethod>
#include <QStandardPaths>

QStringList MissionManager::stageName() { return GcsStageNames(); }

MissionManager::MissionManager()
    : QObject(), _landingModel(&_missionDB, GcsLineStage::Landing), _curiseModel(&_missionDB, GcsLineStage::Curise),
      _ergencyModel(&_missionDB, GcsLineStage::Ergency), _gisFenceModel(&_missionDB, GcsLineStage::GisFence),
      _enterModel(&_missionDB, GcsLineStage::Depart), _templateModel(&_missionDB, GcsLineStage::Templine),
      _flineModel(&_missionDB) {

    QStringList column;
    column.append(transToUnicode("uuid"));
    column.append(transToUnicode("name"));
    column.append(transToUnicode("action"));

    _treeModel.columnSource(column);

    _stageListmode.append(&_enterModel);
    _stageListmode.append(&_landingModel);
    _stageListmode.append(&_curiseModel);
    _stageListmode.append(&_ergencyModel);
    _stageListmode.append(&_gisFenceModel);

    _stageCodeName.append(_enterModel.codeName());
    _stageCodeName.append(_landingModel.codeName());
    _stageCodeName.append(_curiseModel.codeName());
    _stageCodeName.append(_ergencyModel.codeName());
    _stageCodeName.append(_templateModel.codeName());
    _stageCodeName.append(_gisFenceModel.codeName());
}

void MissionManager::initializeMission(VehicleSession *session) {
    assert(session != nullptr);
    _veh_session = session;

    FactListModel *aircrafts = session->aircrafts();
    if (aircrafts != nullptr) {
        const QList<QObject *> &objects = aircrafts->objectList();
        for (auto x = objects.begin(); x != objects.end(); x++) {
            FlightVechicle *veh = dynamic_cast<FlightVechicle *>(*x);
            std::shared_ptr<VehicleMission> mission(new VehicleMission());
            assert(veh != nullptr);
            mission->setSessionVehicle(session, veh);
            map_veh_fline.insert(veh->vehicleIdx(), mission);
            all_veh_missions.append(mission.get());
        }
    }

    connect(session, &VehicleSession::currVehicleChanged, this, &MissionManager::currentVehicleUpdated);

    _curiseModel.stageInitialize();
    _landingModel.stageInitialize();
    _ergencyModel.stageInitialize();
    _enterModel.stageInitialize();
    _templateModel.stageInitialize();
    _gisFenceModel.stageInitialize();

    _treeModel.append(&_curiseModel);
    _treeModel.append(&_landingModel);
    _treeModel.append(&_enterModel);
    _treeModel.append(&_ergencyModel);
    _treeModel.append(&_templateModel);
    _treeModel.append(&_gisFenceModel);

    _flineModel.stageResetCallback([=]() { resetCheck(); });
}

void MissionManager::resetCheck() {
    _curiseModel.resetCheck();
    _landingModel.resetCheck();
    _ergencyModel.resetCheck();
    _enterModel.resetCheck();
    _gisFenceModel.resetCheck();
    _templateModel.resetCheck();
}

GcsTreeModel *MissionManager::stageTree() { return &_treeModel; }

void MissionManager::selectTreeIndex(int type) {
    _selectTreeIndex = type;
    _treeModel.resetChecked();
    emit stageTreeChanged();
}

void MissionManager::registerType() {

    qmlRegisterUncreatableType<Ticker>("QmlQCustomPlot", 1, 0, "Ticker", "");
    qmlRegisterUncreatableType<Axis>("QmlQCustomPlot", 1, 0, "Axis", "");
    qmlRegisterUncreatableType<Grid>("QmlQCustomPlot", 1, 0, "Grid", "");
    qmlRegisterType<SimplePlot>("QmlQCustomPlot", 1, 0, "SimplePlot");
    qmlRegisterType<BasePlot>("QmlQCustomPlot", 1, 0, "BasePlot");

    qmlRegisterType<GcsLineOperate>("GcsLineOperate", 1, 0, "GcsLineOperate");
    qmlRegisterType<CustomPlotItem>("CustomPlotItem", 1, 0, "CustomPlotItem");
    qmlRegisterType<StageCodeName>("StageCodeName", 1, 0, "StageCodeName");
    qmlRegisterType<GcsLineStage>("GcsLineStage", 1, 0, "GcsLineStage");
    qmlRegisterType<GcsTaskLine>("GcsTaskLine", 1, 0, "GcsTaskLine");
    qmlRegisterType<TypeWaypoint>("TypeWaypoint", 1, 0, "TypeWaypoint");
    qmlRegisterType<StageLimit>("StageLimit", 1, 0, "StageLimit");
    qmlRegisterType<FlightStageLimit>("FlightStageLimit", 1, 0, "FlightStageLimit");
    qmlRegisterType<GcsWaypointMode>("GcsWaypointMode", 1, 0, "GcsWaypointMode");
    qmlRegisterUncreatableType<QMissionItem>("QMissionItem", 1, 0, "QMissionItem", "Reference only");
    qmlRegisterUncreatableType<QMissionLine>("QMissionLine", 1, 0, "QMissionLine", "Reference only");
    qmlRegisterUncreatableType<QMissionPath>("QMissionPath", 1, 0, "QMissionPath", "Reference only");
    qmlRegisterUncreatableType<FlineStageDetail>("FlineStageDetail", 1, 0, "FlineStageDetail", "Reference");
    qmlRegisterUncreatableType<GcsTreeModel>("GcsTreeModel", 1, 0, "GcsTreeModel", "Reference");
    qmlRegisterUncreatableType<GcsTreeNode>("GcsTreeNode", 1, 0, "GcsTreeNode", "Reference");
    qmlRegisterUncreatableType<QVisualItem>("QVisualItem", 1, 0, "QVisualItem", "Reference");
    qmlRegisterUncreatableType<GcsListmodel>("GcsListmodel", 1, 0, "GcsListmodel", "Reference");
    qmlRegisterUncreatableType<MissionManager>("MissionManager", 1, 0, "MissionManager", "Reference");
    qmlRegisterUncreatableType<StageController>("StageController", 1, 0, "StageController", "Reference");
    qmlRegisterUncreatableType<StageDBController>("StageDBController", 1, 0, "StageDBController", "Reference");
    qmlRegisterUncreatableType<FLineController>("FLineController", 1, 0, "FLineController", "Reference");
    qmlRegisterUncreatableType<FLineDBController>("FLineDBController", 1, 0, "FLineDBController", "Reference");
}

MissionManager::~MissionManager() { doRelease(); }

void MissionManager::calElevatorFinish() {
    // emit signalManager.redrawCurve();
}

bool MissionManager::processMissionMsg(PtrMessage protomsg) {

    assert(protomsg != nullptr);
    VehicleMissionProto *msg = nullptr;
    msg = (VehicleMissionProto *) protomsg->buffer;
    ProtoDecoderInfo &decodeInfo = GetProtoDecoderInfo();
    if (msg->cmd_type != decodeInfo.MissionOptFrame) {
        return false;
    }

    // 从飞控传输的任务消息，消息源地址只当前的飞机地址
    auto first = map_veh_fline.begin();
    while (msg != nullptr && first != map_veh_fline.end()) {
        if (msg->src_addr == first.key()) {
            first.value()->processMissionMsg(msg);
            break;
        }
        ++first;
    }

    deleteProtoMessage(protomsg);
    return true;
}

void MissionManager::downloadFlightLine(int selectVeh, int linecode) {
    auto mx = map_veh_fline.find(selectVeh);
    if (mx != map_veh_fline.end() && mx.value() != nullptr) {
        mx.value()->downloadFlightLine(linecode);
    }
}

void MissionManager::uploadFlightLine(FlineStageDetail *fline, int selectVeh) {
    assert(fline != nullptr);
    if (fline == nullptr) {
        COMMON_WARN("invalid session");
        return;
    }

    auto mx = map_veh_fline.find(selectVeh);
    if (mx != map_veh_fline.end()) {
        mx.value()->releaseResource();
        mx.value()->uploadFline(fline);
    }
}

bool MissionManager::feedbackVerify(int ctlfeed, int adj) {
    auto first = map_veh_fline.begin();
    while (first != map_veh_fline.end()) {
        first.value()->missionFeedbackVerify(ctlfeed);
        first++;
    }
    return false;
}

void MissionManager::currentVehicleUpdated() { COMMON_WARN("IcurrentVehicleUpdated"); }

void MissionManager::missionProcess(Channel *read_write, MissionMessageQueue &taskqueue) {
    std::unique_lock<std::mutex> lck(veh_mutex);
    auto first = map_veh_fline.begin();
    while (first != map_veh_fline.end()) {
        first.value()->missionProcess(read_write, taskqueue);
        first++;
    }
}

void MissionManager::doRelease() {
    TerrainThread::instance().close();
    _missionDB.disconnectDB();
}

void MissionManager::loadAll() {
    TerrainThread::instance().setTerrainDraw(&_terrainDraw);
    _curiseModel.loadStageLine();
    _landingModel.loadStageLine();
    _ergencyModel.loadStageLine();
    _templateModel.loadStageLine();
    _gisFenceModel.loadStageLine();
    _enterModel.loadStageLine();
    _flineModel.loadAllFline();
}

bool MissionManager::initDatabaseEnv() {

    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    cacheDir.append(QStringLiteral("/GCSCacheDB"));

    if (!QDir::root().mkpath(cacheDir)) {
        COMMON_WARN("Could not to create the cache directory {0}", cacheDir.toStdString());
        cacheDir = QDir::currentPath() + QStringLiteral("/GCSCacheDB");

        if (!QDir::root().mkpath(cacheDir)) {
            COMMON_WARN("Could not to create the cache directory {0}", cacheDir.toStdString());
            cacheDir.clear();
        }
    }

    if (!cacheDir.isEmpty()) {
        _missionDB.setDatabasePath(cacheDir + "/mission.db");
    }

    if (_missionDB.connectDB()) {
        return _missionDB.createDB();
    }

    return false;
}

void MissionManager::dbOperateTest() {}
