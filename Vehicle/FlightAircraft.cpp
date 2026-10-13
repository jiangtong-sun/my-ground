#include "FlightAircraft.h"
#include "FlightWarnGroup.h"
#include "log_category.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxdocument.h"
#include "xlsxworkbook.h"

#define DisplayModulePage transToUnicode("ModulePage")
#define DisplayKeyStatus  transToUnicode("RCAssociated")
#define DisplayHudPage    transToUnicode("HudPage")

void FlightVechicle::registerType() {
    qmlRegisterUncreatableType<NavigationElemNode>("NavigationElemNode", 1, 0, "NavigationElemNode", "Reference");
    qmlRegisterUncreatableType<FeedbackDataNode>("FeedbackDataNode", 1, 0, "FeedbackDataNode", "Reference");
    qmlRegisterUncreatableType<DisplayElemNode>("DisplayElemNode", 1, 0, "DisplayElemNode", "Reference");
    qmlRegisterUncreatableType<FlightVechicle>("FlightVechicle", 1, 0, "FlightVechicle", "Reference");
    qmlRegisterUncreatableType<FlightDisplay>("FlightDisplay", 1, 0, "FlightDisplay", "Reference");
    qmlRegisterUncreatableType<FlightStatus>("FlightStatus", 1, 0, "FlightStatus", "Reference");
    qmlRegisterUncreatableType<FlightWarnGroup>("FlightWarnGroup", 1, 0, "FlightWarnGroup", "Reference");

    qmlRegisterUncreatableType<VehicleFeedback>("VehicleFeedback", 1, 0, "VehicleFeedback", "Reference only");
    qmlRegisterUncreatableType<AttributeTable>("AttributeTable", 1, 0, "AttributeTable", "Reference only");
    qmlRegisterUncreatableType<SimpleElemList>("SimpleElemList", 1, 0, "SimpleElemList", "Reference only");
    qmlRegisterUncreatableType<SimpleElemNode>("SimpleElemNode", 1, 0, "SimpleElemNode", "Reference only");
    qmlRegisterUncreatableType<BatteryElemNode>("BatteryElemNode", 1, 0, "BatteryElemNode", "Reference only");
    qmlRegisterUncreatableType<BusbarElemNode>("BusbarElemNode", 1, 0, "BusbarElemNode", "Reference only");
    qmlRegisterUncreatableType<EngineElemNode>("EngineElemNode", 1, 0, "EngineElemNode", "Reference only");
    qmlRegisterUncreatableType<BakeComponent>("BakeComponent", 1, 0, "BakeComponent", "Reference only");
    qmlRegisterUncreatableType<ElecSpdCtrlElemNode>("ElecSpdCtrlElemNode", 1, 0, "ElecSpdCtrlElemNode",
                                                    "Reference only");
    qmlRegisterUncreatableType<FlapRunnderNode>("FlapRunnderNode", 1, 0, "FlapRunnderNode", "Reference only");
}

FlightVechicle::FlightVechicle() : _name(""), _deviceIdx(-1), _vehicleIdx(0) {
    _position = QGeoCoordinate(37.758828, 108.854387, 213.2);
}

FlightVechicle::~FlightVechicle() {}

void FlightVechicle::clearFlightline() { _clr_fline = true; }

void FlightVechicle::setPosition(QGeoCoordinate pos) {
    _position = pos; //
    emit positionChanged(pos);
}

void FlightVechicle::uptVechiclePos(double lng, double lat, double alt) {
    if (lng > 180 || lng < -180 || lat > 90 || lat < -90 || alt < -5000) {
        return;
    }

    _position.setAltitude(alt);
    _position.setLatitude(lat);
    _position.setLongitude(lng);

    /// 航点位置不一致的时候
    if (abs(lastlatitude - lat) >= 0.000001 || abs(lastlongitude - lng) >= 0.000001) {
        lastlatitude = lat;
        lastlongitude = lng;
        emit trajectoryChanged(_position);
    }

    emit positionChanged(_position);
}

FlightDisplay *FlightVechicle::searchMeta(const QString &name) const {
    FlightStatus *sel = nullptr;
    auto gx = _moduelPage_set.find(DisplayHudPage);
    if (gx != _moduelPage_set.end()) {
        sel = gx.value()->find(name);
    }

    if (sel != nullptr) {
        return sel->first(name);
    } else {
        COMMON_INFO("not found {} model", name.toStdString().c_str());
    }

    return nullptr;
}

