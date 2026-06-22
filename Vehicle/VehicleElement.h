#pragma once

#include "FlightDisplay.h"
#include "factdecodermgr.h"
#include "log_category.h"
#include "xlsxchart.h"
#include "xlsxworkbook.h"
#include "xlsxworksheet.h"

class VEHICLE_EXPORT SimpleElemNode : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(SimpleElemNode)
    Q_PROPERTY(FlightDisplay *defNode READ defNode CONSTANT)
    Q_PROPERTY(QString groupName READ groupName CONSTANT)
    Q_PROPERTY(QString moduleAbbr READ moduleAbbr CONSTANT)
    Q_PROPERTY(QString vGroupName READ vGroupName CONSTANT)
    Q_PROPERTY(QString v2GroupName READ v2GroupName CONSTANT)

  public:
    SimpleElemNode() = default;
    void setGroupName(const QString &name);
    void setModuleAbbr(const QString &name);
    QString moduleAbbr() const { return _moduleAbbr; }
    QString groupName() const { return _groupName; }
    QString vGroupName() const { return _vertGName; }
    QString v2GroupName() const { return _vertG2Name; }
    QString v3GroupName() const { return _vertG3Name; }
    double defRateValue() { return _default.rateValue(); }
    FlightDisplay *defNode() { return &_default; }

    ///
    /// \brief getNode
    /// \param name
    /// \return
    ///
    virtual FlightDisplay *getNode(const QString &name) { return &_default; }
    virtual void doUnitTest(int testval);
    virtual void enableRawUpdate() { ; }

    ///
    /// \brief getDisplayNode
    /// \param name
    /// \return
    ///
    FlightDisplay *getDisplayNode(const QString &name) {
        std::shared_ptr<FlightDisplay> elem(new FlightDisplay());
        cache.insert(name, elem);
        return elem.get();
    }

    int moduleCount() { return cache.count(); }

    QStringList extrakey() { return _extrakey; }

    FlightDisplay *serchModule(const QString &ix) {
        auto mx = cache.find(ix);
        if (mx != cache.end()) {
            return mx.value().get();
        }
        return nullptr;
    }

  public:
    FlightDisplay _default;

  protected:
    QStringList _extrakey;
    QMap<QString, std::shared_ptr<FlightDisplay>> cache;
    QString _moduleAbbr = "";
    QString _vertG3Name = "";
    QString _vertG2Name = "";
    QString _groupName = "";
    QString _vertGName = "";
};

class VEHICLE_EXPORT FlapRunnderNode : public SimpleElemNode {
    Q_OBJECT
    Q_PROPERTY(FlightDisplay *leftRudder READ leftRudder CONSTANT)
    Q_PROPERTY(FlightDisplay *rightRudder READ rightRudder CONSTANT)
    Q_PROPERTY(FlightDisplay *leftElevator READ leftElevator CONSTANT)
    Q_PROPERTY(FlightDisplay *rightElevator READ rightElevator CONSTANT)
    Q_PROPERTY(FlightDisplay *leftFlaps READ leftFlaps CONSTANT)
    Q_PROPERTY(FlightDisplay *rightFlaps READ rightFlaps CONSTANT)
    Q_PROPERTY(FlightDisplay *leftAileron READ leftAileron CONSTANT)
    Q_PROPERTY(FlightDisplay *rightAileron READ rightAileron CONSTANT)
    Q_PROPERTY(FlightDisplay *throttle READ throttle CONSTANT)
    Q_PROPERTY(FlightDisplay *engineSpd READ engineSpd CONSTANT)

