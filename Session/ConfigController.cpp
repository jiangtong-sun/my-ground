#include "ConfigController.h"
#include "log_category.h"
#include "prototelemetry.h"
#include "stringtranslate.h"
#include <QFileInfo>
#include <QGuiApplication>
#include <QNetworkInterface>
#include <QScreen>
#include <QSettings>
#include <QTextCodec>

ConfigController::ConfigController() {
    _flyView = true;
    _update_serial = true;
    _update_network = true;
    _update_softcomm = true;
    _gcsPosition = QGeoCoordinate(22.461, 113.8919, 213.2);
    _mapPosition = QGeoCoordinate(22.461, 113.8919, 213.2);
}

ConfigController::~ConfigController() {}

void ConfigController::releaseCache() {}

QVariantMap &ConfigController::loadTTFDefine() {
    static std::once_flag flag;
    static QVariantMap ttfIcons;
    std::call_once(flag, []() {
        ttfIcons.insert("add", QStringLiteral("\ue664"));
        ttfIcons.insert("addCircle", QStringLiteral("\ue665"));
        ttfIcons.insert("browse", QStringLiteral("\ue666"));
        ttfIcons.insert("arrowDoubleLeft", QStringLiteral("\ue667"));
        ttfIcons.insert("arrowLeftCircle", QStringLiteral("\ue668"));
        ttfIcons.insert("arrowRightCircle", QStringLiteral("\ue669"));
        ttfIcons.insert("caps-lock", QStringLiteral("\ue66a"));
        ttfIcons.insert("complete", QStringLiteral("\ue66b"));
        ttfIcons.insert("copy", QStringLiteral("\ue66c"));
        ttfIcons.insert("dataView", QStringLiteral("\ue66d"));
        ttfIcons.insert("download", QStringLiteral("\ue66e"));
        ttfIcons.insert("exports", QStringLiteral("\ue66f"));
        ttfIcons.insert("edit", QStringLiteral("\ue670"));
        ttfIcons.insert("error", QStringLiteral("\ue671"));
        ttfIcons.insert("favorite", QStringLiteral("\ue672"));
        ttfIcons.insert("fileDelete", QStringLiteral("\ue673"));
        ttfIcons.insert("hide", QStringLiteral("\ue674"));
        ttfIcons.insert("home", QStringLiteral("\ue675"));
        ttfIcons.insert("history", QStringLiteral("\ue676"));
        ttfIcons.insert("fileOpen", QStringLiteral("\ue677"));
        ttfIcons.insert("import", QStringLiteral("\ue678"));
        ttfIcons.insert("screenShrink", QStringLiteral("\ue679"));
        ttfIcons.insert("lock", QStringLiteral("\ue67a"));
        ttfIcons.insert("screenExpand", QStringLiteral("\ue67b"));
        ttfIcons.insert("map", QStringLiteral("\ue67c"));
        ttfIcons.insert("modular", QStringLiteral("\ue67d"));
        ttfIcons.insert("notification", QStringLiteral("\ue67e"));
        ttfIcons.insert("play", QStringLiteral("\ue67f"));
        ttfIcons.insert("navigation", QStringLiteral("\ue680"));
        ttfIcons.insert("move", QStringLiteral("\ue681"));
        ttfIcons.insert("refresh", QStringLiteral("\ue682"));
        ttfIcons.insert("pin", QStringLiteral("\ue683"));
        ttfIcons.insert("save", QStringLiteral("\ue684"));
        ttfIcons.insert("share", QStringLiteral("\ue685"));
        ttfIcons.insert("stop", QStringLiteral("\ue686"));
        ttfIcons.insert("success", QStringLiteral("\ue687"));
        ttfIcons.insert("switch", QStringLiteral("\ue688"));
        ttfIcons.insert("setting", QStringLiteral("\ue689"));
        ttfIcons.insert("unlock", QStringLiteral("\ue68a"));
        ttfIcons.insert("upload", QStringLiteral("\ue68b"));
        ttfIcons.insert("warning", QStringLiteral("\ue68c"));
        ttfIcons.insert("zoomIn", QStringLiteral("\ue68d"));
        ttfIcons.insert("zoomOut", QStringLiteral("\ue68e"));
        ttfIcons.insert("closeBold", QStringLiteral("\ue68f"));
        ttfIcons.insert("selectBold", QStringLiteral("\ue690"));
        ttfIcons.insert("arrowUpFilling", QStringLiteral("\ue691"));
        ttfIcons.insert("arrowDownFilling", QStringLiteral("\ue692"));
        ttfIcons.insert("arrowLeftFilling", QStringLiteral("\ue693"));
        ttfIcons.insert("arrowRightFilling", QStringLiteral("\ue694"));
        ttfIcons.insert("square", QStringLiteral("\ue869"));
    });

    return ttfIcons;
}