FlightDevice *FlightVechicle::searchMode(const QString &mode) const {
    FlightDevice *sel = nullptr;
    auto gx = _moduelPage_set.find(mode);
    if (gx != _moduelPage_set.end()) {
        sel = gx.value().get();
    } else {
        COMMON_INFO("not found {} model", mode.toStdString().c_str());
    }

    return sel;
}

FlightDisplay *FlightVechicle::getDeviceByName(const QString &inname) {
    FlightStatus *leftShow = attiLeftShow();
    if (leftShow == nullptr) {
        return nullptr;
    }

    FactListModel *firstPart = leftShow->firstPart();
    if (firstPart == nullptr) {
        return nullptr;
    }

    for (int mx = 0; mx < firstPart->count(); mx++) {
        FlightDisplay *disp = firstPart->value<FlightDisplay *>(mx);
        if (disp != nullptr && disp->itemName().contains(inname)) {
            return disp;
        }
    }

    return nullptr;
}

void FlightVechicle::setRcGroupName(QString val) {
    _rcGrpName = val;

    FlightDevice *rtn = searchMode(DisplayKeyStatus);
    if (rtn != nullptr) {
        _rcAssociated = rtn->find(val);
        emit rcAssociatedChanged();
    }
}

FlightStatus *FlightVechicle::rcAssociated() { return _rcAssociated; }
FlightStatus *FlightVechicle::attiLeftShow() { return &_attiLeftShow; }
FlightStatus *FlightVechicle::attiCenterShow() { return &_attiCenterShow; }
FlightStatus *FlightVechicle::attiRightShow() { return &_attiRightShow; }
FlightStatus *FlightVechicle::mainNavBlock() { return &_mainNavBlock; }
FlightStatus *FlightVechicle::mainCommBlock() { return &_mainCommBlock; }
FlightStatus *FlightVechicle::navKeyPage() { return &_navKeyPage; }
FlightStatus *FlightVechicle::taskKeyPage() { return &_taskKeyPage; }
FlightStatus *FlightVechicle::flightCommonA() { return &_flightCommonA; }
FlightStatus *FlightVechicle::flightCommonB() { return &_flightCommonB; }

FlightDisplay *FlightVechicle::HeightSource() { return searchMeta("HeightSource"); }
FlightDisplay *FlightVechicle::leftWheelState() { return searchMeta("LeftWheelState"); }
FlightDisplay *FlightVechicle::rightWheelState() { return searchMeta("RightWheelState"); }
FlightDisplay *FlightVechicle::frontWheelState() { return searchMeta("FrontWheelState"); }
FlightDisplay *FlightVechicle::CurrentNav() { return searchMeta("CurrentNav"); }
FlightDisplay *FlightVechicle::AirOrGround() { return searchMeta("AirOrGround"); }
FlightDisplay *FlightVechicle::PlaneAddress() { return searchMeta("PlaneAddress"); }
FlightDisplay *FlightVechicle::DeclineRatio() { return searchMeta("DeclineRatio"); }
FlightDisplay *FlightVechicle::NavigationMode() { return searchMeta("NavigationMode"); }
FlightDisplay *FlightVechicle::LateralMode() { return searchMeta("LateralMode"); }
FlightDisplay *FlightVechicle::LongitudinalMode() { return searchMeta("LongitudinalMode"); }
FlightDisplay *FlightVechicle::FrontWheelAngle() { return searchMeta("FrontWheelAngle"); }
FlightDisplay *FlightVechicle::LeftBrake() { return searchMeta("LeftBrake"); }
FlightDisplay *FlightVechicle::RightBrake() { return searchMeta("RightBrake"); }
FlightDisplay *FlightVechicle::DifferYaw() { return searchMeta("DifferYaw"); }
FlightDisplay *FlightVechicle::TargetYaw() { return searchMeta("TargetYaw"); }