  public:
    FlapRunnderNode() = default;
    FlightDisplay *leftRudder() { return &_leftRudder; }
    FlightDisplay *rightRudder() { return &_rightRudder; }
    FlightDisplay *leftElevator() { return &_leftElevator; }
    FlightDisplay *rightElevator() { return &_rightElevator; }
    FlightDisplay *leftFlaps() { return &_leftFlaps; }
    FlightDisplay *rightFlaps() { return &_rightFlaps; }
    FlightDisplay *leftAileron() { return &_leftAileron; }
    FlightDisplay *rightAileron() { return &_rightAileron; }
    FlightDisplay *throttle() { return &_throttle; }
    FlightDisplay *engineSpd() { return &_engineSpd; }

    virtual void doUnitTest(int testval);
    virtual FlightDisplay *getNode(const QString &name);

    FlightDisplay _throttle;
    FlightDisplay _engineSpd;
    FlightDisplay _leftRudder;
    FlightDisplay _rightRudder;
    FlightDisplay _leftElevator;
    FlightDisplay _rightElevator;
    FlightDisplay _leftFlaps;
    FlightDisplay _rightFlaps;
    FlightDisplay _leftAileron;
    FlightDisplay _rightAileron;
};

class VEHICLE_EXPORT BusbarElemNode : public SimpleElemNode {
    Q_OBJECT

    Q_PROPERTY(FlightDisplay *capacity READ capacity CONSTANT)
    Q_PROPERTY(FlightDisplay *valtage READ valtage CONSTANT)
    Q_PROPERTY(FlightDisplay *current READ current CONSTANT)
    Q_PROPERTY(FlightDisplay *lineColor READ lineColor CONSTANT)
    Q_PROPERTY(FlightDisplay *borderColor READ borderColor CONSTANT)

  public:
    BusbarElemNode() = default;
    FlightDisplay *capacity() { return _capacity; }
    FlightDisplay *valtage() { return _valtage; }
    FlightDisplay *current() { return _current; }
    FlightDisplay *lineColor() { return _lineColor; }
    FlightDisplay *borderColor() { return _borderColor; }
    virtual void doUnitTest(int testval);
    virtual FlightDisplay *getNode(const QString &name);

  protected:
    FlightDisplay *_capacity = nullptr;
    FlightDisplay *_valtage = nullptr;
    FlightDisplay *_current = nullptr;
    FlightDisplay *_lineColor = nullptr;
    FlightDisplay *_borderColor = nullptr;
};

class VEHICLE_EXPORT NavigationElemNode : public SimpleElemNode {
    Q_OBJECT
    Q_PROPERTY(FlightDisplay *name READ name CONSTANT)
    Q_PROPERTY(FlightDisplay *main READ main CONSTANT)
    Q_PROPERTY(FlightDisplay *slave READ slave CONSTANT)
  public:
    NavigationElemNode() = default;
    FlightDisplay *name() { return _name; }
    FlightDisplay *main() { return _main; }
    FlightDisplay *slave() { return _slave; }

    virtual void doUnitTest(int testval);
    virtual FlightDisplay *getNode(const QString &name);

  protected:
    FlightDisplay *_name = nullptr;
    FlightDisplay *_main = nullptr;
    FlightDisplay *_slave = nullptr;
};

class VEHICLE_EXPORT BatteryElemNode : public BusbarElemNode {
    Q_OBJECT
    Q_PROPERTY(FlightDisplay *charge READ charge CONSTANT)
    Q_PROPERTY(FlightDisplay *discharge READ discharge CONSTANT)

  public:
    BatteryElemNode() = default;
    FlightDisplay *discharge() { return _discharge; }
    FlightDisplay *charge() { return _charge; }

    virtual void enableRawUpdate();
    virtual void doUnitTest(int testval);
    virtual FlightDisplay *getNode(const QString &name);

  protected:
    FlightDisplay *_charge = nullptr;
    FlightDisplay *_discharge = nullptr;
    bool raw_update = false;
};

