#pragma once

#include "ConfigNetCom.h"
#include <QGeoCoordinate>
#include <QSettings>

class ConfigController : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(ConfigController)

  public:
    ConfigController();
    ~ConfigController();

    Q_PROPERTY(bool flyView READ flyView WRITE setFlyView NOTIFY flyViewChanged)
    Q_PROPERTY(AppLocateConfig *hudLocate READ hudLocate CONSTANT)
    Q_PROPERTY(AppLocateConfig *fmpLocate READ fmpLocate CONSTANT)
    Q_PROPERTY(AppLocateConfig *fmcLocate READ fmcLocate CONSTANT)

    Q_PROPERTY(int dataUpdateFreq READ dataUpdateFreq CONSTANT)
    Q_PROPERTY(int keepaliveFreq READ keepaliveFreq CONSTANT)
    Q_PROPERTY(int dataTransFreq READ dataTransFreq CONSTANT)
    Q_PROPERTY(QString videoTestFile READ videoTestFile CONSTANT)

    Q_PROPERTY(int zOrderTopMost READ zOrderTopMost CONSTANT)
    Q_PROPERTY(int zOrderVehicles READ zOrderVehicles CONSTANT)
    Q_PROPERTY(int zOrderMapItems READ zOrderMapItems CONSTANT)
    Q_PROPERTY(int zOrderTrajectory READ zOrderTrajectory CONSTANT)
    Q_PROPERTY(int zOrderWaypointLines READ zOrderWaypointLines CONSTANT)
    Q_PROPERTY(int zOrderBaseMap READ zOrderBaseMap CONSTANT)
    Q_PROPERTY(bool updateSerial READ updateSerial CONSTANT)
    Q_PROPERTY(bool updateNetwork READ updateNetwork CONSTANT)
    Q_PROPERTY(bool updateSoftComm READ updateSoftComm CONSTANT)
    Q_PROPERTY(bool autoTestMode READ autoTestMode CONSTANT)
    Q_PROPERTY(int videoRWTimeout READ videoRWTimeout CONSTANT)
    Q_PROPERTY(int hudReferLevel READ hudReferLevel CONSTANT)
    Q_PROPERTY(int staysOnTopHint READ staysOnTopHint CONSTANT)

    Q_PROPERTY(double rundderElevatorFlatRes READ rundderElevatorFlatRes CONSTANT)

    Q_PROPERTY(int aircraftMinSinkRate READ aircraftMinSinkRate CONSTANT)
    Q_PROPERTY(int aircraftMaxSinkRate READ aircraftMaxSinkRate CONSTANT)

    Q_PROPERTY(QString mapProvider READ mapProvider WRITE setMapProvider NOTIFY mapProviderChanged)
    Q_PROPERTY(QGeoCoordinate mapCenter READ getMapPos WRITE setMapPos NOTIFY mapPositionChanged)
    Q_PROPERTY(QGeoCoordinate gcsCenter READ getGcsPos WRITE setGcsPos NOTIFY gcsPositionChanged)
    Q_PROPERTY(SerialNodeConfig *serailNode READ serailNode NOTIFY serailNodeChanged)
    Q_PROPERTY(SoftwareCommConfig *softComm READ softComm NOTIFY softCommChanged)
    Q_PROPERTY(NetworkNodeConfig *networkNode READ mainNetNode NOTIFY mainNetNodeChanged)
    Q_PROPERTY(NetworkNodeConfig *slaveNetNode READ slaveNetNode NOTIFY slaveNetNodeChanged)
    Q_PROPERTY(NetworkNodeConfig *dataTransfer READ dataTransfer NOTIFY dataTransferChanged)
    Q_PROPERTY(QString appRunTime READ appRunTime NOTIFY appRunTimeChanged)
    Q_PROPERTY(FlightLanding* landConfig READ landConfig CONSTANT)

    QGeoCoordinate getMapPos(void) { return _mapPosition; }
    QGeoCoordinate getGcsPos(void) { return _gcsPosition; }
    QString mapProvider(void) { return _mapProvider; }

    SoftwareCommConfig *softComm() { return &_softCommConf; }
    SerialNodeConfig *serailNode() { return &_comNodeConfig; }
    NetworkNodeConfig *mainNetNode() { return &_main_net_config; }
    NetworkNodeConfig *slaveNetNode() { return &_slave_net_config; }
    NetworkNodeConfig *dataTransfer() { return &_data_trans_config; }
    FlightLanding* landConfig() { return &_land_config; }

    bool reloadDisplayConfig(const QString &file);
    QString displayConfig() { return _displayConfig; }
    QString videoTestFile() { return _videoTestFile; }

    int videoRWTimeout() { return _videoRWTimeout; }
    int hudReferLevel() { return _hudReferLevel; }

    int staysOnTopHint() const { return _staysOnTopHint; }

    int aircraftMinSinkRate() { return _aircraftMinSinkRate; }
    int aircraftMaxSinkRate() { return _aircraftMaxSinkRate; }

    virtual void updateMapFactory(bool) { ; }
    virtual void afterReadConfig(QSettings &) { ; }

    double rundderElevatorFlatRes() { return _rundderElevatorFlatRes; }

    bool reloadNetComConfig(const QString &);
    void writeNetComConfig();
    void checkingLocalIP();

    QString appRunTime();
    void updateRunTime();

    ///
    /// \brief resetMapFactory
    ///
    Q_INVOKABLE void resetMapFactory(bool);

    ///
    /// \brief saveAppResource
    ///
    Q_INVOKABLE void saveAppResource();

    ///
    /// \brief exitFlag
    /// \return
    ///
    Q_INVOKABLE bool exitFlag() { return _exit_flag; }

    ///
    /// \brief getSysConfigRole
    /// \return
    ///
    bool getSysConfigRole() { return _isControlRole; }

    static QVariantMap &loadTTFDefine();

    void setMapProvider(const QString &d) {
        _mapProvider = d;
        emit mapProviderChanged(d);
    }

    void setMapPos(QGeoCoordinate g) {
        _mapPosition = g;
        emit mapPositionChanged();
    }

    void setGcsPos(QGeoCoordinate g) {
        _gcsPosition = g;
        emit gcsPositionChanged();
    }

    void resetSerial() { _update_serial = true; }
    void resetSoftComm() { _update_softcomm = true; }

    void setFlyView(bool v) {
        _flyView = v;
        emit flyViewChanged();
    }

    bool updateSerial() {
        bool rtn = _update_serial;
        _update_serial = false;
        return rtn;
    }

    bool updateNetwork() {
        bool rtn = _update_network;
        _update_network = false;
        return rtn;
    }

    bool updateSoftComm() {
        bool rtn = _update_softcomm;
        _update_softcomm = false;
        return rtn;
    }

    bool autoTestMode() { return _autoTestMode; }

    void releaseCache();
    int zOrderTopMost() { return 1000; }
    int zOrderVehicles() { return 100; }
    int zOrderMapItems() { return 50; }
    int zOrderWaypointLines() { return 30; }
    int zOrderTrajectory() { return 40; }
    int zOrderBaseMap() { return 20; }

    bool flyView() { return _flyView; }

    uint32_t transExtMark() { return _transExtMark; }

    bool transExtentMode() { return _transExtentRC; }

    AppLocateConfig *hudLocate() { return &_hud_locate; }
    AppLocateConfig *fmpLocate() { return &_fmp_locate; }
    AppLocateConfig *fmcLocate() { return &_fmc_locate; }

    int dataUpdateFreq() { return _dataUpdateFreq; }
    int keepaliveFreq() { return _keepaliveFreq; }
    int dataTransFreq() { return _dataTransFreq; }
    int deviceDataFreq() { return _devDataFreqUpt;}
    int bigLittleEndian() { return _isBigEndian; }
    int minErgencyDist() { return _minErgencyDis; }
    void loadLocalIP();

  signals:
    void softCommChanged();
    void appRunTimeChanged();
    void mapPositionChanged();
    void gcsPositionChanged();
    void telemetryConfigChanged();
    void serailNodeChanged();
    void mainNetNodeChanged();
    void slaveNetNodeChanged();
    void dataTransferChanged();
    void mapProviderChanged(QString);
    void flyViewChanged();
    void releaseQmlCache();

  protected:
    bool _flyView;
    int _hudReferLevel = -1;
    int _videoRWTimeout = 3000;
    int _dataUpdateFreq = 1;
    int _keepaliveFreq = 1;
    int _dataTransFreq = 1;
    int _devDataFreqUpt = 10;
    int _minErgencyDis = 1000;
    int _staysOnTopHint = 0x00040000;
    double _rundderElevatorFlatRes = 0.5f;
    bool _autoTestMode = false;
    bool _isBigEndian = 1;
    bool _exit_flag = false;
    bool _isControlRole = true;
    bool _transExtentRC = false;
    int _aircraftMinSinkRate = 3;
    int _aircraftMaxSinkRate = 6;
    uint32_t _transExtMark = 0;
    qint64 _appRunTime = 0;
    QStringList _local_ips;
    QString _mapProvider = "";
    QString _displayConfig = "";
    QString _videoTestFile = "";
    std::atomic_bool _update_serial;
    std::atomic_bool _update_softcomm;
    std::atomic_bool _update_network;
    AppLocateConfig _hud_locate;
    AppLocateConfig _fmp_locate;
    AppLocateConfig _fmc_locate;
    FlightLanding _land_config;
    FlightLandCurve _landcurve_conf;
    SerialNodeConfig _comNodeConfig;
    NetworkNodeConfig _main_net_config;
    NetworkNodeConfig _slave_net_config;
    NetworkNodeConfig _data_trans_config;
    SoftwareCommConfig _softCommConf;
    QGeoCoordinate _mapPosition;
    QGeoCoordinate _gcsPosition;
};