FlightDisplay *FlightVechicle::LineWaypoint() { return searchMeta("LineWaypoint"); }
FlightDisplay *FlightVechicle::LineStage() { return searchMeta("LineStage"); }
FlightDisplay *FlightVechicle::WindSpeed() { return searchMeta("WindSpeed"); }
FlightDisplay *FlightVechicle::WindDirection() { return searchMeta("WindDirection"); }
FlightDisplay *FlightVechicle::LateralOffset() { return searchMeta("LateralOffset"); }
FlightDisplay *FlightVechicle::Attack() { return searchMeta("Attack"); }
FlightDisplay *FlightVechicle::SlideAngle() { return searchMeta("SlideAngle"); }
FlightDisplay *FlightVechicle::YawAngle() { return searchMeta("YawAngle"); }
FlightDisplay *FlightVechicle::PitchAngle() { return searchMeta("PitchAngle"); }
FlightDisplay *FlightVechicle::RollAngle() { return searchMeta("RollAngle"); }
FlightDisplay *FlightVechicle::Airspeed() { return searchMeta("Airspeed"); }
FlightDisplay *FlightVechicle::AirspeedSet() { return searchMeta("AirspeedSet"); }
FlightDisplay *FlightVechicle::HeightSet() { return searchMeta("HeightSet"); }
FlightDisplay *FlightVechicle::TrueAirspeed() { return searchMeta("TrueAirspeed"); }
FlightDisplay *FlightVechicle::FlightAngle() { return searchMeta("FlightAngle"); }
FlightDisplay *FlightVechicle::HeightOffset() { return searchMeta("HeightOffset"); }
FlightDisplay *FlightVechicle::GroundSpeed() { return searchMeta("GroundSpeed"); }
FlightDisplay *FlightVechicle::VerticalSpeed() { return searchMeta("VerticalSpeed"); }
FlightDisplay *FlightVechicle::IndicateSpeed() { return searchMeta("IndicateSpeed"); }
FlightDisplay *FlightVechicle::Elevation() { return searchMeta("Elevation"); }
FlightDisplay *FlightVechicle::RadioAltimeter() { return searchMeta("RadioAltimeter"); }
FlightDisplay *FlightVechicle::QNE() { return searchMeta("QNE"); }
FlightDisplay *FlightVechicle::QNH() { return searchMeta("QNH"); }
FlightDisplay *FlightVechicle::QFE() { return searchMeta("QFE"); }
FlightDisplay *FlightVechicle::DTG() { return searchMeta("DTG"); }
FlightDisplay *FlightVechicle::MLD() { return searchMeta("MLD"); }

AttributeTable *FlightVechicle::attitudeModel(void) { return &_attitudeModel; }

void FlightVechicle::updateDeviceModel(int idx) { setDeviceIdx(idx); }