bool ConfigController::reloadDisplayConfig(const QString &file) {
    if (file.isEmpty()) {
        return false;
    }

    _displayConfig = file;
    return true;
}

void ConfigController::resetMapFactory(bool type) { updateMapFactory(type); }

void ConfigController::saveAppResource() {
    COMMON_INFO("save resource....");
    _exit_flag = true;
}

void ConfigController::loadLocalIP() {
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for (int i = 0; i < list.count(); i++) {
        if (!list[i].isLoopback()) {
            if (list[i].protocol() == QAbstractSocket::IPv4Protocol) {
                _local_ips.append(list[i].toString());
            }
        }
    }
}

void ConfigController::updateRunTime() { emit appRunTimeChanged(); }

QString ConfigController::appRunTime() {
    if (_appRunTime == 0) {
        _appRunTime = currentTimeMs();
    }

    int runtime = (currentTimeMs() - _appRunTime) / 1000;
    char buffer[16] = {0};
    int hour = runtime / 3600;
    runtime = runtime % 3600;
    std::snprintf(buffer, 16, "%02d:%02d:%02d", hour, runtime / 60, runtime % 60);
    return QString(buffer);
}

void ConfigController::checkingLocalIP() {
    if (_local_ips.size() <= 0) {
        return;
    }

    // bool getmain = false;
    // bool getslave = false;
    // bool gettrans = false;
    // for (auto mx : _local_ips) {
    //     if (_main_net_config.netIp() == mx) {
    //         getmain = true;
    //     }
    //     if (_slave_net_config.netIp() == mx) {
    //         getslave = true;
    //     }
    //     if (_data_trans_config.netIp() == mx) {
    //         gettrans = true;
    //     }
    // }

    // if (!getmain) {
    //     _main_net_config.netIp(_local_ips[0]);
    // }
    // if (!getmain) {
    //     _slave_net_config.netIp(_local_ips[0]);
    // }
    // if (!gettrans) {
    //     _data_trans_config.netIp(_local_ips[0]);
    // }
}

