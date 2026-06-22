#include "VehicleElement.h"

void SimpleElemNode::doUnitTest(int testval) { _default.unittestDisplay(testval, 0); }

void ElecSpdCtrlElemNode::doUnitTest(int testval) {}

void BatteryElemNode::doUnitTest(int testval) {}

void BusbarElemNode::doUnitTest(int testval) {}

void NavigationElemNode::doUnitTest(int testval) {}

void EngineElemNode::doUnitTest(int testval) {}

void FlapRunnderNode::doUnitTest(int testval) {
    _throttle.unittestDisplay(testval, 0);
    _engineSpd.unittestDisplay(testval, 0);
}

FlightDisplay *FlapRunnderNode::getNode(const QString &name) {
    if (name.compare("LeftRudder") == 0) {
        return &_leftRudder;
    } else if (name.compare("RightRudder") == 0) {
        return &_rightRudder;
    } else if (name.compare("LeftElevator") == 0) {
        return &_leftElevator;
    } else if (name.compare("RightElevator") == 0) {
        return &_rightElevator;
    } else if (name.compare("LeftFlaps") == 0) {
        return &_leftFlaps;
    } else if (name.compare("RightFlaps") == 0) {
        return &_rightFlaps;
    } else if (name.compare("LeftAileron") == 0) {
        return &_leftAileron;
    } else if (name.compare("RightAileron") == 0) {
        return &_rightAileron;
    } else if (name.compare("Throttle") == 0) {
        return &_throttle;
    } else if (name.compare("EngineSpd") == 0) {
        return &_engineSpd;
    } else {
        return nullptr;
    }
}

void BatteryElemNode::enableRawUpdate() {
    if (raw_update) {
        if (_capacity != nullptr) {
            _capacity->notifyRawDisplay();
        }
        if (_valtage != nullptr) {
            _valtage->notifyRawDisplay();
        }
    }
}

FlightDisplay *BatteryElemNode::getNode(const QString &name) {
    if (name.compare("valtage") == 0) {
        _valtage = getDisplayNode(name);
        return _valtage;
    } else if (name.compare("current") == 0) {
        _current = getDisplayNode(name);
        return _current;
    } else if (name.compare("lineColor") == 0) {
        _lineColor = getDisplayNode(name);
        return _lineColor;
    } else if (name.compare("borderColor") == 0) {
        _borderColor = getDisplayNode(name);
        return _borderColor;
    } else if (name.compare("capacity") == 0) {
        _capacity = getDisplayNode(name);
        return _capacity;
    } else if (name.compare("charge") == 0) {
        _charge = getDisplayNode(name);
        return _charge;
    } else if (name.compare("discharge") == 0) {
        _discharge = getDisplayNode(name);
        return _discharge;
    } else {
        _extrakey.append(name);
        return getDisplayNode(name);
    }
}

void ElecSpdCtrlElemNode::enableRawUpdate() {
    if (raw_update) {
        if (_discharge != nullptr) {
            _discharge->notifyRawDisplay();
        }

        if (_charge != nullptr) {
            _charge->notifyRawDisplay();
        }
    }
}

FlightDisplay *ElecSpdCtrlElemNode::getNode(const QString &name) {

    if (name.compare("electronic") == 0) {
        _electronic = getDisplayNode(name);
        return _electronic;
    } else if (name.compare("variablepitch") == 0) {
        _variablepitch = getDisplayNode(name);
        return _variablepitch;
    } else if (name.compare("tilting") == 0) {
        _tilting = getDisplayNode(name);
        return _tilting;
    } else if (name.compare("charge") == 0) {
        _charge = getDisplayNode(name);
        raw_update = true;
        return _charge;
    } else if (name.compare("discharge") == 0) {
        _discharge = getDisplayNode(name);
        raw_update = true;
        return _discharge;
    } else {
        return nullptr;
    }
}

void EngineElemNode::unitTest() {}

FlightDisplay *EngineElemNode::getNode(const QString &name) {
    if (name.compare("inletTotalTmp") == 0) {
        _inletTotalTmp = getDisplayNode(name);
        return _inletTotalTmp;
    } else if (name.compare("oilTmp") == 0) {
        _oilTmp = getDisplayNode(name);
        return _oilTmp;
    } else if (name.compare("oilPressure") == 0) {
        _oilPressure = getDisplayNode(name);
        return _oilPressure;
    } else if (name.compare("mainFuelPump") == 0) {
        _mainFuelPump = getDisplayNode(name);
        return _mainFuelPump;
    } else if (name.compare("slaveFuelPump") == 0) {
        _slaveFuelPump = getDisplayNode(name);
        return _slaveFuelPump;
    } else if (name.compare("lowFuelWarn") == 0) {
        _lowFuelWarn = getDisplayNode(name);
        return _lowFuelWarn;
    } else if (name.compare("throttle") == 0) {
        _throttle = getDisplayNode(name);
        return _throttle;
    } else if (name.compare("physicalRSpeed") == 0) {
        _physicalRSpeed = getDisplayNode(name);
        return _physicalRSpeed;
    } else if (name.compare("givenSpeed") == 0) {
        _givenSpeed = getDisplayNode(name);
        return _givenSpeed;
    } else if (name.compare("engineGear") == 0) {
        _engineGear = getDisplayNode(name);
        return _engineGear;
    } else if (name.compare("exhuastTmp") == 0) {
        _exhuastTmp = getDisplayNode(name);
        return _exhuastTmp;
    } else if (name.compare("engineLevel") == 0) {
        _engineLevel = getDisplayNode(name);
        return _engineLevel;
    } else if (name.compare("pitchControl") == 0) {
        _pitchControl = getDisplayNode(name);
        return _pitchControl;
    } else {
        return nullptr;
    }
}

