#pragma once

#include "FlightDisplay.h"
#include "VehicleMetadef.h"
#include "factlistmodel.h"
#include <memory>

class VEHICLE_EXPORT FlightStatus : public QObject {

    Q_OBJECT
    Q_DISABLE_COPY(FlightStatus)

  public:
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(QString mapGroup READ mapGroup WRITE setMapGroup NOTIFY mapGroupChanged)
    Q_PROPERTY(FactListModel *firstPart READ firstPart CONSTANT)
    Q_PROPERTY(FactListModel *secondPart READ secondPart CONSTANT)
    Q_PROPERTY(FactListModel *thridPart READ thridPart CONSTANT)
    Q_PROPERTY(FactListModel *fourPart READ fourPart CONSTANT)
    Q_PROPERTY(FactListModel *fivePart READ fivePart CONSTANT)
    Q_PROPERTY(FactListModel *sixPart READ sixPart CONSTANT)
    Q_PROPERTY(FactListModel *dataModel READ dataModel CONSTANT)
    Q_PROPERTY(FactListModel *getAllMode READ getAllMode CONSTANT)
    Q_PROPERTY(QStringList subGroupStr READ subGroupStr CONSTANT)
    Q_PROPERTY(QVariantMap countGroupElem READ countGroupElem CONSTANT)
    Q_PROPERTY(QString vertGroup READ vertGroup CONSTANT)
    Q_PROPERTY(QString modelFile READ modelFile CONSTANT)
    Q_PROPERTY(int count READ count CONSTANT)

    FlightStatus() = default;
    ~FlightStatus() = default;

    int count() { return nodeList.size(); }

    FactListModel *navModel(void);
    FactListModel *otherModel(void);
    FactListModel *firstPart();
    FactListModel *secondPart();
    FactListModel *thridPart();
    FactListModel *fourPart();
    FactListModel *fivePart();
    FactListModel *sixPart();

    FactListModel *getAllMode() { return &allDataModel; }

    void addDefault(std::shared_ptr<FlightDisplay>);
    void addGroup(std::shared_ptr<FlightDisplay>, const QString &);

    QStringList subGroupStr() { return _subGroupStr; }
    QVariantMap countGroupElem() { return _countGroupElem; }

    FactListModel *subModal(const QString &sub);
    FactListModel *dataModel() { return &_dataModel[0]; }
    FlightDisplay *first(const QString &) const;

    bool visible() const { return _visible; }
    bool checked() const { return _checked; }

    QString modelFile() const { return _model_file; }
    QString mapGroup() const { return _mapGroup; }
    QString vertGroup() const { return _vertGroupName; }

    void setVisible(bool v) { _visible = v; }
    void setChecked(bool v) { _checked = v; }
    void setMapGroup(const QString &val);
    void setMadelFile(const QString &val);

  signals:
    void mapGroupChanged();
    void visibleChanged();
    void checkedChanged();

  protected:
    bool _visible = false;
    bool _checked = false;
    QList<std::shared_ptr<FlightDisplay>> nodeList;
    FactListModel allDataModel;
    FactListModel _dataModel[10];
    QStringList _subGroupStr;
    QVariantMap _countGroupElem;
    QString _vertGroupName = "";
    QString _model_file = "";
    QString _mapGroup = "";
};
