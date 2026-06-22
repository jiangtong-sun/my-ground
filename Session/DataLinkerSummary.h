#pragma once

#include "ConfigNetCom.h"
#include "VehicleMetadef.h"
#include <QObject>

class DataLinkerSummary : public QObject {
    Q_OBJECT
    VEH_PROPERTY_RW(bool, working)
    VEH_PROPERTY_SETGET(bool, upState)
    VEH_PROPERTY_SETGET(bool, downState)
    VEH_PROPERTY_SETGET(int, quality)
    VEH_PROPERTY_RW(QString, name)
  public:
    DataLinkerSummary();

    void notifyUpdate();

    ///
    /// \brief calculateQuality
    /// \param conf
    ///
    void calculateQuality(NetworkNodeConfig *conf);

    ///
    /// \brief calculateQuality
    /// \param conf
    ///
    void calculateQuality(SerialNodeConfig *conf);

    ///
    /// \brief addRecvFrame
    /// \param datas
    /// \return
    ///
    int addRecvFrame(int datas);

  signals:
    void summaryChanged();

  private:
    uint32_t _recv_frames = 0;
    uint32_t _last_recv_frames = 0;
    uint32_t _last_recv_framedata = 0;
    uint32_t _recv_framedata = 0;
};
