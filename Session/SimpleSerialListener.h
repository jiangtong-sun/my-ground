#pragma once

#include "protomessage.h"
#include "networkdata.h"
#include "concurrentqueue.h"
#include "ConfigNetCom.h"
#include "log_category.h"
#include "DataLinkerSummary.h"
#include "../CSerialPort/CSerialPort.h"
#include <QObject>

class ChannelSelection : public QObject {
    Q_OBJECT
public:
    explicit ChannelSelection(QObject *parent = nullptr) : QObject(parent) {}
    enum Options { SerialNetwork = 0, MainNetwork = 1, SlaveNetwork = 2, AllNetwork = 0xF };
    Q_ENUM(Options)
};

class GcsStationRole : public QObject {
    Q_OBJECT
public:
    explicit GcsStationRole(QObject *parent = nullptr) : QObject(parent) {}
    enum Options { ControlRole = 0x01, TaskRole = 0x02 };
    Q_ENUM(Options)
};


class SimpleSerialListener : public itas109::CSerialPortListener {
public:
    SimpleSerialListener(itas109::CSerialPort *sp, moodycamel::ConcurrentQueue<ProtoMessage *> &cache)
        : serialPort(sp), sessioncache(cache){};
    void onReadEvent(const char *portName, unsigned int readBufferLen) ;

private:
    uint64_t serial_datalen = 0;
    ProtoDataCache serial_cache;
    moodycamel::ConcurrentQueue<ProtoMessage *> &sessioncache;
    itas109::CSerialPort *serialPort = nullptr;
};

using SerailMsgCallback = std::function<void(const char *portName, unsigned int readBuffer)>;

class SimpleSerialListenerExt : public itas109::CSerialPortListener {
public:
    SimpleSerialListenerExt(itas109::CSerialPort *sp, SerailMsgCallback cache)
        : serialPort(sp), sessioncache(cache){};
    void onReadEvent(const char *portName, unsigned int readBufferLen) ;

private:
    uint64_t serial_datalen = 0;
    ProtoDataCache serial_cache;
    SerailMsgCallback sessioncache;
    itas109::CSerialPort *serialPort = nullptr;
};

template<class Type>
void writeSerailData(itas109::CSerialPort &main_serail, Type * nodeProto,
                     DataLinkerSummary & summary, int & lastWriteCount) {
    assert(nodeProto != nullptr);
    if (nodeProto == nullptr) {
        return;
    }
    int rtn = main_serail.writeData((char *) nodeProto, sizeof(*nodeProto));
    COMMON_INFO("serail try write proto, count: {}, result: {}, expect: {}",
                lastWriteCount, rtn, sizeof(*nodeProto));
    if (rtn == sizeof(nodeProto)) {
        summary.upState(true);
        lastWriteCount = 0;
        delete nodeProto;
    }
}

bool openSerialService(itas109::CSerialPort &main_serail, SimpleSerialListener &listener,
                              SerialNodeConfig *configCtrl);
