/**
 * @file factdecodermgr.cpp
 * @brief 车辆协议解析核心管理类实现文件
 * @details 实现FactDecoderMgr类的所有成员函数，包含协议解析、数据解码、QML类型注册、
 *          单元测试、网络模拟、告警解析等核心逻辑，适配多线程并发和跨平台场景
 * @note 依赖Qt网络模块(QUdpSocket)、并发队列(moodycamel::ConcurrentQueue)、
 *       协议解析工具类(ParseProtoFile)等组件
 */

#include "factdecodermgr.h"
#include "CommandGroup.h"
#include "CommandItem.h"
#include "adjustControl.h"
#include "log_category.h"
#include "prototelemetry.h"
#include <QFile>
#include <QUdpSocket>
#include <uv.h>

/**
 * @brief 构造函数
 * @param idx 车辆索引（uint8_t，0~255，车辆唯一标识）
 * @param tm 协议文件时间戳（标记协议文件的生效时间）
 * @details 初始化车辆索引、协议文件时间、车辆地址等核心成员变量，
 *          车辆地址默认初始化为0，后续在协议解析时从帧数据中填充
 */
FactDecoderMgr::FactDecoderMgr(uint8_t idx, QDateTime tm) : vehicleIndex(idx), file_datatime(tm), vehicleAddr(0) {}

/**
 * @brief 注册自定义类型到QML引擎
 * @details 1. 注册元类型（qRegisterMetaType）使Qt元对象系统识别自定义结构体；
 *          2. 注册不可创建类型（qmlRegisterUncreatableType）到QML上下文，
 *             供QML端只读访问（不能实例化），实现C++类型与QML的交互；
 *          3. 注册的类型涵盖告警节点、命令组、数据模型等核心业务类型
 */
void FactDecoderMgr::registerType() {
    qRegisterMetaType<WarnConfNode>();
    qRegisterMetaType<FactMetaData>();
    qRegisterMetaType<WarnElemNode>();
    qmlRegisterUncreatableType<WarnElemNode>("WarnElemNode", 1, 0, "WarnElemNode", "Reference");
    qmlRegisterUncreatableType<RCCmdSubGroup>("RCCmdSubGroup", 1, 0, "RCCmdSubGroup", "Reference");
    qmlRegisterUncreatableType<RCCommandGroup>("RCCommandGroup", 1, 0, "RCCommandGroup", "Reference");
    qmlRegisterUncreatableType<RCCommandGroupExt>("RCCommandGroupExt", 1, 0, "RCCommandGroupExt", "Reference");
    qmlRegisterUncreatableType<AdjustControl>("AdjustControl", 1, 0, "AdjustControl", "Reference");
    qmlRegisterUncreatableType<TransMetaData>("TransMetaData", 1, 0, "TransMetaData", "Reference");
    qmlRegisterUncreatableType<RCCommandItem>("RCCommandItem", 1, 0, "RCCommandItem", "Reference");
    qmlRegisterUncreatableType<RCAdjustItem>("RCAdjustItem", 1, 0, "RCAdjustItem", "Reference");
    qmlRegisterUncreatableType<RCAdjustGroup>("RCAdjustGroup", 1, 0, "RCAdjustGroup", "Reference");
    qmlRegisterUncreatableType<FactTableModel>("FactTableModel", 1, 0, "FactTableModel", "Reference");
    qmlRegisterUncreatableType<FactListModel>("FactListModel", 1, 0, "FactListModel", "Reference");
    qmlRegisterUncreatableType<WarnConfigExt>("WarnConfigExt", 1, 0, "WarnConfigExt", "Reference");
    qmlRegisterUncreatableType<WarnConfigGroupKey>("WarnConfigGroupKey", 1, 0, "WarnConfigGroupKey", "Reference");
    qmlRegisterUncreatableType<WarnConfigGroupVal>("WarnConfigGroupVal", 1, 0, "WarnConfigGroupVal", "Reference");
}

