#pragma once

#include "ConfigControllerExt.h"
#include "GcsDataManager.h"
#include "GcsDataTransfer.h"
#include "MissionManager.h"
#include "VehicleSession.h"

class GcsMFmcManager : public VehicleSession {
    Q_OBJECT

  public:
    Q_PROPERTY(FactTableModel *frameSet READ frameSet NOTIFY frameSetChanged)

    GcsMFmcManager(QObject *parent = nullptr);
    ~GcsMFmcManager();

    Q_INVOKABLE void setDataFrame(QString frame);
    Q_INVOKABLE void notifyFrame();
    FactTableModel *frameSet();

    virtual void saveService(int chlType, const void *adr, const uint8_t *buf, int len) override;
    virtual void dataAfterProcess(Channel *mainNet, Channel *slaveNet, int chl) override;
    virtual void extentNetService(LoopHandle *) override;
    virtual void stopExtentNetService() override;

    void channelSend(Channel *mainNet, ControlAdjustProto *proto);

    void reloadTransConfig(const QString &file);
    void reloadDisplayConfig(const QString &file);
    void reloadNetComConfig(const QString &file);

    void initlialize(const QStringList &file);
    void release();

    MissionManager *missionMgr() { return &_missionMgr; }
    GcsDataManager *dataManager() { return &_dataMgr; }
    ConfigController *configCtrl() { return &configControlExt; }

  signals:
    void frameSetChanged();

  public:
    static GcsMFmcManager *instance();
    static GcsMFmcManager *create(QQmlEngine *, QJSEngine *) { return instance(); }

  private:
    ConfigControllerExt configControlExt;
    GcsDataTransfer data_transfer;
    RTKDataTranfer rtk_tranfer;
    MissionManager _missionMgr;
    GcsDataManager _dataMgr;
    QString lastFrame = "";
};