void FlightVechicle::ReadModule(const QXlsx::Worksheet *currSheet, QMap<QString, int> &columnStatus,
                                FlightStatus *groupCommand, FactDecoderMgr *decoder, int start, int end) {

    QVariant subGroup = QVariant();
    for (int row = start; row <= end; row++) {
        std::shared_ptr<FlightDisplay> dataElem(new FlightDisplay());

        QVariant submodule;
        auto xmx = columnStatus.find(DisplaySubModule);
        if ((xmx != columnStatus.end())) {
            int col = xmx.value();
            submodule = currSheet->read(row, col);
        }

        QVariant frame;
        if (columnStatus.find(DisplayRefFrame) != columnStatus.end()) {
            frame = currSheet->read(row, columnStatus.find(DisplayRefFrame).value());
        }
        if (frame.isValid()) {
            QString tmp = frame.toString();
            dataElem->setFrameName(tmp.trimmed());
        }

        QVariant frameType;
        if (columnStatus.find(DisplayRefFType) != columnStatus.end()) {
            frameType = currSheet->read(row, columnStatus.find(DisplayRefFType).value());
        }
        if (frameType.isValid()) {
            QString tmp = frameType.toString();
            dataElem->setFrameCode(tmp);
        }

        QVariant refData;
        if (columnStatus.find(DisplayRefData) != columnStatus.end()) {
            refData = currSheet->read(row, columnStatus.find(DisplayRefData).value());
        }
        if (refData.isValid()) {
            QString tmp = refData.toString();
            dataElem->setShowIndex(tmp);
        }

        QVariant formatStr;
        if (columnStatus.find(DisplayDataIndex) != columnStatus.end()) {
            formatStr = currSheet->read(row, columnStatus.find(DisplayDataIndex).value());
        }
        if (formatStr.isValid()) {
            QString tmp = formatStr.toString();
            dataElem->setFormatStr(tmp);
        }

        QVariant dataName;
        if (columnStatus.find(DisplayDataName) != columnStatus.end()) {
            dataName = currSheet->read(row, columnStatus.find(DisplayDataName).value());
        }
        if (dataName.isValid()) {
            QString tmp = dataName.toString();
            dataElem->setItemName(tmp.trimmed());
        }

        QVariant colorName;
        if (columnStatus.find(DisplayColorSet) != columnStatus.end()) {
            colorName = currSheet->read(row, columnStatus.find(DisplayColorSet).value());
        }

        if (colorName.isValid()) {
            QString tmp = colorName.toString();
            dataElem->setShowMode(tmp.trimmed().toInt());
        }

        FactTableModel *curTable = nullptr;
        if ((decoder != nullptr) && frame.isValid()) {
            curTable = decoder->protoframe(dataElem->frameName());
        }
        if ((curTable == nullptr) && frameType.isValid()) {
            curTable = decoder->protoframe(frameType.toInt());
        }

        if ((curTable != nullptr) && (curTable->code() != 0)) {
            dataElem->setFrameCode(curTable->code());
            dataElem->insertFactElem(curTable->loadFact(dataElem->showIndex()));
        } else {

            if (frameType.isValid() && refData.isValid() &&
                (refData.toString().contains("/") || refData.toString().contains("-") ||
                 refData.toString().contains("+"))) {
                QRegularExpression splitExpress("[//+-]");
                QString frameStr = frameType.toString();
                QString refdataStr = refData.toString();
                QStringList frameCode = frameStr.split(splitExpress);
                QStringList refdataCode = refdataStr.split(splitExpress);
                FactTableModel *curTable2 = nullptr;
                int codeArr[2] = {0};

                for (int ixx = 0; ixx < refdataCode.size(); ixx++) {
                    codeArr[ixx % 2] = refdataCode[ixx].toInt();
                }

                for (int ixx = 0; ixx < frameCode.size(); ixx++) {
                    int frameId = frameCode[ixx].toInt();
                    if (frameCode[ixx].contains("0x", Qt::CaseInsensitive)) {
                        frameId = frameCode[ixx].toInt(nullptr, 16);
                    }
                    if (ixx == 0) {
                        curTable = decoder->protoframe(frameId);
                        curTable2 = decoder->protoframe(frameId);
                    } else {
                        curTable2 = decoder->protoframe(frameId);
                    }
                }

                if (curTable != nullptr && curTable2 != nullptr) {
                    dataElem->insertFactElemExt(curTable->loadFact(codeArr[0]), curTable2->loadFact(codeArr[1]));
                } else {
                    COMMON_WARN("error config {}, row: {}", frameStr.toStdString(), dataElem->showIndex());
                }
            } else {
                COMMON_INFO("not found fact table by {}", dataElem->frameName().toStdString().c_str());
            }
        }

        if (submodule.isValid()) {
            subGroup = submodule;
            groupCommand->addGroup(dataElem, subGroup.toString());
        } else if (subGroup.isValid()) {
            groupCommand->addGroup(dataElem, subGroup.toString());
        }
    }
}

