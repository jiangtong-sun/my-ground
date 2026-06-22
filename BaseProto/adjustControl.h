#pragma once

#include "CommandGroup.h"
#include "factmetadata.h"
#include "prototelemetry.h"
#include "xlsxworkbook.h"

///
/// \brief AdjustControl 读写遥控遥调数据文件，
/// 每一组遥调数据读取到RCAdjustGroup对象，遥调数据分组存储到groupAdjust_set对象
/// 每一组遥控数据读取到RCCommandGroup对象，遥控数据分组存储到commandtype_set对象
///
///
class BASEPROTO_EXPORT AdjustControl : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(AdjustControl)

  public:
    Q_PROPERTY(QString controlFile READ controlFile CONSTANT)
    Q_PROPERTY(QStringList adjustFile READ adjustFile CONSTANT)

    Q_PROPERTY(int selCmdGroup READ selCmdGroup WRITE setSelCmdGroup NOTIFY selCmdGroupChanged)
    Q_PROPERTY(RCCommandGroup *extentCmdGroup READ extentCmdGroup NOTIFY extCmdGroupChanged)
    Q_PROPERTY(RCCommandGroup *commandGroup READ commandGroup NOTIFY commandGroupChanged)
    Q_PROPERTY(RCCommandGroup *sysCmdGroup READ sysCmdGroup NOTIFY sysCmdGroupChanged)
    Q_PROPERTY(FactListModel *commandTypes READ commandTypes NOTIFY commandTypesChanged)
    Q_PROPERTY(FactListModel *curSysGroupCmd READ curSysGroupCmd CONSTANT)

    Q_PROPERTY(RCAdjustGroup *adjustTypes READ adjustTypes CONSTANT)
    Q_PROPERTY(RCAdjustGroup *vhfWorkFreqSource READ vhfWorkFreqSource CONSTANT)
    Q_PROPERTY(RCAdjustGroup *dataLinkSource READ dataLinkSource CONSTANT)
    Q_PROPERTY(RCAdjustGroup *dataLinkDevSource READ dataLinkDevSource CONSTANT)

    Q_PROPERTY(RCAdjustGroup *atcDeviceSource READ atcDeviceSource CONSTANT)

    Q_PROPERTY(RCAdjustGroup *batteryOnOff READ batteryOnOff CONSTANT)
    Q_PROPERTY(RCAdjustGroup *cmdAdjusts READ cmdAdjusts NOTIFY cmdAdjustsChanged)
    Q_PROPERTY(RCAdjustGroup *devicePowerOnOff READ devicePowerOnOff CONSTANT)
    Q_PROPERTY(RCAdjustGroup *slideingAdjust READ slideingAdjust CONSTANT)
    Q_PROPERTY(RCAdjustGroup *landingAdjust READ landingAdjust CONSTANT)
    Q_PROPERTY(RCAdjustGroup *curiseAdjust READ curiseAdjust CONSTANT)
    Q_PROPERTY(RCCommandGroup *controlRateControl READ controlRateControl CONSTANT)
    Q_PROPERTY(RCCommandGroup *infoSrcControl READ infoSrcControl CONSTANT)
    Q_PROPERTY(RCCommandGroup *generatorControl READ generatorControl CONSTANT)

    void loadVehicleEnv(int count, const QString &routefile);

    AdjustControl();
    ~AdjustControl();

    FactListModel *curSysGroupCmd() { return &_sys_opt_cmds.operate_cmds; }

    Q_INVOKABLE void updateSysCmdGroup(QString name);
    Q_INVOKABLE void uptExtCmdGroup(QString type);

    RCAdjustGroup *groupAdjustCmd(QString type);

    RCCommandGroup *taskInitialize();
    RCCommandGroup *extentCmdGroup();
    RCCommandGroup *sysCmdGroup();
    RCAdjustGroup *cmdAdjusts();
    RCAdjustGroup *batteryOnOff();
    RCAdjustGroup *devicePowerOnOff();
    RCAdjustGroup *slideingAdjust();
    RCAdjustGroup *landingAdjust();
    RCAdjustGroup *curiseAdjust();
    RCAdjustGroup *adjustTypes();
    RCAdjustGroup *dataLinkSource();
    RCAdjustGroup *dataLinkDevSource();
    RCAdjustGroup *vhfWorkFreqSource();

    RCAdjustGroup *atcDeviceSource();

    RCCommandGroup *controlRateControl();
    RCCommandGroup *infoSrcControl();
    RCCommandGroup *generatorControl();

    RCCommandGroup *commandGroup();
    int selCmdGroup() { return _last_ctrl_group; }
    void setSelCmdGroup(int va);

    ///
    /// \brief selectAdjustGroup
    /// \param type
    /// \return
    ///

    RCAdjustGroup *selectAdjustGroup(const QString &type);

    ///
    /// \brief selectAdjustItem
    /// \param type
    /// \param code
    /// \return
    ///
    ///
    RCAdjustItem *selectAdjustItem(const QString &type, int code) const;

    ///
    /// \brief commandTypes
    /// \return
    ///
    FactListModel *commandTypes();

    QString controlFile() { return _controlFile; }

    QStringList adjustFile() { return _adjustFile; }

    ///
    /// \brief verifyCommandAdjust
    ///

    void verifyCommandAdjust(int);

    ///
    /// \brief reloadMoreAdjust
    /// \param files
    /// \return
    ///
    bool reloadMoreAdjust(const QStringList &files);

    ///
    /// \brief reloadAdjusts
    /// \param file
    /// \return
    ///
    bool reloadAdjusts(const QString &file);

    ///
    /// \brief reloadCommands
    ///     read control file, parse file's content as command object.
    /// \param file
    /// \return
    ///
    bool reloadCommands(const QString &file);

    ///
    /// \brief getAdjustByfeedback
    /// \param code
    /// \param cmdtype
    /// \return
    ///
    RCAdjustItem *getAdjustByfeedback(int code, int cmdtype, int subtype) const;
    RCAdjustItem *getAdjustByfeedback(int code) const;

    ///
    /// \brief getControlByFeedback
    /// \param code
    /// \return
    ///
    RCCommandItem *getControlByFeedback(int code) const;

    ///
    /// \brief readSheetHeader
    /// \param header
    /// \param sheet
    /// \return
    ///
    int readSheetHeader(QMap<QString, int> &header, QXlsx::Worksheet *sheet);

    ///
    /// \brief reloadEnumSheet
    /// \param sheet
    /// \param mapEnum
    ///
    void reloadEnumSheet(QXlsx::Worksheet *sheet, QMap<int, QString> &mapEnum);

    ///
    /// \brief reloadStructSheet
    /// \param data
    /// \param sheet
    /// \param mapMeta
    ///
    ///
    void reloadStructSheet(RCAdjustItem *data, QXlsx::Worksheet *sheet,
                           QMap<QString, MetaValueType> &mapMeta);

    ///
    /// \brief reloadStructSheet
    /// \param data
    /// \param sheet
    /// \param mapMeta
    ///
    void reloadStructSheet(TransMetaData *data, QXlsx::Worksheet *sheet,
                           QMap<QString, MetaValueType> &mapMeta);

    ///
    /// \brief reloadStruct
    /// \param row
    /// \param rlen
    /// \param columnStatus
    /// \param mapMetaValue
    /// \param groupname
    /// \param worksheet
    /// \return
    ///
    int reloadStruct(int row, int rlen, QMap<QString, int> &columnStatus,
                     QMap<QString, MetaValueType> &mapMetaValue, const QString &groupname,
                     const QXlsx::Worksheet *worksheet, QXlsx::Document *, std::shared_ptr<RCAdjustItem>);

  signals:
    void cmdAdjustsChanged();
    void sysCmdGroupChanged();
    void selCmdGroupChanged();
    void commandGroupChanged();
    void commandTypesChanged();
    void extCmdGroupChanged();

  protected:
    QString sys_cmds_grp = "";
    QString ext_cmds_grp = "";
    QString _controlFile = "";
    QStringList _adjustFile;

    int _last_ctrl_group = 0;
    int _last_sys_group = 0;

    struct OptCommandGroup {
        FactListModel *cmds() { return &operate_cmds; }
        RCCommandGroup *commandGroup(const QString &val);
        RCCommandGroup *commandGroup(int val);
        bool resetCommandState(int code) const;

        FactListModel operate_cmds;
        QList<std::shared_ptr<RCCommandGroup>> command_group;
    };

    QMap<int, std::shared_ptr<RCCommandItem>> all_command_map;
    QMap<int, std::shared_ptr<RCCommandItem>> ext_command_map;
    QMap<QString, std::shared_ptr<RCAdjustGroup>> groupAdjust_set;

    OptCommandGroup _ext_opt_cmds;
    OptCommandGroup _sys_opt_cmds;
    OptCommandGroup _command_types;
};