bool ConfigController::reloadNetComConfig(const QString &fileName) {
    if (!QFileInfo::exists(fileName)) {
        COMMON_WARN("check {0} is exist or not", fileName.toStdString().c_str());
        return false;
    }

    QSettings settings(fileName, QSettings::IniFormat);
    _main_net_config.multiMode(settings.value("MainNetwork/multiMode").toBool());
    _main_net_config.localLoop(settings.value("MainNetwork/localLoop").toBool());
    _main_net_config.enable(settings.value("MainNetwork/enable").toBool());
    _main_net_config.netIp(settings.value("MainNetwork/netIp").toString());
    _main_net_config.multiTxPort(settings.value("MainNetwork/multiTxPort").toInt());
    _main_net_config.multiTxIp(settings.value("MainNetwork/multiTxIp").toString());
    _main_net_config.multiRxPort(settings.value("MainNetwork/multiRxPort").toInt());
    _main_net_config.multiRxIp(settings.value("MainNetwork/multiRxIp").toString());

    _slave_net_config.multiMode(settings.value("SlaveNetwork/multiMode").toBool());
    _slave_net_config.localLoop(settings.value("SlaveNetwork/localLoop").toBool());
    _slave_net_config.enable(settings.value("SlaveNetwork/enable").toBool());
    _slave_net_config.netIp(settings.value("SlaveNetwork/netIp").toString());
    _slave_net_config.multiTxPort(settings.value("SlaveNetwork/multiTxPort").toInt());
    _slave_net_config.multiTxIp(settings.value("SlaveNetwork/multiTxIp").toString());
    _slave_net_config.multiRxPort(settings.value("SlaveNetwork/multiRxPort").toInt());
    _slave_net_config.multiRxIp(settings.value("SlaveNetwork/multiRxIp").toString());

    _data_trans_config.multiMode(settings.value("DataTransfer/multiMode").toBool());
    _data_trans_config.localLoop(settings.value("DataTransfer/localLoop").toBool());
    _data_trans_config.enable(settings.value("DataTransfer/enable").toBool());
    _data_trans_config.netIp(settings.value("DataTransfer/netIp").toString());
    _data_trans_config.netPort(settings.value("DataTransfer/netPort").toInt());
    _data_trans_config.tcpMode(settings.value("DataTransfer/tcpMode").toBool());
    _data_trans_config.multiTxPort(settings.value("DataTransfer/multiTxPort").toInt());
    _data_trans_config.multiTxIp(settings.value("DataTransfer/multiTxIp").toString());
    _data_trans_config.multiRxPort(settings.value("DataTransfer/multiRxPort").toInt());
    _data_trans_config.multiRxIp(settings.value("DataTransfer/multiRxIp").toString());
    _data_trans_config.biglittle(settings.value("DataTransfer/bigLittle").toBool());
    _data_trans_config.copyMode(settings.value("DataTransfer/copyMode").toBool());
    _data_trans_config.mockData(settings.value("DataTransfer/mockData").toBool());

    _comNodeConfig.bautrate(settings.value("MainSerial/bautrate").toInt());
    _comNodeConfig.stopbit(settings.value("MainSerial/stopbit").toInt());
    _comNodeConfig.parity(settings.value("MainSerial/parity").toInt());
    _comNodeConfig.enable(settings.value("MainSerial/enable").toBool());
    _comNodeConfig.databit(settings.value("MainSerial/databit").toInt());
    _comNodeConfig.timeout(settings.value("MainSerial/timeout").toInt());
    _comNodeConfig.commName(settings.value("MainSerial/commName").toString());

    ProtoDecoderInfo &protoinfo = GetProtoDecoderInfo();

    protoinfo.VehicleLabelAddr = settings.value("Telemetry/VehicleLabelAddr").toInt();
    protoinfo.OtherProtoTypeOffset = settings.value("Telemetry/OtherProtoTypeOffset").toInt();
    protoinfo.FirstProtoTypeOffset = settings.value("Telemetry/FirstProtoTypeOffset").toInt();
    protoinfo.ProtoSinglePackage = settings.value("Telemetry/ProtoSinglePackage").toInt();
    protoinfo.ProtoPackageLength = settings.value("Telemetry/ProtoPackageLength").toInt();
    protoinfo.ProtoTimestampOffset = settings.value("Telemetry/ProtoTimestampOffset").toInt();
    protoinfo.DataBigLittleEndian = settings.value("Telemetry/DataBigLittleEndian").toInt();

    QVariant vmm = settings.value("Telemetry/ProtoTimestampName");
    protoinfo.ProtoTimestampName = vmm.isValid() ? vmm.toString() : "";

    if (settings.value("Telemetry/VehicleFeedbackFrame").isValid()) {
        QString val = settings.value("Telemetry/VehicleFeedbackFrame").toString();
        if (val.contains("0x")) {
            protoinfo.VehicleFeedbackFrame =
                settings.value("Telemetry/VehicleFeedbackFrame").toString().toInt(nullptr, 16);
        } else {
            protoinfo.VehicleFeedbackFrame = settings.value("Telemetry/VehicleFeedbackFrame").toInt();
        }
    }

    if (settings.value("Telemetry/AdjustCommandType").isValid()) {
        QString val = settings.value("Telemetry/AdjustCommandType").toString();
        if (val.contains("0x")) {
            protoinfo.AdjustCommandType = settings.value("Telemetry/AdjustCommandType").toString().toInt(nullptr, 16);
        } else {
            protoinfo.AdjustCommandType = settings.value("Telemetry/AdjustCommandType").toInt();
        }
    }

    if (settings.value("Telemetry/ControlCommandType").isValid()) {
        QString val = settings.value("Telemetry/ControlCommandType").toString();
        if (val.contains("0x")) {
            protoinfo.ControlCommandType = settings.value("Telemetry/ControlCommandType").toString().toInt(nullptr, 16);
        } else {
            protoinfo.ControlCommandType = settings.value("Telemetry/ControlCommandType").toInt();
        }
    }

    if (settings.value("Telemetry/AdjustControlType").isValid()) {
        QString val = settings.value("Telemetry/AdjustControlType").toString();
        if (val.contains("0x")) {
            protoinfo.AdjustControlType = settings.value("Telemetry/AdjustControlType").toString().toInt(nullptr, 16);
        } else {
            protoinfo.AdjustControlType = settings.value("Telemetry/AdjustControlType").toInt();
        }
    }

    if (settings.value("Telemetry/KeepaliveCommand").isValid()) {
        QString val = settings.value("Telemetry/KeepaliveCommand").toString();
        if (val.contains("0x")) {
            protoinfo.keepaliveCommand = settings.value("Telemetry/KeepaliveCommand").toString().toInt(nullptr, 16);
        } else {
            protoinfo.keepaliveCommand = settings.value("Telemetry/KeepaliveCommand").toInt();
        }
    }

    if (settings.value("Telemetry/RTKTransferFrame").isValid()) {
        QString val = settings.value("Telemetry/RTKTransferFrame").toString();
        if (val.contains("0x")) {
            protoinfo.RTKTransferFrame = settings.value("Telemetry/RTKTransferFrame").toString().toInt(nullptr, 16);
        } else {
            protoinfo.RTKTransferFrame = settings.value("Telemetry/RTKTransferFrame").toInt();
        }
    }

    if (settings.value("Telemetry/MissionOperateFrame").isValid()) {
        QString val = settings.value("Telemetry/MissionOperateFrame").toString();
        if (val.contains("0x")) {
            protoinfo.MissionOptFrame = settings.value("Telemetry/MissionOperateFrame").toString().toInt(nullptr, 16);
        } else {
            protoinfo.MissionOptFrame = settings.value("Telemetry/MissionOperateFrame").toInt();
        }
    }

    if (settings.value("Telemetry/VehiclePositionFrame").isValid()) {
        QString val = settings.value("Telemetry/VehiclePositionFrame").toString();
        if (val.contains("0x")) {
            protoinfo.VehiclePositionFrame =
                settings.value("Telemetry/VehiclePositionFrame").toString().toInt(nullptr, 16);
        } else {
            protoinfo.VehiclePositionFrame = settings.value("Telemetry/VehiclePositionFrame").toInt();
        }
    }

    FeedbackPositionInfo &info = GetFeedbackPositionInfo();
    info.ControlFeedbackPos = settings.value("Telemetry/ControlFeedbackPos").toInt();
    info.AdjustFeedbackPos = settings.value("Telemetry/AdjustFeedbackPos").toInt();
    info.VerifyFeedbackSeq = settings.value("Telemetry/VerifyFeedbackSeq").toInt();
    info.ProtoPositionLng = settings.value("Telemetry/ProtoPositionLng").toInt();
    info.ProtoPositionLat = settings.value("Telemetry/ProtoPositionLat").toInt();
    info.ProtoPositionAlt = settings.value("Telemetry/ProtoPositionAlt").toInt();
    info.ProtoPositionHead = settings.value("Telemetry/ProtoPositionHead").toInt();

    _isControlRole = (settings.value("Commons/sysControlRole").toBool());
    _dataUpdateFreq = (settings.value("Commons/dataUpdateFreq").toInt());
    _keepaliveFreq = (settings.value("Commons/keepaliveFreq").toInt());
    _dataTransFreq = (settings.value("Commons/dataTransFreq").toInt());
    _videoTestFile = (settings.value("Commons/videoTestFile").toString());
    _videoTestFile = (settings.value("Commons/videoTestFile").toString());
    _isBigEndian = (settings.value("Commons/bigLittleEndian").toBool());
    _autoTestMode = (settings.value("Commons/autoTestMode").toBool());

    /// 飞机HUD参考地面标记符号
    if (settings.value("Commons/hudReferGround").toBool()) {
        _hudReferLevel = -1;
    } else {
        _hudReferLevel = 1;
    }

    /// window stay on top
    if (settings.contains("Commons/staysOnTopHint")) {
        if (settings.value("Commons/staysOnTopHint").toBool()) {
            _staysOnTopHint = 0x00040000;
        } else {
            _staysOnTopHint = 0;
        }
    }

    ///舵面精度
    if (settings.contains("Commons/rundderElevatorFlatRes")) {
        _rundderElevatorFlatRes = settings.value("Commons/rundderElevatorFlatRes").toFloat();
    }

    _minErgencyDis = (settings.value("Commons/minErgencyDis").toInt());
    _videoRWTimeout = (settings.value("Commons/videoRWTimeout").toInt());
    _videoRWTimeout = qBound(1000, _videoRWTimeout, 10000);
    _minErgencyDis = qBound(200, _minErgencyDis, 10000);

    /// 额外消息发送
    _transExtentRC = (settings.value("Commons/transExtentRC").toBool());
    QString val2 = settings.value("Commons/transExtentMark").toString();
    if (val2.contains("0x")) {
        _transExtMark = val2.toInt(nullptr, 16);
    } else {
        _transExtMark = val2.toInt();
    }

    if (settings.contains("GroundStation/longitude")) {
        _gcsPosition.setLongitude(settings.value("GroundStation/longitude").toDouble());
        _gcsPosition.setLatitude(settings.value("GroundStation/latitude").toDouble());
        _mapPosition.setLongitude(settings.value("GroundStation/longitude").toDouble());
        _mapPosition.setLatitude(settings.value("GroundStation/latitude").toDouble());
    }

    if (!settings.value("Commons/deftChannel").toString().isEmpty()) {
        _softCommConf.deftChannel(settings.value("Commons/deftChannel").toString());
    }
    if (!settings.value("Commons/controlChannel").toString().isEmpty()) {
        _softCommConf.controlChannel(settings.value("Commons/controlChannel").toString());
    }
    if (!settings.value("Commons/taskChannel").toString().isEmpty()) {
        _softCommConf.taskChannel(settings.value("Commons/taskChannel").toString());
    }

    _hud_locate.screenId(settings.value("HudWindow/screenId").toInt());
    _hud_locate.width(settings.value("HudWindow/width").toInt());
    _hud_locate.height(settings.value("HudWindow/height").toInt());
    _hud_locate.xpos(settings.value("HudWindow/xpos").toInt());
    _hud_locate.ypos(settings.value("HudWindow/ypos").toInt());

    _fmc_locate.screenId(settings.value("FmcWindow/screenId").toInt());
    _fmc_locate.width(settings.value("FmcWindow/width").toInt());
    _fmc_locate.height(settings.value("FmcWindow/height").toInt());
    _fmc_locate.xpos(settings.value("FmcWindow/xpos").toInt());
    _fmc_locate.ypos(settings.value("FmcWindow/ypos").toInt());

    _fmp_locate.screenId(settings.value("FmpWindow/screenId").toInt());
    _fmp_locate.width(settings.value("FmpWindow/width").toInt());
    _fmp_locate.height(settings.value("FmpWindow/height").toInt());
    _fmp_locate.xpos(settings.value("FmpWindow/xpos").toInt());
    _fmp_locate.ypos(settings.value("FmpWindow/ypos").toInt());

    afterReadConfig(settings);
    return true;
}

