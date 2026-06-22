#pragma once

#include "FlightAircraft.h"
#include "adjustcontrol.h"
#include "concurrentqueue.h"
#include "factdecodermgr.h"
#include "prototelemetry.h"

typedef moodycamel::ConcurrentQueue<PtrRockerMessage> RockerProtoQueue;
typedef moodycamel::ConcurrentQueue<PtrMissionMessage> MissionMessageQueue;
typedef moodycamel::ConcurrentQueue<PtrProtoMessage> ControlAdjustProtoQueue;
typedef moodycamel::ConcurrentQueue<PtrSoftwareMessage> SoftwareCommProtoQueue;

class VehicleCommand : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(VehicleCommand)

  public:
    Q_PROPERTY(AdjustControl *commands READ commands CONSTANT)
    Q_PROPERTY(bool rcMode READ rcMode WRITE setRCMode NOTIFY rcModeChanged)

    VehicleCommand(ControlAdjustProtoQueue &serial, ControlAdjustProtoQueue &mainqueue,
                   ControlAdjustProtoQueue &slavequeue);
    ~VehicleCommand();

    AdjustControl *commands() { return &vehAdjustControl; }

    RCCommandGroup *taskInitialize() { return vehAdjustControl.taskInitialize(); }

    void setRCMode(bool type);
    int rcMode() { return _rcModeType; }
    int cmdNumber() { return (++_cmdNumber) % 256; }

    bool reloadAdjusts(const QStringList &file);

    bool reloadCommands(const QString &file);

    ///
    /// \brief networkKeepalive
    /// \param serial
    /// \param proto
    ///
    void networkKeepalive(bool serial, ControlAdjustProto &proto, int chl);

    ///
    /// \brief isSameCommand
    /// \param seq
    /// \return
    ///
    bool isSameCommand(uint8_t seq);

    void exportRCData();
    void parseRCBytes(QTextStream& out, QByteArray & next);
    void exportRMData();

    ///
    /// \brief feedbackVerify
    /// \param vehicle
    /// \param nodeProto
    /// \return
    ///
    std::shared_ptr<ControlAdjustVerify> feedbackVerify(FlightVechicle *vehicle, ControlAdjustProto *nodeProto);

    void sendControlCommand(RCCommandItem *, FactDecoderMgr *fact);
    void sendControlAdjustCommand(RCCommandItem *, FactDecoderMgr *fact);

    void sendDeviceOnOffCommand(RCAdjustItem *, FactDecoderMgr *fact);
    void sendAdjustCommand(RCAdjustItem *, FactDecoderMgr *fact);
    void sendAdjustATCSpecial(RCAdjustItem *, FactDecoderMgr *fact, int value);
    void sendAdjustATCCommand(RCAdjustItem *, FactDecoderMgr *fact);

    void sendDeviceSetCommand(RCAdjustItem *, const QString &, FactDecoderMgr *fact);

    void sendMissionCommand(RCAdjustItem *, FactDecoderMgr *fact);
    void sendSimpleCommand(RCAdjustItem *, FactDecoderMgr *fact);
    void sendDeviceCommand(RCAdjustItem *, FactDecoderMgr *fact);

    void contrlRefAdjustCmd(RCAdjustItem *data, RCCommandItem *ctl, FactDecoderMgr *fact);
    void contrlRefAdjustCmdExt(RCAdjustItem *data, FactDecoderMgr *fact, int code);

    void fillProtoTime(ControlAdjustProto &proto);

    template <class Type>
    Type inputDataTrans(FactListModel * subModel);

  protected:
    static PtrProtoMessage newControlAdjustProto(ControlAdjustProto &adjust);
    void doAdjust(ControlAdjustProto *adjust, RCAdjustItem *, FactDecoderMgr *fact);

  signals:
    void verticalBarPos();
    void signalDrawing();
    void vehicleIdChanged();
    void rcModeChanged();
    void adjustTypesChanged();
    void commandTypesChanged();
    void currVehicleChanged();
    void aircraftsChanged();

  protected:
    ControlAdjustProtoQueue &serial_write_cache;
    ControlAdjustProtoQueue &main_write_cache;
    ControlAdjustProtoQueue &slave_write_cache;
    AdjustControl vehAdjustControl;
    bool _rcModeType = true;
    uint8_t _cmdNumber = 0;
    uint8_t last_operate_cmd = 0;
    qint64 last_operate_tm = 0;
};