class BASEPROTO_EXPORT ControlAdjustVerify {
    PtrProtoMessage msg_callback = nullptr;
    uint8_t command_seq = 0;
    uint8_t expect_veh = 0;
    int16_t verify_row = 0;
    int16_t expect_result = 0;
    int16_t feedback_adjust = -1;
    int16_t feedback_control = -1;
    uint8_t feedback_seq = 0;
    int16_t during = 0;
    int16_t print_freq = 0;
    qint64 last_upt = 0;
    QString expect_name = "";

  public:
    RCAdjustItem *adjusts = nullptr;
    RCCommandItem *control = nullptr;
    uint8_t commandSeq() const { return command_seq; }
    qint64 lastUpdateTime() const { return last_upt; }
    QString expectName() const { return expect_name; }
    uint8_t vechile() const { return expect_veh; }
    int16_t verifyRow() const { return verify_row; }
    int16_t expectVal() const { return expect_result; }
    int16_t duringTime() const { return during; }

    int16_t controlCode() const { return feedback_control; }
    int16_t adjustCode() const { return feedback_adjust; }
    int16_t feedbackSeq() const { return feedback_seq; }

    ControlAdjustVerify() : adjusts(nullptr), control(nullptr) {
        expect_name = "";
        command_seq = 0;
        expect_veh = 0;
        verify_row = 0;
        expect_result = 0;
        feedback_adjust = -1;
        feedback_control = -1;
        during = 0;
        last_upt = 0;
        expect_name = "";
    }

    void setFeedback(int16_t ctrl, int16_t adjust, uint8_t seq = 0);
    void setCallback(PtrProtoMessage back) { msg_callback = std::move(back); }
    void setExpect(const QString &name, uint8_t seq, uint8_t veh, int32_t row, int32_t expect);
    bool isTimeout(uint32_t timeout = 1200);
    bool delayTimeout();
    ///
    /// \brief isExpectValue
    /// \return
    ///     0x01 == ok
    ///     0x02 == invalid
    ///     0x03 == unable condition
    ///
    int isExpectValue(AdjustControl *);
    int isExpectValue(AdjustControl *, bool seq);
};
