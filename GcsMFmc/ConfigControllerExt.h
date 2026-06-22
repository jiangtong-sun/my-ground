#pragma once

#include "ConfigController.h"

class ConfigControllerExt : public ConfigController {
    Q_OBJECT
    PROPERTY_READWRITE(bool, verticalMode )
    PROPERTY_READWRITE(bool, longitudeMode)
    PROPERTY_READWRITE(bool, latitudeMode )
    PROPERTY_READWRITE(int, deviceSelect)
    PROPERTY_READWRITE(int, deviceEnable)
    Q_PROPERTY(QString appVersion READ appVersion CONSTANT)

  public:
    ConfigControllerExt() = default;
    ~ConfigControllerExt() = default;

    QString appVersion() { return _appVersion; }
    void setAppVersion(const QString &v) { _appVersion = v; }
    virtual void afterReadConfig(QSettings &) override;
    void updateMapFactory(bool) override;

    SerialNodeConfig *hardwareQuickConf() { return &_hardwareQuickConf; }
    SerialNodeConfig *hardwareSwitchConf() { return &_hardwareSwitchConf; }

    QString pedalConfigFile() { return _pedalConfFile; }
    QString gearstickConfigFile() { return _gearstickConfFile; }
    QString acceleratorConfigFile() { return _acceleratorConfFile; }

  private:
    QString _appVersion;
    SerialNodeConfig _hardwareQuickConf;
    SerialNodeConfig _hardwareSwitchConf;
    QString _pedalConfFile = "devicepedal.ini"; //
    QString _gearstickConfFile = "devicegearstick.ini";//
    QString _acceleratorConfFile = "deviceaccelerator.ini";//
};
