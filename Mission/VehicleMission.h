#pragma once

#include "FLineController.h"
#include "FlightAircraft.h"
#include "GcsMissionCache.h"
#include "VehicleSession.h"
#include <QDateTime>
#include <QUdpSocket>
#include <phashmap/phmap.h>

using phmap::flat_hash_map;

#define UPLOAD_OPERATION   0X10
#define DOWNLOAD_OPERATION 0X20
class VehicleMission : public QObject {
    Q_OBJECT
    QPROPERTY_AUTO(bool, checked)
    Q_PROPERTY(bool showDown READ showDown WRITE setShowDown NOTIFY showDownChanged)
    Q_PROPERTY(FLineController *currFline READ currFline NOTIFY currFLineChanged)
    Q_PROPERTY(FlightVechicle *currVehicle READ currVehicle CONSTANT)
    Q_PROPERTY(QString vehName READ vehName CONSTANT)
    Q_PROPERTY(int vehicleIdx READ vehicleIdx CONSTANT)

  public:
    VehicleMission();
    ~VehicleMission();

    ///
    /// \brief releaseResource
    ///
    void releaseResource();

    ///
    /// \brief loadFlightLine
    /// \param filename
    /// \return
    ///
    Q_INVOKABLE int loadFlightLineFromFile(QString filename);

    /// \brief 切换航线
    ///
    Q_INVOKABLE int switchFlightline(StageController *fline);

    ///
    /// \brief mergeFlightLine
    /// \param task
    ///
    Q_INVOKABLE void mergeFlightLine(FLineController *task);

    ///
    /// \brief saveFlightLinue
    ///
    Q_INVOKABLE int exportFlightLine(QString file);

    ///
    /// \brief 切换飞行航点
    ///
    Q_INVOKABLE void switchWaypoint(StageController *fline, int idx);

    ///
    /// \brief downloadFlightLine
    /// \param stagecode
    ///
    Q_INVOKABLE int downloadFlightLine(int linecode);

    ///
    /// \brief setSessionVehicle
    /// \param session
    /// \param veh
    ///
    void setSessionVehicle(VehicleSession *session, FlightVechicle *veh);

    FlightVechicle *currVehicle() { return curr_vehicle; }

    Q_INVOKABLE void mockEnable();

    ///
    /// \brief uploadFline
    /// \param flightline
    ///
    ///
    Q_INVOKABLE int uploadStage(StageController *fline);
    Q_INVOKABLE int uploadFline(const FlineStageDetail *flightline);
    Q_INVOKABLE int uploadFline();

    static int64_t currentTimeMs();

    FLineController *currFline();

    ///
    /// \brief missionProcess
    ///     任务指令发送逻辑
    /// \param read_write
    ///
    void missionProcess(Channel *read_write, MissionMessageQueue &taskqueue);

    ///
    /// \brief processMissionMsg
    ///     任务指令响应
    /// \param protomsg
    ///
    void processMissionMsg(VehicleMissionProto *protomsg);

    ///
    /// \brief mapNoticeMessage
    ///     指令操作结果提示
    /// \param feed
    ///
    void mapNoticeMessage(int feed);

    ///
    /// \brief missionFeedbackVerify
    ///     任务指令反馈处理
    /// \param feed
    ///
    void missionFeedbackVerify(int feed);

    int vehicleIdx();
    QString vehName();

    void setShowDown(bool val);
    bool showDown() { return show_downline; }

  private slots:
    void processMessionDownload(int);
    void templineMission();
    void offlineMission();

  signals:
    void notifyOperateResult(QString value);
    void notifyDownloadResult(int code);
    void showDownChanged();
    void currFLineChanged();
    void downloadDataFinish(int);

  protected:
    void nextUploadDataVerify(VehicleMissionProto *protomsg);
    void nextUploadData(VehicleMissionProto *protomsg);
    void nextDownloadDataVerify(VehicleMissionProto *protomsg);
    void nextDownloadData(VehicleMissionProto *protomsg);
    void mockDownloadResponse(VehicleMissionProto *protomsg);
    void mockUploadResponse(VehicleMissionProto *protomsg);
    void processMissionUpload(VehicleMissionProto *protomsg);
    void processMissionDownload(VehicleMissionProto *protomsg);
    void missionOperateTimeout(int cmdtype, int timeout);

    using NextOperateCallback = std::function<void(VehicleMissionProto *)>;
    using TimeoutVerifyCallback = std::function<void(int type, int timeout)>;

    struct ExpectFeedback {
        uint8_t resendCount = 0;
        uint8_t expectKey = 0;
        uint8_t expectVal = 0;
        uint8_t downtype = 0;
        qint64 timeout = 0;
        TimeoutVerifyCallback timeouts = nullptr;
        NextOperateCallback nextoperate = nullptr;
        std::shared_ptr<VehicleMissionProto> taskProto = nullptr;
    };

    static uint16_t missionCRC16(const uint8_t *head, uint16_t len);
    static uint16_t missionCRC32(const uint8_t *head, uint16_t len);

  private:
    struct SummaryBandwidth {
        int32_t databytes = 0;
        int64_t next_time = 0;
    };

    bool show_downline = false;

    enum UploadDownloadState {
        DownloadInit = DOWNLOAD_OPERATION + 1, //
        DownloadData = DOWNLOAD_OPERATION + 2, //
        DownloadCRC = DOWNLOAD_OPERATION + 3,  //
        DownloadEnd = DOWNLOAD_OPERATION + 4   //
    };

    enum UploadState {
        UploadInit = UPLOAD_OPERATION + 1, //
        UploadData = UPLOAD_OPERATION + 2, //
        UploadCRC = UPLOAD_OPERATION + 3,  //
        UploadEnd = UPLOAD_OPERATION + 4,  //
    };

    std::atomic<int> upload_download = 0;

    QUdpSocket unit_udpsocket;
    ConfigController unit_config;
    int start_down_req = -1;
    bool unit_test_mock = false;

    SummaryBandwidth _bandwidth;
    MissionUpDownCache up_down_cache;
    VehicleSession *veh_session = nullptr;

    FlightVechicle *curr_vehicle = nullptr;
    FLineController up_down_fightline;
    std::shared_ptr<ExpectFeedback> up_down_proto = nullptr;
};
