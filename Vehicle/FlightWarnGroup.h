#pragma once

#include "FlightWarning.h"
#include "VehicleMetadef.h"
#include "factlistmodel.h"
#include <memory>

class VEHICLE_EXPORT FlightWarnGroup : public QObject {
    Q_OBJECT
    VEH_PROPERTY_RW(int, warnTypes)
    Q_PROPERTY(FactListModel *warnGroup READ warnGroup CONSTANT)
  public:
    FlightWarnGroup() {}
    FactListModel *warnGroup(void) { return &_warnGroup; }
    bool addWarnGroup(int priority, int level, const QString &warnGroup);
    void calculateWarnGroup();

  private:
    uint64_t warn_updatefreq = 0;
    FactListModel _warnGroup;
    QList<std::shared_ptr<WarnElemNode>> warnGroupCache;
};