void FlightVechicle::ReadModule(const QXlsx::Worksheet *currSheet, QMap<QString, int> &columnStatus,
                                std::shared_ptr<FlightDevice> &module, FactDecoderMgr *decoder, int frow, int lrow) {

    QString indexBlock("");
    std::shared_ptr<FlightStatus> groupCommand = nullptr;
    for (int row = frow; (row <= lrow); row++) {
        std::shared_ptr<FlightDisplay> dataElem(new FlightDisplay());

        QVariant submodule;
        auto xmx = columnStatus.find(DisplaySubModule);
        if ((xmx != columnStatus.end())) {
            int col = xmx.value();
            submodule = currSheet->read(row, col);
        }

        QVariant frame;
        xmx = columnStatus.find(DisplayRefFrame);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            frame = currSheet->read(row, col);
        }
        if (frame.isValid()) {
            QString tmp = frame.toString();
            dataElem->setFrameName(tmp.trimmed());
        }

        QVariant frameType;
        xmx = columnStatus.find(DisplayRefFType);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            frameType = currSheet->read(row, col);
        }
        if (frameType.isValid()) {
            QString tmp = frameType.toString();
            dataElem->setFrameCode(tmp);
        }

        QVariant refData;
        xmx = columnStatus.find(DisplayRefData);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            refData = currSheet->read(row, col);
        }
        if (refData.isValid()) {
            QString tmp = refData.toString();
            dataElem->setShowIndex(tmp);
        }

        QVariant dataName;
        xmx = columnStatus.find(DisplayDataName);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            dataName = currSheet->read(row, col);
        }
        if (dataName.isValid()) {
            QString tmp = dataName.toString();
            dataElem->setItemName(tmp.trimmed());
        }

        QVariant colorName;
        xmx = columnStatus.find(DisplayColorSet);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            colorName = currSheet->read(row, col);
        }
        if (colorName.isValid()) {
            QString tmp = colorName.toString();
            dataElem->setShowMode(tmp.toInt());
        }

        // if (dataElem->frameName().isEmpty() && dataElem->frameCode() <= 0) {
        //     COMMON_INFO("not to define the display node ",
        //                 dataElem->frameName().toStdString().c_str());
        //     continue;
        // }

        const FactTableModel *curTable = nullptr;
        if ((decoder != nullptr) && frame.isValid()) {
            curTable = decoder->protoframe(dataElem->frameName());
        }
        if (curTable == nullptr && dataElem->frameCode() > 0) {
            curTable = decoder->protoframe(dataElem->frameCode());
        }

        if (dataElem->showIndex() < 0) {
            int frameright = dataElem->frameCode() & 0xffff;
            int frameleft = (dataElem->frameCode() >> 16) & 0xffff;
            int itemright = dataElem->showIndex() & 0xff;
            int itemleft = (dataElem->showIndex() >> 8) & 0xff;
            const FactTableModel *curTable2 = nullptr;
            curTable2 = decoder->protoframe(frameright);
            curTable = decoder->protoframe(frameleft);
            if (curTable != nullptr && curTable2 != nullptr) {
                if (colorName.isValid()) {
                    QString tmps = colorName.toString();
                    dataElem->setCondition(tmps);
                }
                dataElem->insertFactElemExt(curTable->loadFact(itemleft), curTable2->loadFact(itemright));
            } else {
                COMMON_WARN("error config, frame: {}, idx: {}", dataElem->frameCode(), dataElem->showIndex());
            }
        } else {
            /// 默认一个数据元素的情况
            if ((curTable != nullptr) && (curTable->code() != 0)) {
                dataElem->setFrameCode(curTable->code());
                dataElem->insertFactElem(curTable->loadFact(dataElem->showIndex()));
            }
        }

        // else {
        //     COMMON_INFO("not found fact table by {}", row);
        //     continue;
        // }

        if (groupCommand && !submodule.isValid()) {
            groupCommand->addDefault(dataElem);
        } else {
            if (groupCommand != nullptr) {
                module->addStatus(groupCommand);
            }

            groupCommand = std::shared_ptr<FlightStatus>(new FlightStatus());
            if (groupCommand != nullptr) {
                groupCommand->setMapGroup(submodule.toString().trimmed());
                groupCommand->addDefault(dataElem);

            } else {
                COMMON_INFO("allocate memory failed");
            }
        }
    }

    if (groupCommand && groupCommand->count() > 0) {
        module->addStatus(groupCommand);
    } else {
        COMMON_INFO("no element {} {}", frow, lrow);
    }
}