void ConfigController::writeNetComConfig() {
    QString fileName = "networkserail.ini";
    QSettings settings(fileName, QSettings::IniFormat);
    settings.setValue("MainNetwork/multiMode", _main_net_config.multiMode());
    settings.setValue("MainNetwork/localLoop", _main_net_config.localLoop());
    settings.setValue("MainNetwork/enable", _main_net_config.enable());
    settings.setValue("MainNetwork/netIp", _main_net_config.netIp());
    settings.setValue("MainNetwork/multiTxPort", _main_net_config.multiTxPort());
    settings.setValue("MainNetwork/multiTxIp", _main_net_config.multiTxIp());
    settings.setValue("MainNetwork/multiRxPort", _main_net_config.multiRxPort());
    settings.setValue("MainNetwork/multiRxIp", _main_net_config.multiRxIp());
    settings.setValue("MainNetwork/dataTransFreq", _main_net_config.dataTransFreq());

    settings.setValue("SlaveNetwork/multiMode", _slave_net_config.multiMode());
    settings.setValue("SlaveNetwork/dataTransFreq", _slave_net_config.dataTransFreq());
    settings.setValue("SlaveNetwork/localLoop", _slave_net_config.localLoop());
    settings.setValue("SlaveNetwork/enable", _slave_net_config.enable());
    settings.setValue("SlaveNetwork/netIp", _slave_net_config.netIp());
    settings.setValue("SlaveNetwork/multiTxPort", _slave_net_config.multiTxPort());
    settings.setValue("SlaveNetwork/multiTxIp", _slave_net_config.multiTxIp());
    settings.setValue("SlaveNetwork/multiRxPort", _slave_net_config.multiRxPort());
    settings.setValue("SlaveNetwork/multiRxIp", _slave_net_config.multiRxIp());

    settings.setValue("DataTransfer/dataTransFreq", _data_trans_config.dataTransFreq());
    settings.setValue("DataTransfer/multiMode", _data_trans_config.multiMode());
    settings.setValue("DataTransfer/localLoop", _data_trans_config.localLoop());
    settings.setValue("DataTransfer/enable", _data_trans_config.enable());
    settings.setValue("DataTransfer/netIp", _data_trans_config.netIp());
    settings.setValue("DataTransfer/multiTxPort", _data_trans_config.multiTxPort());
    settings.setValue("DataTransfer/multiTxIp", _data_trans_config.multiTxIp());
    settings.setValue("DataTransfer/multiRxPort", _data_trans_config.multiRxPort());
    settings.setValue("DataTransfer/multiRxIp", _data_trans_config.multiRxIp());

    settings.setValue("MainSerial/bautrate", _comNodeConfig.bautrate());
    settings.setValue("MainSerial/timeout", _comNodeConfig.timeout());
    settings.setValue("MainSerial/stopbit", _comNodeConfig.stopbit());
    settings.setValue("MainSerial/enable", _comNodeConfig.enable());
    settings.setValue("MainSerial/databit", _comNodeConfig.databit());
    settings.setValue("MainSerial/commName", _comNodeConfig.commName());
}
