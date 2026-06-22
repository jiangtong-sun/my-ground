#include "GcsMFmcManager.h"
#include "log_category.h"

Q_GLOBAL_STATIC(GcsMFmcManager, gCommandMgr)

GcsMFmcManager *GcsMFmcManager::instance() { return gCommandMgr(); }

GcsMFmcManager::GcsMFmcManager(QObject *parent) : VehicleSession(&configControlExt), _missionMgr(), lastFrame(-1) {}

GcsMFmcManager::~GcsMFmcManager() {}

void GcsMFmcManager::initlialize(const QStringList &file) {
    addSendingCallback([=](Channel *readwrite, MissionMessageQueue &queue) {
        if (readwrite != nullptr) {
            _missionMgr.missionProcess(readwrite, queue);
        }
    });
    addProcessCallback([=](PtrMessage code) { return _missionMgr.processMissionMsg(code); });
    addFeedbackCallback([=](int ctl, int adj) { return _missionMgr.feedbackVerify(ctl, adj); });

    _missionMgr.initializeMission(this);
    _missionMgr.initDatabaseEnv();
    _missionMgr.loadAll();

    _dataMgr.loadDataFile();
    _dataMgr.startRecord();
}

void GcsMFmcManager::release() {
    _missionMgr.doRelease();
    _dataMgr.stopRecord();
}

void GcsMFmcManager::reloadTransConfig(const QString &file) { data_transfer.parseTransFile(file); }

void GcsMFmcManager::reloadDisplayConfig(const QString &file) { configControlExt.reloadDisplayConfig(file); }

void GcsMFmcManager::reloadNetComConfig(const QString &file) {
    COMMON_INFO("reload config file");
    configControlExt.reloadNetComConfig(file);
    configControlExt.loadLocalIP();
    configControlExt.checkingLocalIP();
}

void GcsMFmcManager::extentNetService(LoopHandle *loop) {
    assert(loop != nullptr);
    rtk_tranfer.enableNetworkService([=]() { return _station_role == GcsStationRole::ControlRole ? true : false; },
                                     [=]() { return vehAdjustControl.cmdNumber(); }, loop,
                                     configControlExt.dataTransfer(), configControlExt.mainNetNode(),
                                     configControlExt.slaveNetNode());
    rtk_tranfer.enableDataTranfer(true);
}

void GcsMFmcManager::stopExtentNetService() {
    rtk_tranfer.stopNetworkService();
}

///
/// \brief GcsMFmcManager::saveService
/// \param chlType
/// \param databuf
/// \param datalen
///
void GcsMFmcManager::saveService(int chlType, const void *sockaddr, const uint8_t *databuf, int datalen) {
    _dataMgr.writeData(&configControlExt, chlType, sockaddr, databuf, datalen);
}

void GcsMFmcManager::channelSend(Channel *mainNet, ControlAdjustProto *proto){
    if (proto == nullptr) {
        return;
    }

    ControlAdjustProto * newproto = new ControlAdjustProto();
    assert(newproto != nullptr);
    if (newproto == nullptr) {
        COMMON_INFO("memory allocate failed {}", proto->length);
        return;
    }

    memcpy(newproto, proto, sizeof(ControlAdjustProto));
    mainNet->Send((char *) newproto, sizeof(ControlAdjustProto), [=](int status, char *buf, int len) {
        ControlAdjustProto *frame = (ControlAdjustProto *) buf;
        QString hex = ToHexStr((uint8_t *) frame, sizeof(ControlAdjustProto));
        COMMON_INFO("device command: {} ", hex.toStdString().c_str());
        if (frame != nullptr) {
            delete frame;
        }
    });
}

///
/// \brief GcsMFmcManager::dataAfterProcess
/// \param mainNet
///
void GcsMFmcManager::dataAfterProcess(Channel *mainNet, Channel *slaveNet, int chl) {

    rtk_tranfer.tranferProto(mainNet, slaveNet, serial_rtk_cache, configControlExt.dataTransfer());
}

void GcsMFmcManager::setDataFrame(QString frame) {
    if (_currentVehicle == nullptr) {
        return;
    }

    if (!lastFrame.isEmpty() && protoDataSet != nullptr) {
        FactTableModel *data = protoDataSet->protoframe(lastFrame);
        if (data != nullptr) {
            data->setChecked(false);
        }
    }

    lastFrame = frame;
    emit frameSetChanged();
    notifyFrame(); ///
}

void GcsMFmcManager::notifyFrame() {
    FactTableModel *data = nullptr;
    data = (FactTableModel *) protoDataSet->protoframe(lastFrame);
    if (data != nullptr) {
        COMMON_INFO("reload data {}", lastFrame.toLocal8Bit().toStdString());
        data->signalModel(_net_selected.load());
    }
}

FactTableModel *GcsMFmcManager::frameSet() {
    if (_currentVehicle == nullptr) {
        return nullptr;
    }

    FactTableModel *data = nullptr;
    if (lastFrame >= 0 && protoDataSet != nullptr) {
        data = (FactTableModel *) protoDataSet->protoframe(lastFrame);
        if (data != nullptr) {
            data->setChecked(true);
        }
    }

    return data;
}