/**
 * @brief 析构函数
 * @details 空实现，因成员变量多为智能指针/Qt父子对象管理，自动释放资源；
 *          若后续有手动分配的资源（如文件句柄、网络连接），需在此释放
 */
FactDecoderMgr::~FactDecoderMgr() {}

/**
 * @brief 关闭所有通道的协议数据保存功能
 * @details 1. 设置退出标志exitflag（原子变量，多线程安全），通知数据保存线程停止；
 *          2. 遍历所有通道的FactTableModel，调用closeFileSave关闭文件保存；
 *          3. veh_proto_set为QMap<int, shared_ptr<FactTableModel>>，存储各通道数据模型
 */
void FactDecoderMgr::closeFactSave() {
    exitflag = true;
    //begin() 返回的迭代器指向第一个 <通道号, 数据模型智能指针> 键值对
    auto mx = veh_proto_set.begin();
    while (mx != veh_proto_set.end()) {
        //mx.value()：返回std::shared_ptr<FactTableModel>（共享智能指针，指向FactTableModel类实例）
        //closeFileSave()：FactTableModel类的公有成员函数（无参数、无返回值）
        mx.value()->closeFileSave();
        mx++;
    }
}

void FactDecoderMgr::enableFactSave(int chl) {
    auto mx = veh_proto_set.begin();
    while (mx != veh_proto_set.end()) {
        mx.value()->resetFilename(chl);
        mx++;
    }
}

void FactDecoderMgr::dataPackage(const void *addr, const uint8_t *buf, int len, int channel,
                                 moodycamel::ConcurrentQueue<ProtoMessage *> &cache) {
    struct sockaddr_in *addrsock = (struct sockaddr_in *) (addr); // check message
    COMMON_TRACE("data package {}:{}", addrsock ? addrsock->sin_port : 0, len);
    parseProtoData(buf, len, channel, addrsock->sin_addr.S_un.S_addr, addrsock->sin_port, cache);
}

void FactDecoderMgr::dataDecoder(ProtoDataCache &decoder, int channel,
                                 moodycamel::ConcurrentQueue<ProtoMessage *> &cacheQueue) {
    dataDecoder(decoder, nullptr, channel, cacheQueue);
}

QByteArray FactDecoderMgr::unitTest(const QString &filename, uint32_t row) {
    QFile file(filename);
    QByteArray rtn;

    if (file.open(QIODevice::ReadOnly)) {
        int ix = 0;
        while (ix++ < row) {
            rtn = file.readLine();
        }

        file.close();
    }

    return rtn;
}

void FactDecoderMgr::initSummary(TelemetrySummary summary) {
    assert(summary != nullptr);
    summary_telemetry = summary;
}

void FactDecoderMgr::decodeWarnData(WarnConfig *warnConf, FactTableModel *table, WarningLevel warn,
                                    CheckWarnCallback first, int chl) {

    int checkVal = warnConf->_checkValue[warn];
    FactMetaData *frameData = table->loadFact(warnConf->_frameItem);
    assert(frameData != nullptr);
    if (frameData == nullptr) {
        COMMON_WARN("unknow frame {}", warnConf->_frameItem);
        return;
    }

    FactTableModel *frame = protoframe((warnConf->_referFrame >> 16) & 0xff);
    assert(frame != nullptr);
    if (frame == nullptr) {
        COMMON_WARN("unknow frame {}", warnConf->_referFrame);
        return;
    }

    FactMetaData *meta = frame->loadFact((warnConf->_referFrame >> 8) & 0xff);
    assert(meta != nullptr);
    if (meta == nullptr) {
        COMMON_WARN("unknow frame row {}", warnConf->_referFrame);
        return;
    }

    int val = frameData->rawValue(chl);
    QString timestamp = frame->currTimestamp(chl);
    if (meta->rawValue(chl) && ((checkVal & 0xff) == val) && (first != nullptr)) {
        std::shared_ptr<WarnElemNode> warnNode(new (WarnElemNode));
        warnNode->group(warnConf->_warnString[warn]);
        warnNode->level(warn);
        warnNode->name(frameData->name());
        warnNode->timestamp(timestamp);
        warnNode->frame(frame->name());
        warnNode->priority(warnConf->_priorityValue[0]);
        first(warnNode);
    }
}