FlightDisplay *BakeComponent::getNode(const QString &name) {
    if (name.compare("leftWheelState") == 0) {
        return &_leftWheelState;
    } else if (name.compare("rightWheelState") == 0) {
        return &_rightWheelState;
    } else {
        return nullptr;
    }
}

FlightDisplay *BusbarElemNode::getNode(const QString &name) {
    if (name.compare("valtage") == 0) {
        _valtage = getDisplayNode(name);
        return _valtage;
    } else if (name.compare("current") == 0) {
        _current = getDisplayNode(name);
        return _current;
    } else if (name.compare("lineColor") == 0) {
        _lineColor = getDisplayNode(name);
        return _lineColor;
    } else if (name.compare("borderColor") == 0) {
        _borderColor = getDisplayNode(name);
        return _borderColor;
    } else if (name.compare("capacity") == 0) {
        _capacity = getDisplayNode(name);
        return _capacity;
    } else {
        return nullptr;
    }
}

FlightDisplay *NavigationElemNode::getNode(const QString &name) {
    if (name.compare("rowname") == 0) {
        _name = getDisplayNode(name);
        return _name;
    } else if (name.compare("mainnav") == 0) {
        _main = getDisplayNode(name);
        return _main;
    } else if (name.compare("slavenav") == 0) {
        _slave = getDisplayNode(name);
        return _slave;
    } else {
        return nullptr;
    }
}

void SimpleElemNode::setModuleAbbr(const QString &name) { _moduleAbbr = name; }

void SimpleElemNode::setGroupName(const QString &name) {
    _vertGName = FactMetaData::vertString(name);
    _vertG2Name = FactMetaData::vert2String(name);
    _vertG3Name = FactMetaData::vert3String(name);
    _groupName = name;
}

void ComponentConf::ReadModule(const QXlsx::Worksheet *currSheet, QMap<QString, int> &columnStatus,
                               FactDecoderMgr *decoder, int frow, int lrow) {

    for (int row = frow; (row <= lrow); row++) {
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

        QVariant refFrame;
        xmx = columnStatus.find(DisplayRefFType);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            refFrame = currSheet->read(row, col);
        }

        QVariant refData;
        xmx = columnStatus.find(DisplayRefData);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            refData = currSheet->read(row, col);
        }

        QVariant dataName;
        xmx = columnStatus.find(DisplayDataName);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            dataName = currSheet->read(row, col);
        }

        QVariant dataIndex;
        xmx = columnStatus.find(DisplayDataIndex);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            dataIndex = currSheet->read(row, col);
        }

        QVariant colorName;
        xmx = columnStatus.find(DisplayColorSet);
        if (xmx != columnStatus.end()) {
            int col = xmx.value();
            colorName = currSheet->read(row, col);
        }

        const FactTableModel *curTable = nullptr;
        if ((decoder != nullptr) && frame.isValid()) {
            curTable = decoder->protoframe(frame.toString());
        }
        if ((curTable == nullptr) && refFrame.isValid()) {
            curTable = decoder->protoframe(refFrame.toInt());
        }

        if (submodule.isValid()) {
            setGroupName(submodule.toString());
        }

        FlightDisplay *node = nullptr;
        if (dataIndex.isValid()) {
            node = getNode(dataIndex.toString());
        } else {
            node = getNode("default");
        }

        if (node == nullptr) {
            COMMON_WARN("invalid data index name");
            continue;
        }

        if (colorName.isValid()) {
            QString tmp = colorName.toString();
            node->setShowMode(tmp.trimmed().toInt());
        }

        if (dataName.isValid()) {
            QString tmp = dataName.toString();
            node->setItemName(tmp.trimmed());
        }

        if (frame.isValid()) {
            QString tmp = frame.toString();
            node->setFrameName(tmp.trimmed());
        }

        if (refData.isValid()) {
            QString tmp = refData.toString();
            node->setShowIndex(tmp.trimmed().toInt());
        }

        if ((curTable != nullptr) && (curTable->code() != 0)) {
            QString tmp = refData.toString();
            node->insertFactElem(curTable->loadFact(tmp.trimmed().toInt()));
        } else {
            COMMON_INFO("not found fact by {}: {}", frame.toString().toStdString().c_str(),
                        refData.toString().toStdString().c_str());
        }
    }
}

void SimpleElemList::doUnitTest(int testval) {
    for (auto xm = nodeList.begin(); xm != nodeList.end(); xm++) {
        xm->get()->doUnitTest(testval);
    }
}
