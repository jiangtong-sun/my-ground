#pragma once

#include "ConfigController.h"
#include "DataLinkerSummary.h"
#include "FlightAircraft.h"
#include "SimpleSerialListener.h"
#include "UpdateDrawSignal.h"
#include "VehicleCommand.h"
#include "VehicleWarnExport.h"
#include "WindowShortCut.h"
#include "factdecodermgr.h"
#include "protochannel.h"
#include "prototelemetry.h"
#include "uvloop.h"
#include <Mmsystem.h >
#include <QtQml/qqml.h>
#include <QtTextToSpeech/QTextToSpeech>
#include <mutex>
#include <thread>
#include <windows.h>
#pragma comment(lib, "Winmm.lib")

using MsgSendingCallback = std::function<void(Channel *rw, MissionMessageQueue &)>;
using MsgProcessCallback = std::function<bool(PtrMessage msg)>;
using FeedbackCallback = std::function<bool(int control, int adjust)>;

// 定义一个名为 VehicleSession 的类，公有继承自 Qt 的核心基类 QObject
class VehicleSession : public QObject {
    // 注意：继承 QObject 的类必须添加 Q_OBJECT 宏（关键！）
    Q_OBJECT
    Q_DISABLE_COPY(VehicleSession)

  public:
    Q_PROPERTY(QTextToSpeech *textSpeech READ textSpeech CONSTANT)
    Q_PROPERTY(VehicleCommand *vehicleRC READ vehicleRC CONSTANT)
    Q_PROPERTY(DataLinkerSummary *mainLinker READ mainLinker CONSTANT)
    Q_PROPERTY(DataLinkerSummary *slaveLinker READ slaveLinker CONSTANT)
    Q_PROPERTY(DataLinkerSummary *serialLinker READ serialLinker CONSTANT)
    Q_PROPERTY(RedrawSignal *redraw READ redraw NOTIFY signalDrawing)
    Q_PROPERTY(FactListModel *aircrafts READ aircrafts NOTIFY aircraftsChanged)
    Q_PROPERTY(FlightVechicle *currentVehicle READ currVehicle NOTIFY currVehicleChanged)
    Q_PROPERTY(int vehicleId READ vehicleId WRITE setVehicleId NOTIFY vehicleIdChanged)
    Q_PROPERTY(int netChannel READ netChannel WRITE setNetChannel NOTIFY netChannelChanged)
    Q_PROPERTY(int stationRole READ stationRole NOTIFY stationRoleChanged)
    Q_PROPERTY(RCCommandGroup *taskInitItem READ taskInitItem CONSTANT)

    VehicleSession(ConfigController *config = nullptr);
    ~VehicleSession();

    DataLinkerSummary *mainLinker() { return &main_linker_summary; }
    DataLinkerSummary *slaveLinker() { return &slave_linker_summary; }
    DataLinkerSummary *serialLinker() { return &serial_linker_summary; }
    RCCommandGroup *taskInitItem() { return vehAdjustControl.taskInitialize(); }

    void calculateLinkQuality(int timeVal);
    int autoSelectLinker();

    int stationRole() { return _station_role; }

    FactListModel *aircrafts() { return &_aircrafts; }

    Q_INVOKABLE FlightVechicle *currVehicle();
    Q_INVOKABLE void switchVehicel(int idx);
    Q_INVOKABLE void loadVehicleEnv(int count, const QString &routefile);

    void switchStationControlRole();
    void switchStationTaskRole();

    VehicleCommand *vehicleRC() { return &vehAdjustControl; }
    QTextToSpeech *textSpeech() { return txtToSpeech.get(); }
    RedrawSignal *redraw() { return &redrawSignal; }

    int vehicleId() { return _vehicleIndex; }

    void setVehicleId(int id) {
        _vehicleIndex = id;
        emit vehicleIdChanged();
    }

    int netChannel() { return _net_selected; }

    void setNetChannel(int id) {
        _net_selected = id;
        emit netChannelChanged();
    }

    static void registerType();

    void mockRTKDataFrame();

    void enableUpKeepalive();

    ///
    /// \brief manualSerailCommand
    /// \param newproto
    ///
    virtual void manualSerailCommand(ControlAdjustProto * newproto);