void FactDecoderMgr::notifyDataUpdate(CheckWarnCallback first, int chl) {
    auto mx = veh_proto_set.begin();
    while (mx != veh_proto_set.end()) {
        mx.value()->flushModelData(first, chl);
        mx++;
    }

    auto bx = warnings.begin();
    while (bx != warnings.end()) {
        WarnConfig *config = bx->get();
        if ((config != nullptr) && (config->_referFrame > 0)) {
            FactTableModel *model = protoframe(config->_frameCode);
            decodeWarnData(config, model, LevelOne, first, chl);
        }
        ++bx;
    }
}

bool FactDecoderMgr::protoDecoder(ProtoMessage *msg) {
    ProtoDecoderInfo &conf = GetProtoDecoderInfo();
    if (msg == nullptr || msg->length < conf.ProtoSinglePackage) {
        COMMON_WARN("discard unknown data, length {}", msg ? msg->length : 0);
        return true;
    }

    uint8_t *buffer = msg->buffer;
    uint16_t *type1 = (uint16_t *) (buffer + conf.FirstProtoTypeOffset);
    uint16_t *vechAddr = (uint16_t *) (buffer + conf.VehicleLabelAddr);
    if (vehicleAddr == 0) {
        vehicleAddr = *vechAddr;
    }

    int selChl = msg->channel;
    selChl = (selChl << 8) | msg->select;
    auto get1 = veh_proto_set.find(*type1);
    if (get1 != veh_proto_set.end()) {
        /// 解析数据
        get1.value()->decoder(buffer, msg->length, selChl,
                              [&](ReferenceValueCmp *refer, int chl) { return referDeviceState(refer, chl); });
        /// 保存数据
        get1.value()->dataFileSave(0, msg->channel, msg->select, conf.ProtoTimestampOffset);
        /// 统计获取的帧数量
        if (summary_telemetry != nullptr) {
            summary_telemetry(vehicleIndex, msg->length, *type1);
        }
    }

    return true;
}

bool FactDecoderMgr::referDeviceCondition(ReferenceDevice *refer, int chl) {
    assert(refer != nullptr);
    if (refer == nullptr) {
        return false;
    }

    auto get1 = veh_proto_set.find(refer->frame);
    if (get1 != veh_proto_set.end()) {
        FactMetaData *elem = get1.value()->at(refer->item);
        assert(elem != nullptr);
        if (elem == nullptr) {
            return false;
        }
        if (elem->rawValue(chl) == refer->expect) {
            return true;
        }
    }

    return false;
}

bool FactDecoderMgr::referDeviceState(ReferenceValueCmp *refer, int chl) {
    assert(refer != nullptr);
    if (refer == nullptr) {
        return false;
    }

    ReferenceDevice &ref = refer->_ref[0];
    if (refer->_operate == InvalidOperate) {
        return referDeviceCondition(&refer->_ref[0], chl);

    } else if (refer->_operate == AndOperate) {
        bool ref1 = referDeviceCondition(&refer->_ref[0], chl);
        bool ref2 = referDeviceCondition(&refer->_ref[1], chl);
        return ref1 && ref2;

    } else if (refer->_operate == OrOperate) {
        bool ref1 = referDeviceCondition(&refer->_ref[0], chl);
        bool ref2 = referDeviceCondition(&refer->_ref[1], chl);
        return ref1 || ref2;
    }

    return false;
}