bool FlightVechicle::reloadDisplay(FactDecoderMgr *decoder, const QString &file) {
    QXlsx::Document xlsxRead(file);
    if (!xlsxRead.load()) {
        return false;
    }

    _attitudeModel.initColumnHeader();

    QStringList list = xlsxRead.sheetNames();
    for (int i = 0; i < list.length(); i++) {
        QString protoFrame = list[i].trimmed();
        if (!xlsxRead.selectSheet(protoFrame)) {
            continue;
        }

        QXlsx::Worksheet *currSheet = xlsxRead.currentWorksheet();
        QXlsx::CellRange range = currSheet->dimension();
        int fcol = range.firstColumn();
        int lcol = range.lastColumn();
        int frow = range.firstRow();
        int lrow = range.lastRow();

        QVector<QString> tableHeader;
        tableHeader.append(DisplayIndex);
        tableHeader.append(DisplayRefFrame);
        tableHeader.append(DisplayRefFType);
        tableHeader.append(DisplaySubModule);
        tableHeader.append(DisplayRefData);
        tableHeader.append(DisplayDataName);
        tableHeader.append(DisplayDataIndex);
        tableHeader.append(DisplayModuleAbbr);
        tableHeader.append(DisplayColorSet);

        QMap<QString, int> columnStatus;

        bool getValid = false;
        while ((frow <= lrow) && (!getValid)) {
            for (int col = fcol; (col <= lcol) && (!getValid); col++) {
                QVariant value = currSheet->read(frow, col);
                getValid = value.isValid();
            }

            if (getValid) { // table header
                for (int col = fcol; col <= lcol; col++) {
                    QVariant value = currSheet->read(frow, col);
                    if (!value.isNull()) {
                        for (auto itr = tableHeader.begin(); itr != tableHeader.end(); itr++) {
                            if (itr->contains(value.toString().trimmed())) {
                                columnStatus.insert(*itr, col);
                            }
                        }
                    } else {
                        COMMON_WARN("read excel {} {} failed", frow, col);
                    }
                }
            }

            ++frow;
        }

        if (columnStatus.size() <= 0) {
            COMMON_INFO("read excel failed");
            return false;
        }

        if (protoFrame.compare(DisplayModulePage) == 0) {
            int srow = 0;
            QString indexCurrLast;

            for (int row = frow; row <= lrow; row++) {
                QVariant indexCurr;
                auto gx = columnStatus.find(DisplayIndex);
                if (gx != columnStatus.end()) {
                    indexCurr = currSheet->read(row, gx.value());
                }
                if (indexCurr.isValid()) {
                    if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("NavigationExt")) {
                        _attitudeModel.ReadModule(currSheet, columnStatus, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("Navigation")) {
                        _navigationElems.ReadModule<NavigationElemNode>(currSheet, columnStatus, decoder, srow,
                                                                        row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("EIMCStatus")) {
                        _emicElems.ReadModule<BatteryElemNode>(currSheet, columnStatus, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("BrakeStatus")) {
                        _brakeElems.ReadModule(currSheet, columnStatus, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("Battery")) {
                        _batteryElems.ReadModule<BatteryElemNode>(currSheet, columnStatus, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("FlapRundder")) {
                        _flapRundder.ReadModule<FlapRunnderNode>(currSheet, columnStatus, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("ElecSpdCtrl")) {
                        _elecSpdCtrl.ReadModule<ElecSpdCtrlElemNode>(currSheet, columnStatus, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("EngineMode")) {
                        _engineElem.ReadModule<EngineElemNode>(currSheet, columnStatus, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("Temperature")) {
                        _temperatureElems.ReadModule<SimpleElemNode>(currSheet, columnStatus, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("FlightMission")) {
                        ReadModule(currSheet, columnStatus, &_missionElem, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("Generator")) {
                        _generatorMode.ReadModule<BatteryElemNode>(currSheet, columnStatus, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("FlightStates")) {
                        ReadModule(currSheet, columnStatus, &_flightStates, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("VehicleMode")) {
                        ReadModule(currSheet, columnStatus, &_vehicleMode, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("ModelStates")) {
                        ReadModule(currSheet, columnStatus, &_modelStates, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("MainNavBlock")) {
                        ReadModule(currSheet, columnStatus, &_mainNavBlock, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("MainCommBlock")) {
                        ReadModule(currSheet, columnStatus, &_mainCommBlock, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("NavKeyPage")) {
                        ReadModule(currSheet, columnStatus, &_navKeyPage, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("TaskKeyPage")) {
                        ReadModule(currSheet, columnStatus, &_taskKeyPage, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("FlightStateA")) {
                        ReadModule(currSheet, columnStatus, &_flightCommonA, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("FlightStateB")) {
                        ReadModule(currSheet, columnStatus, &_flightCommonB, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("AttiLeftShow")) {
                        ReadModule(currSheet, columnStatus, &_attiLeftShow, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("AttiCenterShow")) {
                        ReadModule(currSheet, columnStatus, &_attiCenterShow, decoder, srow, row - 1);
                    } else if (srow > 0 && !indexCurrLast.isEmpty() && indexCurrLast.contains("AttiRightShow")) {
                        ReadModule(currSheet, columnStatus, &_attiRightShow, decoder, srow, row - 1);
                    } else {
                        if (srow > 0 && !indexCurrLast.isEmpty()) {
                            std::shared_ptr<FlightDevice> displayDevice = nullptr;
                            displayDevice = std::shared_ptr<FlightDevice>(new FlightDevice());
                            displayDevice->setMapGroup(indexCurrLast);
                            ReadModule(currSheet, columnStatus, displayDevice, decoder, srow, row - 1);
                            _moduelPage_set.insert(indexCurrLast, displayDevice);
                        }
                    }
                    indexCurrLast = indexCurr.toString().trimmed();
                    srow = row;
                }
            }

            if (srow >= 0 && !indexCurrLast.isEmpty()) {
                std::shared_ptr<FlightDevice> displayDevice = nullptr;
                displayDevice = std::shared_ptr<FlightDevice>(new FlightDevice());
                displayDevice->setMapGroup(indexCurrLast);
                ReadModule(currSheet, columnStatus, displayDevice, decoder, srow, lrow);
                _moduelPage_set.insert(indexCurrLast, displayDevice);
            }

        } else {
            int srow = 0;
            std::shared_ptr<FlightDevice> displayDevice = nullptr;
            displayDevice = std::shared_ptr<FlightDevice>(new FlightDevice());

            QString indexCurrLast = "";
            for (int row = frow; row <= lrow; row++) {
                QVariant indexCurr;
                auto gx = columnStatus.find(DisplayIndex);
                if (gx != columnStatus.end()) {
                    indexCurr = currSheet->read(row, gx.value());
                }

                if (indexCurr.isValid()) {
                    if (srow > 0 && !indexCurrLast.isEmpty()) {
                        std::shared_ptr<FlightStatus> groupCommand(new FlightStatus());
                        assert(groupCommand != nullptr);

                        groupCommand->setMapGroup(indexCurrLast);
                        ReadModule(currSheet, columnStatus, groupCommand.get(), decoder, srow, row - 1);
                        displayDevice->addStatus(groupCommand);
                    }

                    indexCurrLast = indexCurr.toString();
                    srow = row;
                }
            }

            if (srow >= 0 && !indexCurrLast.isEmpty()) {
                std::shared_ptr<FlightStatus> groupCommand(new FlightStatus());
                assert(groupCommand != nullptr);

                groupCommand->setMapGroup(indexCurrLast);
                ReadModule(currSheet, columnStatus, groupCommand.get(), decoder, srow, lrow);
                displayDevice->addStatus(groupCommand);
            }

            displayDevice->setMapGroup(protoFrame);
            _device_set.insert(protoFrame, displayDevice);
            _deviceList.append(displayDevice.get());
        }
    }

    return true;
}

FlightDevice *FlightVechicle::flightManage() {
    if (_deviceList.count() > 0) {
        return _deviceList.value<FlightDevice *>(0);
    }
    return nullptr;
}

FlightDevice *FlightVechicle::navigateSensor() {
    if (_deviceList.count() > 1) {
        return _deviceList.value<FlightDevice *>(1);
    }
    return nullptr;
}

FlightDevice *FlightVechicle::enginePower() {
    if (_deviceList.count() > 2) {
        return _deviceList.value<FlightDevice *>(2);
    }
    return nullptr;
}

FlightDevice *FlightVechicle::deviceExecutor() {
    if (_deviceList.count() > 3) {
        return _deviceList.value<FlightDevice *>(3);
    }
    return nullptr;
}

FlightDevice *FlightVechicle::extraDevice() {
    if (_deviceList.count() > 4) {
        return _deviceList.value<FlightDevice *>(4);
    }
    return nullptr;
}

FlightDevice *FlightVechicle::dataLinker() {
    if (_deviceList.count() > 5) {
        return _deviceList.value<FlightDevice *>(5);
    }
    return nullptr;
}

FlightDevice *FlightVechicle::airTrafficControl() {
    if (_deviceList.count() > 6) {
        return _deviceList.value<FlightDevice *>(6);
    }
    return nullptr;
}

void FlightVechicle::setVehicleAddr(int val) { _vehicleAddr = val; }

void FlightVechicle::setCurrNavMode(int val) { _attitudeModel.setNavigationMode(val); }

/**
 * 设置飞行器索引并更新名称
 * @param val 要设置的索引值
 */
void FlightVechicle::setVehicleIdx(int val) {
    // 设置飞行器索引值
    _vehicleIdx = val;
    // 将索引值转换为字符串并设置为飞行器名称
    _name = QString::number(val);
    // 发射飞行器索引变化的信号
    emit vehicleIdxChanged();
}

FlightDevice *FlightVechicle::deviceSelect() {
    COMMON_TRACE("switch detail panel {}", _deviceIdx);
    return _deviceList.value<FlightDevice *>(_deviceIdx);
}

void FlightVechicle::setDeviceIdx(int val) {
    _deviceIdx = val;
    emit deviceIdxChanged();
}

int64_t FlightVechicle::currentTimeMs() {
    typedef std::chrono::milliseconds ms_precision;
    typedef std::chrono::time_point<std::chrono::system_clock, ms_precision> time_precision;
    time_precision tp = std::chrono::time_point_cast<ms_precision>(std::chrono::system_clock::now());
    return tp.time_since_epoch().count();
}

void FlightVechicle::summaryDataLink(int datas, int frame) {

    auto gx = _datalinker_summary.find(frame);
    if (gx == _datalinker_summary.end()) {
        _datalinker_summary.insert(frame, datas);
    } else {
        _datalinker_summary[frame] += datas;
    }
}

void FlightVechicle::doUnitTest(int testval) {
    _engineElem.doUnitTest(testval);
    _emicElems.doUnitTest(testval);
    _batteryElems.doUnitTest(testval);
    _temperatureElems.doUnitTest(testval);
}

void FlightVechicle::decodeWarnData(CheckWarnCallback first, FactTableModelCallabck tableModelCB, int chl) {
    assert(tableModelCB != nullptr);
    assert(first != nullptr);

    for (auto var = warn_ext.begin(); var != warn_ext.end(); var++) {
        PWarnConfGrpK config = var.key();
        PWarnConfGrpV value = var.value();
        assert(config != nullptr);

        /// 判断条件
        if (config == nullptr || value == nullptr) {
            continue;
        }

        std::shared_ptr<WarnConfigExt> warnExt = nullptr;
        int refframe = config->_referFrame[0];
        int refitem = config->_referItem[0];

        // 设备告警的在时间10s后不触发告警则清除该告警颜色显示
        value->resetWarnColor(10000);

        FactTableModel *mx = tableModelCB(refframe);
        if (mx != nullptr) {

            FactMetaData *frameData = mx->loadFact(refitem);
            assert(frameData != nullptr); /// meta data item
            if (frameData == nullptr) {
                continue;
            }

            int valueelem = frameData->display(chl).toInt();
            warnExt = value->findBySeq(valueelem);
            if (warnExt == nullptr) {
                COMMON_TRACE("ref {}, item {}, value {}", refframe, refitem, valueelem);
                continue;
            }
        }

        if (warnExt != nullptr) {

            QString timestamp = mx->currTimestamp(chl);
            refframe = config->_referFrame[1];
            refitem = config->_referItem[1];

            FactTableModel *bcx = tableModelCB(refframe);
            assert(bcx != nullptr);
            if (bcx == nullptr) {
                continue;
            }

            FactMetaData *frameData = bcx->loadFact(refitem);
            assert(frameData != nullptr); /// meta data item
            if (frameData == nullptr) {
                continue;
            }

            uint32_t value = frameData->display(chl).toInt();
            std::shared_ptr<WarnConfNode> conf = warnExt->findByValue(value);
            if (conf != nullptr) {
                QString warnStr = conf->warnStr;
                std::shared_ptr<WarnElemNode> warnNode(new (WarnElemNode));
                warnNode->group(warnStr);
                warnNode->level(conf->level);
                warnNode->name(warnExt->_warnItemName);
                warnNode->timestamp(timestamp);
                warnNode->priority(conf->priority);
                first(warnNode);
            }
        }
    }
}
