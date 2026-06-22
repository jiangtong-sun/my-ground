#pragma once

#include "FlightStatus.h"
#include "VehicleMetadef.h"

class VEHICLE_EXPORT FlightDevice : public QObject {

    Q_OBJECT
    Q_DISABLE_COPY(FlightDevice)
  public:
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(int selectIndex READ selectIndex WRITE setSelectIndex NOTIFY selectIndexChanged)
    Q_PROPERTY(QString mapGroup READ mapGroup WRITE setMapGroup NOTIFY mapGroupChanged)
    Q_PROPERTY(FlightStatus *selectNode READ selectNode NOTIFY selectNodeChanged)
    Q_PROPERTY(FlightStatus *firstNode READ firstNode CONSTANT)
    Q_PROPERTY(FactListModel *dataModel READ dataModel CONSTANT)
    Q_PROPERTY(QString rawGroupName READ rawGroupName CONSTANT)
    Q_PROPERTY(int elemCount READ elemCount CONSTANT)
    Q_PROPERTY(int count READ count CONSTANT)
    FlightDevice() { nodeList.clear(); }
    ~FlightDevice() = default;

    int count() const { return _dataModel.count(); }
    void addStatus(std::shared_ptr<FlightStatus> node);

    Q_INVOKABLE QString deviceNodeName(int idx);
    Q_INVOKABLE FlightStatus * deviceNode(int idx);

    int selectIndex() { return _select_node; }
    void setSelectIndex(int x);

    FactListModel *dataModel() { return &_dataModel; }

    FlightStatus *find(const QString &name);
    FlightStatus *nodes(const QString &name);
    FlightStatus *selectNode();
    FlightStatus *firstNode();

    bool visible() const { return _visible; }
    bool checked() const { return _checked; }
    int elemCount() const { return _elemCount; }

    QString mapGroup() const { return _mapGroup; }
    QString rawGroupName() const { return _rawGroupName; }

    void setVisible(bool v) {
        _visible = v;
        emit visibleChanged();
    }

    void setChecked(bool v) {
        _checked = v;
        emit checkedChanged();
    }

    void setMapGroup(const QString &displayFrame);

  signals:
    void selectIndexChanged();
    void selectNodeChanged();
    void mapGroupChanged();
    void visibleChanged();
    void checkedChanged();

  protected:
    bool _visible = true;
    bool _checked = false;
    int _elemCount = 0;
    int _select_node = 0;
    QList<std::shared_ptr<FlightStatus>> nodeList;
    FactListModel _dataModel;
    QString _rawGroupName = "";
    QString _mapGroup = "";
};