void FactDecoderMgr::dataDecoder(ProtoDataCache &decoder, const void *addr, int channel,
                                 moodycamel::ConcurrentQueue<ProtoMessage *> &datacache) {
    int countPackage = 0;
    int uncheck = decoder.lastPos;
    ProtoDecoderInfo &conf = GetProtoDecoderInfo();
    struct sockaddr_in *addrsock = (struct sockaddr_in *) (addr); // check message
    COMMON_TRACE("data package {}:{}", addrsock ? addrsock->sin_port : 0, decoder.length);

    while ((uncheck < decoder.length) && (uncheck + conf.ProtoSinglePackage <= decoder.length)) {
        if ((decoder.data[uncheck + 0] == conf.ProtoSplitWordA) &&
            (decoder.data[uncheck + 1] == conf.ProtoSplitWordB)) {
            decoder.lastPos = uncheck; // current data pointer
            ControlFrameBase *frameBase = (ControlFrameBase *) (decoder.data + uncheck);
            if ((frameBase->length > 0) && ((decoder.length - uncheck) >= frameBase->length) &&
                (ProtoCRC8(&decoder.data[uncheck], frameBase->length, false) == 0)) {
                parseProtoData(decoder.data + uncheck, frameBase->length, channel,
                               addrsock ? addrsock->sin_addr.S_un.S_addr : 0, addrsock ? addrsock->sin_port : 0,
                               datacache);
                decoder.lastPos = uncheck + frameBase->length;
                uncheck += frameBase->length;
                ++countPackage;
            } else {
                uncheck += 1;
            }
        } else {
            uncheck += 1;
        }
    }

    if (decoder.lastPos > 0) {
        if (decoder.length >= decoder.lastPos) {
            uint16_t sxx = decoder.length - decoder.lastPos;
            COMMON_TRACE("package {}, length: {}, last: {}", countPackage, decoder.length, decoder.lastPos);
            memmove(decoder.data, decoder.data + decoder.lastPos, sxx);
            decoder.length = sxx;
            decoder.lastPos = 0;
        } else {
            COMMON_WARN("reset cache data, length: {}, last: {}", decoder.length, decoder.lastPos);
            decoder.length = 0;
            decoder.lastPos = 0;
        }
    }

    if (decoder.length > conf.ProtoPackageLength * 8) {
        COMMON_WARN("discard data, length {}", decoder.length);
        decoder.length = 0;
        decoder.lastPos = 0;
    }
}

bool FactDecoderMgr::parseProtoData(const uint8_t *buffer, int size, int channel, int addr, int port,
                                    moodycamel::ConcurrentQueue<ProtoMessage *> &cache) {
    if (buffer == nullptr || size <= 2) {
        return false;
    }

    ProtoMessage *message = newProtoMessage(channel, buffer, size);
    if (message != nullptr) {
        message->address = addr;
        message->port = port;
        cache.enqueue(message);
        return true;
    }

    COMMON_WARN("allocate memory {} failed", size);
    return false;
}

FactTableModel *FactDecoderMgr::protoframe(int type) const {
    auto mx = veh_proto_set.find(type);
    if (mx != veh_proto_set.end()) {
        return mx.value().get();
    }
    return nullptr;
}

FactTableModel *FactDecoderMgr::protoframe(const QString &name) const {
    auto mx = veh_proto_set.begin();
    while (mx != veh_proto_set.end()) {
        QString tname = mx.value()->name();
        if (tname.contains(name)) {
            return mx.value().get();
        }
        mx++;
    }
    return nullptr;
}

void FactDecoderMgr::unitTestProto(int frame, int ctxrand) {

#if defined(AFXF_PROJECT_CONF)
    uint8_t buffer[128] = {0};
    memset(buffer, ctxrand, sizeof(buffer));
    ProtoDecoderInfo &conf = GetProtoDecoderInfo();
    uint16_t *type1 = (uint16_t *) (buffer + conf.FirstProtoTypeOffset);
    buffer[63] = 0; //
    *type1 = frame;
    ProtoCRC8(buffer, 64, true);
    ProtoMessage *msg = newProtoMessage(0, buffer, 64);
    protoDecoder(msg);
    delete msg;
#elif defined(WHXQ_PROJECT_CONF)
    uint8_t buffer[128] = {0};
    memset(buffer, ctxrand, sizeof(buffer));
    ProtoDecoderInfo &conf = GetProtoDecoderInfo();
    uint16_t *type1 = (uint16_t *) (buffer + conf.FirstProtoTypeOffset);
    buffer[63] = 0; //
    *type1 = frame;
    ProtoCRC8(buffer, 64, true);
    ProtoMessage *msg = newProtoMessage(0, buffer, 64);
    protoDecoder(msg);
#endif
}

