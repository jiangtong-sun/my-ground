#pragma once

#include "FlightDevice.h"
#include "FlightWarnGroup.h"
#include "VehicleAttitude.h"
#include "VehicleElement.h"
#include "VehilceFeedback.h"
#include "transmetadata.h"

#include <QGeoCoordinate>
#include <QVariantList>

class VEHICLE_EXPORT FlightVechicle : public QObject {

    Q_OBJECT
    Q_DISABLE_COPY(FlightVechicle)

  public:
    FlightVechicle();
    ~FlightVechicle();

    Q_PROPERTY(int minAirspeed READ minAirspeed CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setVehName NOTIFY vehNameChanged)
    Q_PROPERTY(qreal heading READ heading WRITE setHeading NOTIFY headingChanged)
    Q_PROPERTY(int vehicleIdx READ vehicleIdx WRITE setVehicleIdx NOTIFY vehicleIdxChanged)
    Q_PROPERTY(int deviceIdx READ deviceIdx WRITE setDeviceIdx NOTIFY deviceIdxChanged)
    Q_PROPERTY(FlightDevice *deviceSelect READ deviceSelect NOTIFY deviceSelectChanged)
    Q_PROPERTY(QGeoCoordinate position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(FlightStatus *rcAssociated READ rcAssociated NOTIFY rcAssociatedChanged)
    Q_PROPERTY(FlightWarnGroup *warnLevelA READ warnLevelA CONSTANT)
    Q_PROPERTY(FlightWarnGroup *warnLevelB READ warnLevelB CONSTANT)
    Q_PROPERTY(FlightWarnGroup *warnLevelC READ warnLevelC CONSTANT)

    Q_PROPERTY(WarnConfigGroupVal *warnConfGroup READ warnConfGroup CONSTANT)

    Q_PROPERTY(FlightDevice *flightManage READ flightManage CONSTANT)
    Q_PROPERTY(FlightDevice *navigateSensor READ navigateSensor CONSTANT)
    Q_PROPERTY(FlightDevice *enginePower READ enginePower CONSTANT)
    Q_PROPERTY(FlightDevice *deviceExecutor READ deviceExecutor CONSTANT)
    Q_PROPERTY(FlightDevice *dataLinker READ dataLinker CONSTANT)
    Q_PROPERTY(FlightDevice *airTrafficControl READ airTrafficControl CONSTANT)
    Q_PROPERTY(FlightDevice *extraDevice READ extraDevice CONSTANT)

    Q_PROPERTY(FlightStatus *mainNavBlock READ mainNavBlock CONSTANT)
    Q_PROPERTY(FlightStatus *mainCommBlock READ mainCommBlock CONSTANT)
    Q_PROPERTY(FlightStatus *navKeyPage READ navKeyPage CONSTANT)
    Q_PROPERTY(FlightStatus *taskKeyPage READ taskKeyPage CONSTANT)
    Q_PROPERTY(FlightStatus *flightCommonA READ flightCommonA CONSTANT)
    Q_PROPERTY(FlightStatus *flightCommonB READ flightCommonB CONSTANT)

    Q_PROPERTY(FlightStatus *attiLeftShow READ attiLeftShow CONSTANT)
    Q_PROPERTY(FlightStatus *attiCenterShow READ attiCenterShow CONSTANT)
    Q_PROPERTY(FlightStatus *attiRightShow READ attiRightShow CONSTANT)
    Q_PROPERTY(FactListModel *deviceList READ deviceList CONSTANT)
    Q_PROPERTY(SimpleElemList *generatorMode READ generatorMode CONSTANT)
    Q_PROPERTY(FlightStatus *missionMode READ missionMode CONSTANT)
    Q_PROPERTY(FlightStatus *flightStates READ flightStates CONSTANT)
    Q_PROPERTY(FlightStatus *vehicleMode READ vehicleMode CONSTANT)
    Q_PROPERTY(FlightStatus *fmodelStates READ fmodelStates CONSTANT)
    Q_PROPERTY(SimpleElemList *engineMode READ engineMode CONSTANT)
    Q_PROPERTY(SimpleElemList *temperatureMode READ temperatureMode CONSTANT)
    Q_PROPERTY(SimpleElemList *batteryMode READ batteryMode CONSTANT)
    Q_PROPERTY(SimpleElemList *eimcMode READ eimcMode CONSTANT)
    Q_PROPERTY(SimpleElemList *flapRunnder READ flapRunnder CONSTANT)
    Q_PROPERTY(SimpleElemList *elecSpdCtrl READ elecSpdCtrl CONSTANT)
    Q_PROPERTY(SimpleElemList *navigationState READ navigationState CONSTANT)
    Q_PROPERTY(BakeComponent *brakeMode READ brakeMode CONSTANT)
    Q_PROPERTY(FlightDisplay *leftWheelState READ leftWheelState CONSTANT)
    Q_PROPERTY(FlightDisplay *rightWheelState READ rightWheelState CONSTANT)
    Q_PROPERTY(FlightDisplay *frontWheelState READ frontWheelState CONSTANT)

    Q_PROPERTY(AttributeTable *attitudeModel READ attitudeModel CONSTANT)
    Q_PROPERTY(VehicleFeedback *feedback READ feedback CONSTANT)

    Q_PROPERTY(FlightDisplay *AirOrGround READ AirOrGround CONSTANT)
    Q_PROPERTY(FlightDisplay *CurrentNav READ CurrentNav CONSTANT)
    Q_PROPERTY(FlightDisplay *PlaneAddress READ PlaneAddress CONSTANT)
    Q_PROPERTY(FlightDisplay *DeclineRatio READ DeclineRatio CONSTANT)
    Q_PROPERTY(FlightDisplay *NavigationMode READ NavigationMode CONSTANT)
    Q_PROPERTY(FlightDisplay *LateralMode READ LateralMode CONSTANT)
    Q_PROPERTY(FlightDisplay *LongitudinalMode READ LongitudinalMode CONSTANT)
    Q_PROPERTY(FlightDisplay *FrontWheelAngle READ FrontWheelAngle CONSTANT)
    Q_PROPERTY(FlightDisplay *LeftBrake READ LeftBrake CONSTANT)
    Q_PROPERTY(FlightDisplay *RightBrake READ RightBrake CONSTANT)
    Q_PROPERTY(FlightDisplay *DifferYaw READ DifferYaw CONSTANT)
    Q_PROPERTY(FlightDisplay *TargetYaw READ TargetYaw CONSTANT)
    Q_PROPERTY(FlightDisplay *LineWaypoint READ LineWaypoint CONSTANT)
    Q_PROPERTY(FlightDisplay *LineStage READ LineStage CONSTANT)
    Q_PROPERTY(FlightDisplay *WindSpeed READ WindSpeed CONSTANT)
    Q_PROPERTY(FlightDisplay *WindDirection READ WindDirection CONSTANT)
    Q_PROPERTY(FlightDisplay *LateralOffset READ LateralOffset CONSTANT)
    Q_PROPERTY(FlightDisplay *Attack READ Attack CONSTANT)
    Q_PROPERTY(FlightDisplay *SlideAngle READ SlideAngle CONSTANT)
    Q_PROPERTY(FlightDisplay *YawAngle READ YawAngle CONSTANT)
    Q_PROPERTY(FlightDisplay *PitchAngle READ PitchAngle CONSTANT)
    Q_PROPERTY(FlightDisplay *RollAngle READ RollAngle CONSTANT)
    Q_PROPERTY(FlightDisplay *Airspeed READ Airspeed CONSTANT)
    Q_PROPERTY(FlightDisplay *AirspeedSet READ AirspeedSet CONSTANT)
    Q_PROPERTY(FlightDisplay *HeightSet READ HeightSet CONSTANT)
    Q_PROPERTY(FlightDisplay *TrueAirspeed READ TrueAirspeed CONSTANT)
    Q_PROPERTY(FlightDisplay *FlightAngle READ FlightAngle CONSTANT)
    Q_PROPERTY(FlightDisplay *GroundSpeed READ GroundSpeed CONSTANT)
    Q_PROPERTY(FlightDisplay *VerticalSpeed READ VerticalSpeed CONSTANT)
    Q_PROPERTY(FlightDisplay *IndicateSpeed READ IndicateSpeed CONSTANT)
    Q_PROPERTY(FlightDisplay *Elevation READ Elevation CONSTANT)
    Q_PROPERTY(FlightDisplay *RadioAltimeter READ RadioAltimeter CONSTANT)
    Q_PROPERTY(FlightDisplay *HeightSource READ HeightSource CONSTANT)
    Q_PROPERTY(FlightDisplay *HeightOffset READ HeightOffset CONSTANT)
    Q_PROPERTY(FlightDisplay *QNE READ QNE CONSTANT)
    Q_PROPERTY(FlightDisplay *QNH READ QNH CONSTANT)
    Q_PROPERTY(FlightDisplay *QFE READ QFE CONSTANT)
    Q_PROPERTY(FlightDisplay *DTG READ DTG CONSTANT)
    Q_PROPERTY(FlightDisplay *MLD READ MLD CONSTANT)

    QGeoCoordinate position() const { return _position; }
    void setPosition(QGeoCoordinate pos);

    FactListModel *deviceList() { return &_deviceList; }

    FlightDevice *flightManage();
    FlightDevice *navigateSensor();
    FlightDevice *enginePower();
    FlightDevice *deviceExecutor();
    FlightDevice *dataLinker();
    FlightDevice *airTrafficControl();
    FlightDevice *extraDevice();
    FlightDevice *deviceSelect();

    Q_INVOKABLE void setRcGroupName(QString val);
    Q_INVOKABLE void updateDeviceModel(int idx);
    Q_INVOKABLE void clearFlightline();

    QString name() const { return _name; }
    qreal heading() const { return _heading; }
    int minAirspeed() const { return _min_airspeed; }
    int vehicleIdx() const { return _vehicleIdx; }
    int vehicleAddr() const { return _vehicleAddr; }
    int deviceIdx() const { return _deviceIdx; }

    void setVehName(const QString &val) {
        _name = val; // vehicle name
        emit vehNameChanged();
    }

    void setHeading(qreal val) {
        _heading = val;
        emit headingChanged();
    }

    void setVehicleAddr(int val);
    void setVehicleIdx(int val);
    void setDeviceIdx(int val);
    void setCurrNavMode(int val);

    static int64_t currentTimeMs();

    void summaryDataLink(int datas, int frame);
    FlightDisplay *getDeviceByName(const QString &);
    FlightDisplay *searchMeta(const QString &) const;
    FlightDevice *searchMode(const QString &) const;

    FlightStatus *rcAssociated();
    FlightStatus *attiLeftShow();
    FlightStatus *attiCenterShow();
    FlightStatus *attiRightShow();

    FlightStatus *mainNavBlock();
    FlightStatus *mainCommBlock();
    FlightStatus *navKeyPage();
    FlightStatus *taskKeyPage();
    FlightStatus *flightCommonA();
    FlightStatus *flightCommonB();

    SimpleElemList *generatorMode() { return &_generatorMode; }
    FlightStatus *missionMode() { return &_missionElem; }
    FlightStatus *flightStates() { return &_flightStates; }
    FlightStatus *vehicleMode() { return &_vehicleMode; }
    FlightStatus *fmodelStates() { return &_modelStates; }

    SimpleElemList *engineMode() { return &_engineElem; }
    SimpleElemList *temperatureMode() { return &_temperatureElems; }
    SimpleElemList *batteryMode() { return &_batteryElems; }
    SimpleElemList *eimcMode() { return &_emicElems; }
    SimpleElemList *flapRunnder() { return &_flapRundder; }
    SimpleElemList *elecSpdCtrl() { return &_elecSpdCtrl; }
    SimpleElemList *navigationState() { return &_navigationElems; }
    BakeComponent *brakeMode() { return &_brakeElems; }

    FlightDisplay *leftWheelState();
    FlightDisplay *rightWheelState();
    FlightDisplay *frontWheelState();
    FlightDisplay *HeightSource();
    FlightDisplay *CurrentNav();
    FlightDisplay *PlaneAddress();
    FlightDisplay *AirOrGround();
    FlightDisplay *DeclineRatio();
    FlightDisplay *NavigationMode();
    FlightDisplay *LateralMode();
    FlightDisplay *LongitudinalMode();
    FlightDisplay *FrontWheelAngle();
    FlightDisplay *LeftBrake();
    FlightDisplay *RightBrake();
    FlightDisplay *DifferYaw();
    FlightDisplay *TargetYaw();
    FlightDisplay *LineWaypoint();
    FlightDisplay *LineStage();
    FlightDisplay *WindSpeed();
    FlightDisplay *WindDirection();
    FlightDisplay *LateralOffset();
    FlightDisplay *Attack();
    FlightDisplay *SlideAngle();
    FlightDisplay *YawAngle();
    FlightDisplay *PitchAngle();
    FlightDisplay *RollAngle();
    FlightDisplay *Airspeed();
    FlightDisplay *AirspeedSet();
    FlightDisplay *HeightSet();
    FlightDisplay *TrueAirspeed();
    FlightDisplay *FlightAngle();
    FlightDisplay *HeightOffset();
    FlightDisplay *GroundSpeed();
    FlightDisplay *VerticalSpeed();
    FlightDisplay *IndicateSpeed();
    FlightDisplay *Elevation();
    FlightDisplay *RadioAltimeter();
    FlightDisplay *QNE();
    FlightDisplay *QNH();
    FlightDisplay *QFE();
    FlightDisplay *DTG();
    FlightDisplay *MLD();

    AttributeTable *attitudeModel(void);

    FlightWarnGroup *warnLevelA() { return &deviceWarnGroupA; }
    FlightWarnGroup *warnLevelB() { return &deviceWarnGroupB; }
    FlightWarnGroup *warnLevelC() { return &deviceWarnGroupC; }

    void mergeWarnGroup() {}

    void addWarnGroup(int priority, int level, const QString &value) {
        if (level == WarningLevel::LevelOne) {
            deviceWarnGroupA.addWarnGroup(priority, level, value);
        } else if (level == WarningLevel::LevelTwo) {
            deviceWarnGroupB.addWarnGroup(priority, level, value);
        } else if (level == WarningLevel::LevelThree) {
            deviceWarnGroupC.addWarnGroup(priority, level, value);
        }
    }

    void calculateWarnGroup() {
        deviceWarnGroupA.calculateWarnGroup();
        deviceWarnGroupB.calculateWarnGroup();
        deviceWarnGroupC.calculateWarnGroup();
    }

    void doUnitTest(int testval);

    static void registerType();

    bool loadExcelWarn(const QString &dataFile, FactTableModelCallabck tableModel) {
        return ParseProtoFile::warnParse(warn_ext, tableModel, dataFile);
    }

    VehicleFeedback *feedback() const { return const_cast<VehicleFeedback *>(&_feedbackModel); }

    bool reloadDisplay(FactDecoderMgr *decoder, const QString &file);

    void ReadModule(const QXlsx::Worksheet *currSheet, QMap<QString, int> &columnStatus, FlightStatus *groupCommand,
                    FactDecoderMgr *decoder, int start, int end);

    void ReadModule(const QXlsx::Worksheet *currSheet, QMap<QString, int> &columnStatus,
                    std::shared_ptr<FlightDevice> &module, FactDecoderMgr *decoder, int frow, int lrow);

    void ReadModule(const QXlsx::Worksheet *currSheet, QMap<QString, int> &columnStatus, AttributeTable &module,
                    FactDecoderMgr *decoder, int frow, int lrow);

    void uptVechiclePos(double lng, double lat, double alt);

    void decodeWarnData(CheckWarnCallback first, FactTableModelCallabck tableModelCB, int chl);

    void notifyDataUpdate(CheckWarnCallback first, int chl);

    WarnConfigGroupVal *warnConfGroup() {
        if (!warn_ext.empty()) {
            return warn_ext.begin().value().get();
        }
        return nullptr;
    }

  signals:
    void trajectoryChanged(QGeoCoordinate);
    void positionChanged(QGeoCoordinate);
    void securityMinDisChanged();
    void linkerChanged();
    void vehNameChanged();
    void headingChanged();
    void vehicleIdxChanged();
    void deviceSelectChanged();
    void rcGroupNameChanged();
    void rcAssociatedChanged();
    void deviceIdxChanged();

  protected:
    QString _name;
    QString _rcGrpName;
    qreal _heading = 0;
    int _vehicleIdx = 0;
    int _vehicleAddr = 0;
    int _deviceIdx = 0;
    int _min_airspeed = 1;
    int lastHighOffset = 6000;
    double lastlongitude = 0;
    double lastlatitude = 0;
    std::atomic_bool _clr_fline = false;
    FlightDisplay *_highOffset = nullptr;

    FlightDisplay nullMData;
    QGeoCoordinate _position;
    VehicleFeedback _feedbackModel;

    FlightWarnGroup deviceWarnGroupA;
    FlightWarnGroup deviceWarnGroupB;
    FlightWarnGroup deviceWarnGroupC;

    AttributeTable _attitudeModel;
    BakeComponent _brakeElems;
    SimpleElemList _emicElems;
    SimpleElemList _batteryElems;
    SimpleElemList _vehicleStatus;
    SimpleElemList _temperatureElems;
    SimpleElemList _engineElem;
    SimpleElemList _flapRundder;
    SimpleElemList _elecSpdCtrl;
    SimpleElemList _navigationElems;
    SimpleElemList _generatorMode;
    FlightStatus *_rcAssociated = nullptr;
    FlightStatus _missionElem;
    FlightStatus _flightStates;
    FlightStatus _vehicleMode;
    FlightStatus _modelStates;
    FlightStatus _mainNavBlock;
    FlightStatus _mainCommBlock;
    FlightStatus _navKeyPage;
    FlightStatus _taskKeyPage;
    FlightStatus _flightCommonA;
    FlightStatus _flightCommonB;
    FlightStatus _attiLeftShow;
    FlightStatus _attiCenterShow;
    FlightStatus _attiRightShow;

    FactListModel _deviceList;

    QMap<int, int> _datalinker_summary;
    QMap<PWarnConfGrpK, PWarnConfGrpV> warn_ext;
    QMap<QString, std::shared_ptr<FlightDevice>> _device_set;
    QMap<QString, std::shared_ptr<FlightDevice>> _moduelPage_set;
};
