#include "prototelemetry.h"
#include "log_category.h"

#include <QNetworkInterface>
#include <QTime>

#pragma comment(lib,"ws2_32.lib")

int Channel::Send(char *data, int size, OnSentCallback send_cb){
    QHostAddress multitxaddr(QString::fromStdString(configmain.multiTxAddr));
    int res = sock_read.writeDatagram((char *) data, size, multitxaddr, configmain.multiTxPort);
    if (send_cb != nullptr) {
        send_cb(res, data, size);
    }
    return res;
}

bool Channel::Initialize(const ChannelConfig &conf, LoopHandle *loop_handle, bool loop){
    configmain = conf;
    return true;
}

bool Channel::CreateEx(LoopHandle *loop_handle){
    QVariant tt(0);
    QHostAddress address1(QString::fromStdString(configmain.address));
    QHostAddress multitxaddr(QString::fromStdString(configmain.multiTxAddr));
    QNetworkInterface targetInterface;
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : interfaces) {
        QList<QNetworkAddressEntry> entries = iface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
            if (entry.ip() == address1) {
                targetInterface = iface;
                break;
            }
        }
        if (targetInterface.isValid()) {
            break;
        }
    }
    sock_write.bind(address1, configmain.multiTxPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    sock_write.setSocketOption(QAbstractSocket::MulticastLoopbackOption, tt);
    sock_write.joinMulticastGroup(multitxaddr, targetInterface);
    COMMON_INFO("data trans {}:{} -> {}", configmain.address, configmain.multiTxPort, configmain.multiTxAddr);
    memset(&rw_multiTx_addr, 0, sizeof(rw_multiTx_addr));  //每个字节都用0填充
    rw_multiTx_addr.sin_family = AF_INET;
    rw_multiTx_addr.sin_addr.s_addr = inet_addr(configmain.multiTxAddr.c_str());  //具体的IP地址
    rw_multiTx_addr.sin_port = htons(configmain.multiTxPort);  //端口号
    return true;
}

bool Channel::Create(LoopHandle *loop_handle){
    QVariant tt(1);
    QHostAddress address1(QString::fromStdString(configmain.address));
    QHostAddress multirxaddr(QString::fromStdString(configmain.multiRxAddr));
    QHostAddress multitxaddr(QString::fromStdString(configmain.multiTxAddr));
    QNetworkInterface targetInterface;
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : interfaces) {
        QList<QNetworkAddressEntry> entries = iface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
            if (entry.ip() == address1) {
                targetInterface = iface;
                break;
            }
        }
        if (targetInterface.isValid()) {
            break;
        }
    }
    sock_read.bind(address1, configmain.multiRxPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    sock_read.setSocketOption(QAbstractSocket::MulticastLoopbackOption, tt);
    sock_read.joinMulticastGroup(multirxaddr, targetInterface);
    COMMON_INFO("data trans {}:{} -> {}", configmain.address, configmain.multiRxPort, configmain.multiRxAddr);
    sock_write.bind(address1, configmain.multiTxPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    sock_write.setSocketOption(QAbstractSocket::MulticastLoopbackOption, tt);
    sock_write.joinMulticastGroup(multitxaddr, targetInterface);
    COMMON_INFO("data trans {}:{} -> {}", configmain.address, configmain.multiTxPort, configmain.multiTxAddr);
    memset(&rw_multiRx_addr, 0, sizeof(rw_multiRx_addr));  //每个字节都用0填充
    memset(&rw_multiTx_addr, 0, sizeof(rw_multiTx_addr));  //每个字节都用0填充
    rw_multiRx_addr.sin_family = AF_INET;
    rw_multiRx_addr.sin_addr.s_addr = inet_addr(configmain.multiRxAddr.c_str());  //具体的IP地址
    rw_multiRx_addr.sin_port = htons(configmain.multiRxPort);  //端口号
    rw_multiTx_addr.sin_family = AF_INET;
    rw_multiTx_addr.sin_addr.s_addr = inet_addr(configmain.multiTxAddr.c_str());  //具体的IP地址
    rw_multiTx_addr.sin_port = htons(configmain.multiTxPort);  //端口号
    return true;
}

bool Channel::Destory(){
    if(sock_read.isOpen()) {
        sock_read.close();
    }
    if(sock_write.isOpen()) {
        sock_write.close();
    }
    return true;
}

FeedbackPositionInfo &GetFeedbackPositionInfo() {
    static FeedbackPositionInfo FeedPosInfo;
    return FeedPosInfo;
}

ProtoDecoderInfo &GetProtoDecoderInfo() {
    static ProtoDecoderInfo DecoderInfo;
    return DecoderInfo;
}

uint16_t AircraftAddress(ControlAdjustProto *proto) {
    assert(proto != nullptr);
    return proto->dst_addr;
}

void FillProtoTime(VehicleMissionProto *proto) {
    QTime curtime = QTime::currentTime();
    uint32_t tm = (curtime.hour() << 19) | (curtime.minute() << 13) |
                  (curtime.second() * 100 + curtime.msec() / 10);
    proto->hour = (tm >> 16) & 0xff;
    proto->minute = (tm >> 8) & 0xff;
    proto->second = (tm >> 0) & 0xff;
}

void FillProtoTime(PtrRockerMessage proto) {
    QTime curtime = QTime::currentTime();
    uint32_t tm = (curtime.hour() << 19) | (curtime.minute() << 13) |
                  (curtime.second() * 100 + curtime.msec() / 10);
    proto->hour = (tm >> 16) & 0xff;
    proto->minute = (tm >> 8) & 0xff;
    proto->second = (tm >> 0) & 0xff;
}

void FillProtoTime(ControlAdjustProto * proto) {
    QTime curtime = QTime::currentTime();
    uint32_t tm = (curtime.hour() << 19) | (curtime.minute() << 13) |
                  (curtime.second() * 100 + curtime.msec() / 10);
    // proto->hour = (tm >> 16) & 0xff;
    // proto->minute = (tm >> 8) & 0xff;
    // proto->second = (tm >> 0) & 0xff;
}

QString ToHexStr(const uint8_t *addr, uint8_t len) {
    static const char SHex[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    int iy = 0;
    char buffer[512] = {0};
    for (int iy = 0; iy < len; iy++) {
        buffer[3 * iy + 0] = SHex[addr[iy] / 16];
        buffer[3 * iy + 1] = SHex[addr[iy] % 16];
        buffer[3 * iy + 2] = ' ';
    }
    return QString::fromStdString(buffer);
}

uint8_t ProtoCRC8Ext(const uint8_t *addr, uint8_t len, bool set){
    uint8_t *end = (uint8_t *) addr + len;
    uint8_t *p = (uint8_t *) addr;
    uint16_t crc8 = 0;
    while (p < end) {
        crc8 += *p++;
        crc8 = crc8 % 256;
        len -= 1;
    }

    if (set) {
        *--p = crc8;
        return *p;
    }
    return static_cast<uint8_t>(crc8);
}

uint8_t ProtoCRC8(const uint8_t *addr, uint8_t len, bool set) {
    uint8_t *end = (uint8_t *) addr + len;
    uint8_t *p = (uint8_t *) addr;
    uint16_t crc8 = 0;
    while (p < end) {
        crc8 += *p++;
        crc8 = crc8 % 256;
        len -= 1;
    }
    if (set) {
        *--p = (256 - crc8);
        return *p;
    }
    return static_cast<uint8_t>(crc8);
}