class VEHICLE_EXPORT ElecSpdCtrlElemNode : public SimpleElemNode {
    Q_OBJECT
    Q_PROPERTY(FlightDisplay *electronic READ electronic CONSTANT)
    Q_PROPERTY(FlightDisplay *variablepitch READ variablepitch CONSTANT)
    Q_PROPERTY(FlightDisplay *tilting READ tilting CONSTANT)
    Q_PROPERTY(FlightDisplay *discharge READ discharge CONSTANT)
    Q_PROPERTY(FlightDisplay *charge READ charge CONSTANT)

  public:
    ElecSpdCtrlElemNode() = default;
    FlightDisplay *electronic() { return _electronic; }
    FlightDisplay *variablepitch() { return _variablepitch; }
    FlightDisplay *tilting() { return _tilting; }
    FlightDisplay *discharge() { return _discharge; }
    FlightDisplay *charge() { return _charge; }

    virtual void enableRawUpdate();
    virtual void doUnitTest(int testval);
    virtual FlightDisplay *getNode(const QString &name);

  protected:
    bool raw_update = false;
    FlightDisplay *_charge = nullptr;
    FlightDisplay *_discharge = nullptr;
    FlightDisplay *_electronic = nullptr;
    FlightDisplay *_variablepitch = nullptr;
    FlightDisplay *_tilting = nullptr;
};

class VEHICLE_EXPORT EngineElemNode : public SimpleElemNode {
    Q_OBJECT
    Q_PROPERTY(FlightDisplay *engineLevel READ engineLevel CONSTANT)
    Q_PROPERTY(FlightDisplay *throttle READ throttle CONSTANT)
    Q_PROPERTY(FlightDisplay *physicalRSpeed READ physicalRSpeed CONSTANT)
    Q_PROPERTY(FlightDisplay *givenSpeed READ givenSpeed CONSTANT)
    Q_PROPERTY(FlightDisplay *inletTotalTmp READ inletTotalTmp CONSTANT)
    Q_PROPERTY(FlightDisplay *exhuastTmp READ exhuastTmp CONSTANT)
    Q_PROPERTY(FlightDisplay *oilPressure READ oilPressure CONSTANT)
    Q_PROPERTY(FlightDisplay *lowFuelWarn READ lowFuelWarn CONSTANT)
    Q_PROPERTY(FlightDisplay *pitchControl READ pitchControl CONSTANT)
    Q_PROPERTY(FlightDisplay *oilTmp READ oilTmp CONSTANT)

  public:
    EngineElemNode() = default;
    void unitTest(); // rand data
    FlightDisplay *engineLevel() { return _engineLevel; }
    FlightDisplay *throttle() { return _throttle; }
    FlightDisplay *physicalRSpeed() { return _physicalRSpeed; }
    FlightDisplay *givenSpeed() { return _givenSpeed; }
    FlightDisplay *inletTotalTmp() { return _inletTotalTmp; }
    FlightDisplay *exhuastTmp() { return _exhuastTmp; }
    FlightDisplay *oilPressure() { return _oilPressure; }
    FlightDisplay *lowFuelWarn() { return _lowFuelWarn; }
    FlightDisplay *oilTmp() { return _oilTmp; }
    FlightDisplay *pitchControl() { return _pitchControl; }
    FlightDisplay *mainFuelPump() { return _mainFuelPump; }
    FlightDisplay *slaveFuelPump() { return _slaveFuelPump; }
    FlightDisplay *engineGear() { return _engineGear; }

    virtual void doUnitTest(int testval);
    virtual FlightDisplay *getNode(const QString &name);

  protected:
    FlightDisplay *_pitchControl = nullptr;
    FlightDisplay *_inletTotalTmp = nullptr;
    FlightDisplay *_exhuastTmp = nullptr;
    FlightDisplay *_oilTmp = nullptr;
    FlightDisplay *_oilPressure = nullptr;
    FlightDisplay *_mainFuelPump = nullptr;
    FlightDisplay *_slaveFuelPump = nullptr;
    FlightDisplay *_lowFuelWarn = nullptr;
    FlightDisplay *_throttle = nullptr;
    FlightDisplay *_physicalRSpeed = nullptr;
    FlightDisplay *_givenSpeed = nullptr;
    FlightDisplay *_engineGear = nullptr;
    FlightDisplay *_engineLevel = nullptr;
};