    ///
    /// \brief switchStationRoleMsg
    ///
    Q_INVOKABLE void switchStationRoleMsg();

    ///
    /// \brief unittest
    ///
    Q_INVOKABLE void unittestSimpleThd();
    Q_INVOKABLE void unittestRandSimple(int value, int jump, int type);

    ///
    /// \brief clearTxtSpeech
    ///
    Q_INVOKABLE void clearTxtSpeech();

    ///
    /// \brief stopService
    ///
    Q_INVOKABLE void stopService();
    Q_INVOKABLE void configService();

    ///
    /// \brief sendControlCommand
    ///
    Q_INVOKABLE void sendControlCommand(RCCommandItem *);

    /// 设备开关命令
    Q_INVOKABLE void sendDeviceOnOffCommand(RCAdjustItem *);

    ///
    Q_INVOKABLE void sendAdjustCommand(RCAdjustItem *);

    ///
    Q_INVOKABLE void sendAdjustATCCommand(RCAdjustItem *, int value);

    ///
    Q_INVOKABLE void sendDeviceSetCommand(RCAdjustItem *, QString);

    ///
    Q_INVOKABLE void sendMissionCommand(RCAdjustItem *);

    /// 更新光纤，信息源等设备命令
    Q_INVOKABLE void sendSimpleCommand(RCAdjustItem *);

    ///
    Q_INVOKABLE void sendDeviceCommand(RCAdjustItem *);
    Q_INVOKABLE void loadExcelProto(const QString &file);
    Q_INVOKABLE void loadExcelWarn(const QString &file);
    Q_INVOKABLE void loadRCProto(const QStringList &adjust, const QString &control);
    Q_INVOKABLE void exportRCData();
    Q_INVOKABLE void exportRMData();

    ///
    /// \brief clearSerailResource
    ///     删除串口缓存对象数据
    ///
    void clearSerailResource();

  protected:
    void updateDrawing();
    void lanchProcessSignal();
    void verifyFeedback(FlightVechicle *vehicle, FactTableModel *);
    void updateCommandAdjust(int vehKey, std::shared_ptr<FactDecoderMgr>);
    void verifyCommandAdjust(int);
    void calReadTimeout();
    bool isReadTimeout();

    ///
    /// \brief processSysWarn
    /// \param update
    /// \param channel
    ///
    void processSysWarn(bool update, int channel);

    ///
    /// \brief serialServiceWrite
    /// \param port
    /// \param proto
    ///
    void startSerialService();
    void startMainNetService();
    void startSlaveNetService();
    void addSendingCallback(MsgSendingCallback);
    void addProcessCallback(MsgProcessCallback);
    void addFeedbackCallback(FeedbackCallback);

    virtual void extentNetService(LoopHandle *) { ; }
    virtual void stopExtentNetService() { ; }
    virtual void saveService(int chlType, const void *adr, const uint8_t *buf, int leb) {}
    virtual void dataAfterProcess(Channel *mainNet, Channel *slaveNet, int chl) { ; }

    void dataProcess(Channel *mainNet, Channel *slaveNet);

    int dataAnalysis(Channel *mainNet, Channel *slaveNet, ProtoMessage *msg);

    void networkSend(Channel *mainNet, Channel *slaveNet);

    bool keepalive(Channel *mainNet, Channel *slaveNet);

    void transExtenMsg(Channel *channel, ControlAdjustProto *proto);

    ///
    /// \brief adjustControlProcess
    /// \param read_write
    /// \param queue
    /// \return
    ///
    bool adjustControlProcess(Channel *read_write, ControlAdjustProtoQueue &queue);

    FactDecoderMgr *protoManager(uint8_t);
    FactDecoderMgr *createProtoDecoder(uint8_t id);

    void decodeSoftComm(SoftwareCommProto *msg);
    bool controlAdjustProto(PtrMessage msg);
    void channelNotAvail(ControlAdjustProto *nodeProto, const QString &);

    FlightVechicle *serachVehicle(int);
    VehicleFeedback *feedbackByVehicle(int);

