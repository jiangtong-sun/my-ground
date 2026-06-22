#pragma once

#include "FLineDBController.h"
#include "SqliteDatabase.h"
#include "StageDBController.h"
#include "TerrainDrawSignal.h"
#include "VehicleMission.h"
#include "VehicleSession.h"

#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>

class MissionManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList stageName READ stageName CONSTANT)
    Q_PROPERTY(FactListModel *vehMissions READ vehMissions NOTIFY vehMissionsChanged)
    Q_PROPERTY(GcsListmodel *stageModels READ stageModels NOTIFY stageModelsChanged)
    Q_PROPERTY(GcsTreeModel *stageTree READ stageTree NOTIFY stageTreeChanged)
    Q_PROPERTY(GcsListmodel *stageCodeName READ stageCodeName CONSTANT)
    Q_PROPERTY(FLineDBController *flineModel READ flineModel CONSTANT)
    Q_PROPERTY(TerrainDrawSignal *terrainDraw READ terrainDraw CONSTANT)
    Q_PROPERTY(FlightStageLimit *stageLimit READ stageLimit CONSTANT)

  public:
    MissionManager();
    ~MissionManager();

    static void registerType();

    QStringList stageName();
    GcsTreeModel *stageTree();
    FlightStageLimit *stageLimit() { return &_stageLimit; }
    TerrainDrawSignal *terrainDraw() { return &_terrainDraw; }
    GcsListmodel *stageCodeName() { return &_stageCodeName; }
    FLineDBController *flineModel() { return &_flineModel; }
    GcsListmodel *stageModels() { return &_stageListmode; }

    ///当Socket收到数据后，Session调用回调函数处理任务消息数据
    ///如果数据是任务消息，则返回true，表示该消息已经被删除，后续逻辑不处理该消息
    ///
    bool processMissionMsg(PtrMessage msg);

    ///数据处理线程通过SOCKET句柄来写数据
    ///
    void missionProcess(Channel *read_write, MissionMessageQueue &);

    ///处理消息响应值
    bool feedbackVerify(int ctl, int adj);

    void calElevatorFinish();

    FactListModel *vehMissions() { return &all_veh_missions; }

    Q_INVOKABLE void downloadFlightLine(int vehId, int linecode);
    Q_INVOKABLE void uploadFlightLine(FlineStageDetail *fline, int destVehicle);
    Q_INVOKABLE void selectTreeIndex(int);
    Q_INVOKABLE void resetCheck();
    Q_INVOKABLE void loadAll();

    void initializeMission(VehicleSession *session);
    bool initDatabaseEnv();
    void doRelease();

  public:
    void dbOperateTest();

  private slots:
    void currentVehicleUpdated();

  signals:
    void terrainPlotChanged();
    void stageModelsChanged();
    void notifyTaskChanged();
    void vehMissionsChanged();
    void stageTreeChanged();

  private:
    std::mutex veh_mutex;

    FlightStageLimit _stageLimit;
    GcsListmodel _stageCodeName;
    VehicleSession *_veh_session = nullptr;

    QMap<uint32_t, std::shared_ptr<VehicleMission>> map_veh_fline;

    FactListModel all_veh_missions;

    GcsDatabase _missionDB;

    int _selectTreeIndex;

    TerrainDrawSignal _terrainDraw;

    GcsTreeModel _treeModel;

    FLineDBController _flineModel;

    GcsListmodel _stageListmode;

    StageDBController _curiseModel;
    StageDBController _landingModel;
    StageDBController _ergencyModel;
    StageDBController _enterModel;
    StageDBController _gisFenceModel;
    StageDBController _templateModel;
};