void FactDecoderMgr::unitTestDecoder(uint8_t ctxrand, int chl) {
    for (auto x = veh_proto_set.begin(); x != veh_proto_set.end(); ++x) {
        uint8_t buffer[128] = {0};
        memset(buffer, ctxrand, 64);
        int sz = x.value()->decoder(buffer, 64, ((chl << 8) | chl), nullptr);
        if (sz != 64) { // verify decoder result....
            COMMON_WARN("decoder expect {}, ectual {}", 64, sz);
        }
    }
}

void FactDecoderMgr::unitTestNetwork(int chl, ChannelConfig &configmain) {

    std::thread testThread([=]() {
        // QVariant tt(1);
        QUdpSocket unit_udpsocket;
        QHostAddress group(QString::fromStdString(configmain.multiRxAddr));
        QHostAddress address(QString::fromStdString(configmain.address));
        unit_udpsocket.bind(address, configmain.multiRxPort + 100,
                            QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
        // unit_udpsocket.setSocketOption(QAbstractSocket::MulticastLoopbackOption, tt);
        unit_udpsocket.joinMulticastGroup(group);
        COMMON_INFO("socket test {} {} -> {}:{}", chl, configmain.address, configmain.multiRxAddr,
                    configmain.multiRxPort);
        for (int ix = 0; ix < 1000000 && !exitflag; ix++) {
            if (chl > 0) {
                for (auto x = veh_proto_set.begin(); x != veh_proto_set.end(); ++x) {
                    uint8_t buffer[128] = {0};
                    memset(buffer, rand(), sizeof(buffer));
                    buffer[0] = 0xeb;
                    buffer[1] = 0x90;
                    buffer[2] = 64;
                    ProtoDecoderInfo &conf = GetProtoDecoderInfo();
                    uint16_t *type1 = (uint16_t *) (buffer + conf.FirstProtoTypeOffset);
                    uint16_t *vechAddr = (uint16_t *) (buffer + conf.VehicleLabelAddr);
                    *vechAddr = 1;
                    *type1 = x.key();
                    buffer[63] = 0; // crc
                    ProtoCRC8(buffer, 64, true);
                    unit_udpsocket.writeDatagram((char *) buffer, 64, group, configmain.multiRxPort);
                    // COMMON_INFO("mock data {} {} ==> {}", configmain.multiRxPort, *type1,
                    // buffer[3]);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } else {
                for (auto x = veh_proto_set.begin(); x != veh_proto_set.end(); ++x) {
                    uint8_t buffer[128] = {0};
                    memset(buffer, ix % 256, sizeof(buffer));
                    buffer[0] = 0xeb;
                    buffer[1] = 0x90;
                    buffer[2] = 64;
                    buffer[3] = ix % 256;
                    ProtoDecoderInfo &conf = GetProtoDecoderInfo();
                    uint16_t *type1 = (uint16_t *) (buffer + conf.FirstProtoTypeOffset);
                    uint16_t *vechAddr = (uint16_t *) (buffer + conf.VehicleLabelAddr);
                    *vechAddr = 1;
                    *type1 = x.key();
                    buffer[63] = 0; // crc
                    ProtoCRC8(buffer, 64, true);
                    unit_udpsocket.writeDatagram((char *) buffer, 64, group, configmain.multiRxPort);
                    // COMMON_INFO("mock data {} {} ==> {}", configmain.multiRxPort, *type1,
                    // buffer[3]);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    });
    testThread.detach();
}

void FactDecoderMgr::unitTestDecoder(int ctxrand, uint8_t jump, int type, ChannelConfig &config) {
    QUdpSocket unit_udpsocket;
    QHostAddress group(QString::fromStdString(config.multiRxAddr));
    QHostAddress address(QString::fromStdString(config.address));
    unit_udpsocket.bind(address, config.multiRxPort + 100, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    unit_udpsocket.joinMulticastGroup(group);
    COMMON_INFO("socket test {} -> {}:{}", config.address, config.multiRxAddr, config.multiRxPort);

    jump = jump % 4;
    type = type % 2;
    for (auto x = veh_proto_set.begin(); x != veh_proto_set.end(); ++x) {
        uint8_t buffer[256] = {0};
        uint8_t *pbuffer = (uint8_t *) (buffer);

        if (jump == 2) {
            uint16_t *tmp = (uint16_t *) (buffer + (type > 0 ? 1 : 0));
            for (int im = 0; im < 250; im += 2) {
                *tmp = ctxrand;
                ++tmp;
            }
        } else if (jump == 3) {
            uint32_t *tmp = (uint32_t *) (buffer);
            for (int im = 0; im < 250; im += 4) {
                *tmp = ctxrand;
                ++tmp;
            }
        } else {
            memset(buffer, ctxrand % 256, 256);
        }

        ProtoDecoderInfo &conf = GetProtoDecoderInfo();
        for (int ix = 0; (ix + conf.ProtoPackageLength) < 256; ix += conf.ProtoPackageLength) {
            pbuffer[0] = 0xeb;
            pbuffer[1] = 0x90;
            pbuffer[2] = conf.ProtoPackageLength;
            pbuffer[3] = ix % 256;
            uint16_t *type1 = (uint16_t *) (pbuffer + conf.FirstProtoTypeOffset);
            uint16_t *vechAddr = (uint16_t *) (pbuffer + conf.VehicleLabelAddr);
            *vechAddr = 1;
            *type1 = x.key();
            pbuffer[conf.ProtoPackageLength - 1] = 0; // crc
            ProtoCRC8(pbuffer, conf.ProtoPackageLength, true);
            pbuffer += conf.ProtoPackageLength;
        }

        unit_udpsocket.writeDatagram((char *) buffer, 256, group, config.multiRxPort);
    }

    if (unit_udpsocket.isOpen()) {
        unit_udpsocket.close();
    }
}

void FactDecoderMgr::unitTestDecoder(int ctxrand, uint8_t jump, int type) {
    ProtoDecoderInfo &conf = GetProtoDecoderInfo();
    for (auto x = veh_proto_set.begin(); x != veh_proto_set.end(); ++x) {
        uint8_t buffer[128] = {0};
        uint8_t *pbuffer = (uint8_t *) (buffer);
        if (jump == 2) {
            uint16_t *tmp = (uint16_t *) (buffer + (type > 0 ? 1 : 0));
            for (int im = 0; im < 120; im += 2) {
                *tmp = ctxrand;
                ++tmp;
            }
        } else if (jump == 3) {
            uint32_t *tmp = (uint32_t *) (buffer);
            for (int im = 0; im < 120; im += 4) {
                *tmp = ctxrand;
                ++tmp;
            }
        } else {
            memset(buffer, ctxrand % 256, 128);
        }
        x.value()->decoder(pbuffer, conf.ProtoPackageLength, 0, nullptr);
    }
}

FactTableModel *FactDecoderMgr::unittestProto(int chl, int type, const uint8_t *buffer) {
    assert(buffer != nullptr);
    auto mx = veh_proto_set.find(type);
    if (mx != veh_proto_set.end()) {
        mx.value()->decoder(buffer, strlen((const char *) buffer), ((chl << 8) | chl), nullptr);
        return mx.value().get();
    }
    return nullptr;
}

bool FactDecoderMgr::loadExcelProto(const QString &dataFile) {
    bool rtn = proto_parser.protoParse(warnings, veh_proto_set, dataFile, file_datatime);
    for (auto x = veh_proto_set.begin(); x != veh_proto_set.end(); ++x) {
        frame_type_set.append(x.value().get());
    }
    return rtn;
}