class VEHICLE_EXPORT ComponentConf : public SimpleElemNode {
    Q_OBJECT
  public:
    void ReadModule(const QXlsx::Worksheet *currSheet, QMap<QString, int> &columnStatus,
                    FactDecoderMgr *decoder, int frow, int lrow);
};

class VEHICLE_EXPORT BakeComponent : public ComponentConf {
    Q_OBJECT
    Q_PROPERTY(FlightDisplay *leftWheelState READ leftWheelState CONSTANT)
    Q_PROPERTY(FlightDisplay *rightWheelState READ rightWheelState CONSTANT)
  public:
    BakeComponent() = default;
    FlightDisplay *leftWheelState() { return &_leftWheelState; }
    FlightDisplay *rightWheelState() { return &_rightWheelState; }
    virtual FlightDisplay *getNode(const QString &name);
    FlightDisplay _leftWheelState;
    FlightDisplay _rightWheelState;
};

class VEHICLE_EXPORT SimpleElemList : public QObject {
    Q_OBJECT
    Q_PROPERTY(int count READ count CONSTANT)
    Q_PROPERTY(int selectNode READ selectNode WRITE setSelectNode NOTIFY selectNodeChanged)
    Q_PROPERTY(SimpleElemNode *deftElemNode READ deftElemNode NOTIFY deftElemNodeChanged)
    Q_PROPERTY(FactListModel *dataModel READ dataModel CONSTANT)
    Q_PROPERTY(SimpleElemNode *firstNode READ firstNode CONSTANT)
    Q_PROPERTY(SimpleElemNode *secondNode READ secondNode CONSTANT)
    Q_PROPERTY(SimpleElemNode *thirdNode READ thirdNode CONSTANT)
    Q_PROPERTY(SimpleElemNode *fourNode READ fourNode CONSTANT)
    Q_PROPERTY(SimpleElemNode *fiveNode READ fiveNode CONSTANT)
    Q_PROPERTY(SimpleElemNode *sixNode READ sixNode CONSTANT)
    Q_PROPERTY(SimpleElemNode *sevenNode READ sevenNode CONSTANT)
    Q_PROPERTY(SimpleElemNode *eightNode READ eightNode CONSTANT)

  signals:
    void selectNodeChanged();
    void deftElemNodeChanged();

  public:
    SimpleElemList() = default;

    int selectNode() { return _choose; }

    int count() const { return nodeList.count(); }

    void setSelectNode(int val) {
        int count = nodeList.count();
        _choose = (val % (count <= 0 ? 1 : count));
        emit deftElemNodeChanged();
    }

    SimpleElemNode *deftElemNode() {
        if (nodeList.empty()) {
            return nullptr;
        }

        return nodeList.at(_choose).get();
    }

    FactListModel *dataModel() { return &_dataModel; }

    virtual void doUnitTest(int testval);

    SimpleElemNode *firstNode() {
        if (nodeList.empty()) {
            return nullptr;
        }
        return nodeList.at(0).get();
    }

    SimpleElemNode *secondNode() {
        if (nodeList.size() < 2) {
            return nullptr;
        }
        return nodeList.at(1).get();
    }

    SimpleElemNode *thirdNode() {
        if (nodeList.size() < 3) {
            return nullptr;
        }

        return nodeList.at(2).get();
    }

    SimpleElemNode *fourNode() {
        if (nodeList.empty()) {
            return nullptr;
        }
        return nodeList.at(3).get();
    }

    SimpleElemNode *fiveNode() {
        if (nodeList.size() < 2) {
            return nullptr;
        }
        return nodeList.at(4).get();
    }

