#include "ConfigControllerExt.h"
#include "GCSMapEngine.h"
#include "log_category.h"

void ConfigControllerExt::updateMapFactory(bool type) {
    GCSMapEngine *ist = GCSMapEngine::instance();
    QStringList prov = ist->getMapNameList();
    QString name = type ? prov[0] : prov[1];

    COMMON_INFO("mapfactory {0}", name.toStdString());

    if (type) {
        ist->resetMapType(true);
    } else {
        ist->resetMapType(false);
    }

    setMapProvider(name);
}

void ConfigControllerExt::afterReadConfig(QSettings& settings) {

    _verticalMode = settings.value("JoyStick/verticalMode").toBool();
    _longitudeMode = settings.value("JoyStick/longitudeMode").toBool();
    _latitudeMode = settings.value("JoyStick/latitudeMode").toBool();

    _hardwareQuickConf.bautrate(settings.value("HardwareQuick/bautrate").toInt());
    _hardwareQuickConf.stopbit(settings.value("HardwareQuick/stopbit").toInt());
    _hardwareQuickConf.parity(settings.value("HardwareQuick/parity").toInt());
    _hardwareQuickConf.enable(settings.value("HardwareQuick/enable").toBool());
    _hardwareQuickConf.databit(settings.value("HardwareQuick/databit").toInt());
    _hardwareQuickConf.timeout(settings.value("HardwareQuick/timeout").toInt());
    _hardwareQuickConf.commName(settings.value("HardwareQuick/commName").toString());

    _hardwareSwitchConf.bautrate(settings.value("HardwareSwitch/bautrate").toInt());
    _hardwareSwitchConf.stopbit(settings.value("HardwareSwitch/stopbit").toInt());
    _hardwareSwitchConf.parity(settings.value("HardwareSwitch/parity").toInt());
    _hardwareSwitchConf.enable(settings.value("HardwareSwitch/enable").toBool());
    _hardwareSwitchConf.databit(settings.value("HardwareSwitch/databit").toInt());
    _hardwareSwitchConf.timeout(settings.value("HardwareSwitch/timeout").toInt());
    _hardwareSwitchConf.commName(settings.value("HardwareSwitch/commName").toString());
}