  signals:
    void verticalBarPos();
    void signalDrawing();
    void vehicleIdChanged();
    void netChannelChanged();
    void adjustTypesChanged();
    void commandTypesChanged();
    void currVehicleChanged();
    void aircraftsChanged();
    void checkWarnMessage();
    void stationRoleChanged();
    void mergeWarnSignal(int veh);
    void addWarnLevel(int veh, int level, int priority, const QString &value);
    void addressNotAvail(QString msg);

  protected:
    void writeOriginData(ProtoDataCache &cache, FILE *file, uint8_t *buffer, int len) {
        if (file != nullptr) {
            cache.addData(buffer, len);
            if (cache.length > PROTO_PACKAGE_LEN / 2) {
                fwrite(cache.data, 1, cache.length, file);
                cache.reset();
            }
        }
    }

    WindowShortCut win_shortcut;

    RockerProtoQueue serial_rtk_cache;
    MissionMessageQueue serial_mission_cache;
    ControlAdjustProtoQueue serial_write_cache;
    ControlAdjustProtoQueue mainnet_write_cache;
    ControlAdjustProtoQueue slavenet_write_cache;
    SoftwareCommProtoQueue softcomm_proto_cache;

    moodycamel::ConcurrentQueue<QString> txtSpeechQueue;
    moodycamel::ConcurrentQueue<FlightDisplay *> cmdOperateQueue;
    moodycamel::ConcurrentQueue<ProtoMessage *> data_read_cache;
    moodycamel::ConcurrentQueue<ProtoMessage *> data_write_cache;
    moodycamel::ConcurrentQueue<SoftwareCommProto> softcomm_write_cache;
    QMap<int, std::shared_ptr<FactDecoderMgr>> veh_proto_manager;
    std::list<std::shared_ptr<ControlAdjustVerify>> feedback_verify;

    std::atomic_bool adjust_control_upt = false;
    FlightVechicle *_currentVehicle = nullptr;
    FactDecoderMgr *protoDataSet = nullptr;

    QList<std::shared_ptr<FlightVechicle>> _vehicleList;

    FactListModel _aircrafts;
    RedrawSignal redrawSignal;
    std::mutex command_mutex;
    std::mutex display_mutex;

    std::mutex process_mutex;
    std::condition_variable process_condition;

    int64_t tm_linker_quality = 0;
    uint64_t next_queue_msg = 0;

    bool vehDataUpdate = 0;
    int count_during_msgs = 0;
    int _vehicleIndex = 0;
    int _timeoutCheck = 0;
    int _log_keepalive = 0;
    int _station_switch = 0;
    std::atomic<int> _station_role = GcsStationRole::ControlRole;
    std::atomic<int> _net_selected = ChannelSelection::MainNetwork;
    uint64_t next_keepalive = 0;
    uint64_t draw_updatefreq = 0;
    bool _upload_keepalive = 0;

    QString metaDataFile = "";
    QString metaWarnFile = "";

    QDateTime currDatetime;
    VehicleCommand vehAdjustControl;

    QVector<MsgSendingCallback> msgSendHandlers;
    QVector<MsgProcessCallback> msgProcessHandlers;
    QVector<FeedbackCallback> feedbackHandlers;
    std::shared_ptr<QTextToSpeech> txtToSpeech = nullptr;

    DataLinkerSummary main_linker_summary;
    DataLinkerSummary serial_linker_summary;
    DataLinkerSummary slave_linker_summary;
    VehilceWarnExport vehicle_warn_export;
    ConfigController *configCtrl = nullptr;
    // FILE *_chlControl[5] = {nullptr};
    // FILE *_chlTelemetry[5] = {nullptr};
    // ProtoDataCache cacheControl[5];
    // ProtoDataCache cacheTelemetry[5];
    QMap<QString, int64_t> warningFilter;
    uint64_t warn_updatefreq = 0;
    std::atomic_bool thread_status = false;
    std::atomic_bool serail_status = false;
    std::atomic_bool softcomm_status = false;
    std::atomic_bool savefile_status = false;

    std::thread serail_thd;
    std::thread softcomm_thd;
    std::thread net_data_thd;

    LoopHandle sock_loop;

    Channel slave_read_write;
    Channel main_read_write;

    MMRESULT net_data_timer = 0;
};