    SimpleElemNode *sixNode() {
        if (nodeList.size() < 3) {
            return nullptr;
        }

        return nodeList.at(5).get();
    }

    SimpleElemNode *sevenNode() {
        if (nodeList.empty()) {
            return nullptr;
        }
        return nodeList.at(6).get();
    }

    SimpleElemNode *eightNode() {
        if (nodeList.size() < 2) {
            return nullptr;
        }
        return nodeList.at(7).get();
    }

    template <class Type>
    void ReadModule(const QXlsx::Worksheet *currSheet, QMap<QString, int> &columnStatus,
                    FactDecoderMgr *decoder, int frow, int lrow) {

        if (currSheet == nullptr) {
            COMMON_WARN("invalid sheet param");
            return;
        }

        QString indexBlock("");
        std::shared_ptr<SimpleElemNode> data = nullptr;
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

            QVariant ftype;
            xmx = columnStatus.find(DisplayRefFType);
            if (xmx != columnStatus.end()) {
                int col = xmx.value();
                ftype = currSheet->read(row, col);
            }

            QVariant refData;
            xmx = columnStatus.find(DisplayRefData);
            if (xmx != columnStatus.end()) {
                int col = xmx.value();
                refData = currSheet->read(row, col);
            }

            QVariant dataIndex;
            xmx = columnStatus.find(DisplayDataIndex);
            if (xmx != columnStatus.end()) {
                int col = xmx.value();
                dataIndex = currSheet->read(row, col);
            }

            QVariant dataName;
            xmx = columnStatus.find(DisplayDataName);
            if (xmx != columnStatus.end()) {
                int col = xmx.value();
                dataName = currSheet->read(row, col);
            }

            QVariant colorName;
            xmx = columnStatus.find(DisplayColorSet);
            if (xmx != columnStatus.end()) {
                int col = xmx.value();
                colorName = currSheet->read(row, col);
            }

            QVariant abbrName;
            xmx = columnStatus.find(DisplayModuleAbbr);
            if (xmx != columnStatus.end()) {
                int col = xmx.value();
                abbrName = currSheet->read(row, col);
            }

            FactTableModel *curTable = nullptr;
            if ((decoder != nullptr) && frame.isValid()) {
                curTable = decoder->protoframe(frame.toString());
            }

            if (submodule.isValid()) {
                if (indexBlock.isEmpty() || indexBlock.compare(submodule.toString()) != 0) {
                    data = std::make_shared<Type>();
                    nodeList.append(data);
                    _dataModel.append(data.get());
                    if (abbrName.isValid()) { /// 缩写
                        data->setModuleAbbr(abbrName.toString());
                    }
                    data->setGroupName(submodule.toString());
                }
            }

            FlightDisplay *node = nullptr;
            if (dataIndex.isValid() && !dataIndex.toString().isEmpty()) {
                node = data->getNode(dataIndex.toString());
                if (node == nullptr) {
                    COMMON_WARN("invalid index {}", dataIndex.toString().toStdString().c_str());
                    continue;
                }
            } else {
                node = data->getNode("default");
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

            if (ftype.isValid()) {
                QString tmp = ftype.toString();
                if (tmp.contains("[") && tmp.contains("]")) {
                    node->parseDispaySets(decoder, tmp);
                    if (colorName.isValid()) {
                        node->parseDispayColor(colorName.toString());
                    }
                }
            }

            if ((curTable != nullptr) && (curTable->code() != 0) && refData.isValid()) {
                QString tmp = refData.toString();
                node->insertFactElem(curTable->loadFact(tmp.trimmed().toInt()));
                data->enableRawUpdate();
            }


            COMMON_INFO("frame node {} ", dataName.toString().toLocal8Bit().toStdString());
        }
    }

  protected:
    int _choose = 0;
    FactListModel _dataModel;
    QList<std::shared_ptr<SimpleElemNode>> nodeList;
};
