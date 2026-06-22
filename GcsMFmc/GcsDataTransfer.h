#pragma once

#include "ConfigNetCom.h"
#include "GcsSimpleTcpClient.h"
#include "VehicleCommand.h"
#include "factdecodermgr.h"
#include "uvloop.h"
#include <QUdpSocket>
#include <QVector>

class GcsDataTransfer {

  public:
    GcsDataTransfer();
    ~GcsDataTransfer();
    void enableDataTranfer(bool);
    bool parseTransFile(const QString &file);
    void transferProto(FactDecoderMgr *, int freq, int bigLittle, int chl);
    void enableNetworkService(LoopHandle *, NetworkNodeConfig *ext);
    void stopNetworkService();

  protected:
    double calResolution(const QString &value);
    int assignProto(MetaValueType, uint8_t *buffer, int bigLittle, double input);

    struct ColumnName {
        QString name;
        int column;
    };

    Channel socket_simple;

    uint64_t next_trans_ms = 0;
    std::atomic_bool enable_mode = false;
    QVector<ColumnName> excelTableHeader;
    QMap<QString, MetaValueType> datatypeMap;
    QVector<TypeMetaData *> proto_datatype;
};

using RtkAvailCallback = std::function<bool()>;

class RTKDataTranfer {
  public:
    RTKDataTranfer();
    ~RTKDataTranfer();

    ///
    /// \brief enableDataTranfer
    ///
    void enableDataTranfer(bool);
    ///
    /// \brief tranferProto
    /// \param mainNet
    /// \param slaveNet
    /// \param queue
    /// \param ext
    ///
    void tranferProto(Channel *mainNet, Channel *slaveNet, RockerProtoQueue &queue,
                      NetworkNodeConfig *ext);

    ///
    /// \brief enableNetworkService
    /// \param rtkAvail
    /// \param call
    /// \param ext
    /// \return
    ///
    bool enableNetworkService(RtkAvailCallback rtkAvail, CallNumberCallback call, LoopHandle *,
                              NetworkNodeConfig *ext, NetworkNodeConfig *main,
                              NetworkNodeConfig *slave);

    ///
    /// \brief stopNetworkService
    ///
    void stopNetworkService();

  private:
    void tranferProtoMain(RockerDeviceProto *, Channel *mainNet);
    void tranferProtoSlave(RockerDeviceProto *, Channel *slaveNet);
    void tranferProto(Channel *mainNet, PtrRockerMessage, NetworkNodeConfig *extConf);
    bool multiNetworkService(LoopHandle *, NetworkNodeConfig *ext, RtkAvailCallback rtkAvail);
    bool tcpNetworkService(LoopHandle *, NetworkNodeConfig *ext, RtkAvailCallback rtkAvail);
    void cacheTransData(RtkAvailCallback rtkAvail, const char *buffer, int length);

  protected:
    uint8_t _oprint_freq = 0;
    uint8_t _cmd_number = 0;
    uint32_t _totalNumber = 0;
    uint16_t _rtk_dst_port = 0;
    std::string _rtk_dst_addr;
    std::atomic_bool enable_mode;
    RtkAvailCallback rtk_avail_func = nullptr;
    CallNumberCallback call_number = nullptr;
    //UdpSocketSimple rtk_read_write;
    SimpleTcpClient rtk_rw_client;
    // QUdpSocket udpsocket_rtkmain;
    // QUdpSocket udpsocket_rtkslave;
    NetworkNodeConfig *main_config;
    NetworkNodeConfig *slave_config;
    RockerProtoQueue rtk_write_cache;
};
