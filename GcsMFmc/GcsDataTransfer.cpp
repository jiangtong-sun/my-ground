#include "GcsDataTransfer.h"
#include "log_category.h"
#include "prototelemetry.h"
#include "stringtranslate.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxdocument.h"
#include "xlsxworkbook.h"

#define ProtoDataSeq transToUnicode("序号")
#define ProtoDataName transToUnicode("名称")
#define ProtoFrameIdent transToUnicode("帧类型")
#define ProtoItemIdent transToUnicode("数据项ID")
#define ProtoDataRange transToUnicode("范围")
#define ProtoDataOffset transToUnicode("偏移")
#define ProtoDataResol transToUnicode("分辨率")
#define ProtoDataType transToUnicode("数据类型")

static uint8_t TransProtoCRC8(const uint8_t *addr, uint8_t len, bool set) {
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

GcsDataTransfer::~GcsDataTransfer() {}

GcsDataTransfer::GcsDataTransfer() {
    enable_mode = false;
    excelTableHeader.append(ColumnName{ProtoDataSeq, true});
    excelTableHeader.append(ColumnName{ProtoDataName, true});
    excelTableHeader.append(ColumnName{ProtoFrameIdent, true});
    excelTableHeader.append(ColumnName{ProtoItemIdent, true});
    excelTableHeader.append(ColumnName{ProtoDataRange, true});
    excelTableHeader.append(ColumnName{ProtoDataOffset, true});
    excelTableHeader.append(ColumnName{ProtoDataResol, true});
    excelTableHeader.append(ColumnName{ProtoDataType, true});

    datatypeMap.insert(QStringLiteral("Uint8"), valueTypeUint8);
    datatypeMap.insert(QStringLiteral("Int8"), valueTypeInt8);
    datatypeMap.insert(QStringLiteral("Uint16"), valueTypeUint16);
    datatypeMap.insert(QStringLiteral("Int16"), valueTypeInt16);
    datatypeMap.insert(QStringLiteral("Uint32"), valueTypeUint32);
    datatypeMap.insert(QStringLiteral("Int32"), valueTypeInt32);
}

///
/// \brief GcsDataTransfer::transferProto
/// \param protoMgr
/// \param freq
/// \param bigLittle
///
///

int GcsDataTransfer::assignProto(MetaValueType valueType, uint8_t *buffer, int bigLittle,
                                 double inputs) {
    switch (valueType) {
    case valueTypeUint8:
    case valueTypeInt8: {
        uint8_t input = inputs;
        buffer[0] = input;
        return 1;
    }
    case valueTypeUint16:
    case valueTypeInt16: {
        uint16_t input = inputs;
        if (bigLittle) {
            buffer[0] = (input >> 8) & 0xff;
            buffer[1] = (input) &0xff;
        } else {
            buffer[0] = (input) &0xff;
            buffer[1] = (input >> 8) & 0xff;
        }
        return 2;
    }
    case valueTypeUint32:
    case valueTypeInt32: {
        uint32_t input = inputs;
        if (bigLittle) {
            buffer[0] = (input >> 24) & 0xff;
            buffer[1] = (input >> 16) & 0xff;
            buffer[2] = (input >> 8) & 0xff;
            buffer[3] = (input) &0xff;
        } else {
            buffer[3] = (input >> 24) & 0xff;
            buffer[2] = (input >> 16) & 0xff;
            buffer[1] = (input >> 8) & 0xff;
            buffer[0] = (input) &0xff;
        }
        return 4;
    }
    default:
        break;
    }
    return 1;
}

void GcsDataTransfer::enableDataTranfer(bool transMode) { enable_mode = transMode; }

void GcsDataTransfer::transferProto(FactDecoderMgr *protoMgr, int freq, int bigLittle, int chl) {
    if (!enable_mode) {
        return;
    }

    static int countTrans = 0;
    assert(protoMgr != nullptr);
    freq = 1000 / (freq <= 0 ? 1 : freq);

    if (next_trans_ms <= 1) {
        next_trans_ms = currentTimeMs() + freq;
    }

    if (currentTimeMs() < next_trans_ms) {
        return;
    }

    next_trans_ms += freq;

    int bufSize = 0;
    uint8_t buffer[64] = {0};
    for (int ix = 0; ix < proto_datatype.size(); ix++) {
        TypeMetaData *curData = proto_datatype.at(ix);
        if (curData == nullptr) {
            return;
        }

        MetaValueType valueType = curData->metaValueType();
        QString value = curData->getBitSet();
        if (!value.isEmpty()) {
            int frameType = value.toInt();
            if (value.contains("0x") || value.contains("0X")) {
                bool ok = false;
                frameType = value.trimmed().toInt(&ok, 16);
            }
            FactTableModel *model = protoMgr->protoframe(frameType);
            if (model != nullptr) {
                int item = curData->byteCount();
                int offset = curData->offset();
                double resol = curData->resolution();
                FactMetaData *metaData = model->loadFact(item);
                if (metaData != nullptr) {
                    double proto = metaData->display(chl).toDouble();
                    double result = (proto + offset) / resol;
                    bufSize += assignProto(valueType, buffer + bufSize, bigLittle, result);
                } else {
                    bufSize += assignProto(valueType, buffer + bufSize, bigLittle, 0);
                }
            } else {
                bufSize += assignProto(valueType, buffer + bufSize, bigLittle, 0);
            }
        } else {
            QString range = curData->range();
            bool ok = false;
            int res = 0;
            if (range.contains("0x") || range.contains("0X")) {
                res = range.trimmed().toInt(&ok, 16);
            }

            bufSize += assignProto(valueType, buffer + bufSize, bigLittle, res);
        }
    }

    TransProtoCRC8((uint8_t *) buffer, bufSize, true);

    if (++countTrans % 50 == 0) {
        QString hex = ToHexStr((uint8_t *) buffer, bufSize);
        COMMON_INFO("trans data {}: {} ", bufSize, hex.toStdString().c_str());
    }

    socket_simple.Send((char *) buffer, bufSize, nullptr);
}

void GcsDataTransfer::stopNetworkService() {
    if (socket_simple.IsConnected()) {
        socket_simple.Destory();
    }
    while (!proto_datatype.empty()) {
        auto begin = proto_datatype.begin();
        if (begin != proto_datatype.end()) {
            TypeMetaData *cur = *begin;
            proto_datatype.erase(begin);
            delete cur;
        }
    }
}

void GcsDataTransfer::enableNetworkService(LoopHandle *loop, NetworkNodeConfig *ext) {
    if (loop == nullptr || ext == nullptr) {
        COMMON_INFO("invalid param {}", (int) loop);
        return;
    }

    ChannelConfig &configslave = ext->config();
    socket_simple.Initialize(configslave, nullptr, true);
    socket_simple.CreateEx(nullptr);
}

bool GcsDataTransfer::parseTransFile(const QString &dataFile) {
    QXlsx::Document xlsxRead(dataFile);
    if (!xlsxRead.load()) {
        return false;
    }

    QStringList list = xlsxRead.sheetNames();
    for (int i = 0; i < list.length(); i++) {
        QString protoFrame = list[i].trimmed();
        if (!xlsxRead.selectSheet(protoFrame)) {
            continue;
        }

        QXlsx::Worksheet *currSheet = xlsxRead.currentWorksheet();
        QXlsx::CellRange range = currSheet->dimension();
        int fcol = range.firstColumn();
        int lcol = range.lastColumn();
        int frow = range.firstRow();
        int lrow = range.lastRow();
        QVector<ColumnName> columnStatus;

        lrow = lrow > 256 ? 256 : lrow;
        lcol = lcol > 32 ? 32 : lcol;
        fcol = fcol < 0 ? 0 : fcol;
        frow = frow < 0 ? 0 : frow;

        bool getValid = false;
        while ((frow <= lrow) && (!getValid)) {
            for (int col = fcol; (col <= lcol) && (!getValid); col++) {
                QVariant value = currSheet->read(frow, col);
                getValid = value.isValid();
            }

            if (getValid) { // table header
                for (int col = fcol; col <= lcol; col++) {
                    QVariant value = currSheet->read(frow, col);
                    if (!value.isNull()) {
                        for (auto itr = excelTableHeader.begin(); itr != excelTableHeader.end();
                             itr++) {
                            if (itr->name.contains(value.toString().trimmed())) {
                                columnStatus.append(ColumnName{value.toString(), col});
                                break;
                            }
                        }
                    } else {
                        COMMON_WARN("failed to read element from proto file");
                    }
                }
            }

            ++frow;
        }

        int currIndex = 1;
        for (int row = frow; row <= lrow; row++) {
            TypeMetaData *dataElem = new TypeMetaData();
            proto_datatype.append(dataElem); //
            for (auto itr = columnStatus.begin(); itr != columnStatus.end(); itr++) {
                QVariant value = currSheet->read(row, itr->column);
                if (itr->name.compare(ProtoDataSeq) == 0 && value.isValid()) {
                    dataElem->setItemNum(value.toString());
                } else if (itr->name.compare(ProtoDataName) == 0 && value.isValid()) {
                    dataElem->setName(value.toByteArray());
                } else if (itr->name.compare(ProtoFrameIdent) == 0 && value.isValid()) {
                    dataElem->setBitSet(value.toString().trimmed());
                } else if (itr->name.compare(ProtoItemIdent) == 0 && value.isValid()) {
                    dataElem->setByteCount(value.toInt());
                } else if (itr->name.compare(ProtoDataRange) == 0 && value.isValid()) {
                    dataElem->setRange(value.toString());
                } else if (itr->name.compare(ProtoDataResol) == 0 && value.isValid()) {
                    dataElem->setResolution(calResolution(value.toString()));
                } else if (itr->name.compare(ProtoDataOffset) == 0 && value.isValid()) {
                    dataElem->setOffset(value.toString().toInt());
                } else if (itr->name.compare(ProtoDataType) == 0 && value.isValid()) {
                    auto xmm = datatypeMap.find(value.toString());
                    if (xmm != datatypeMap.end()) {
                        dataElem->setMetaValueType(xmm.value());
                    }
                }
            }
            ++currIndex;
        }
    }

    return true;
}

double GcsDataTransfer::calResolution(const QString &value) {
    bool ok = false;
    if (value.indexOf("/") > 0) {
        QStringList em = value.split("/", Qt::SkipEmptyParts);
        if (em.size() >= 2) {
            double numerator = em[0].toDouble();
            double denominator = em[1].toDouble();
            if (denominator == 0) { // todo
                denominator = 1;
            }
            return numerator / denominator;
        }
    } else if (value.indexOf("0.") == 0) {
        return value.toDouble();
    }

    if (value.contains("0x")) {
        return value.trimmed().toInt(&ok, 16);
    } else {
        return value.trimmed().toInt(&ok);
    }
}

RTKDataTranfer::RTKDataTranfer() : call_number(nullptr) { enable_mode = false; }

RTKDataTranfer::~RTKDataTranfer() {}

void RTKDataTranfer::enableDataTranfer(bool val) { enable_mode = val; }

void RTKDataTranfer::tranferProtoMain(RockerDeviceProto *msg, Channel *mainNet) {
    if (mainNet != nullptr) {
        mainNet->Send((char *) msg, msg->length, [=](int status, char *buf, int len) {
            RockerDeviceProto *frame = (RockerDeviceProto *) buf;
            if (frame != nullptr) {
                delete frame;
            }
        });
    }
}

void RTKDataTranfer::tranferProtoSlave(RockerDeviceProto *msg, Channel *slaveNet) {
    if (slaveNet != nullptr) {
        slaveNet->Send((char *) msg, msg->length, [=](int status, char *buf, int len) {
            RockerDeviceProto *frame = (RockerDeviceProto *) buf;
            if (frame != nullptr) {
                delete frame;
            }
        });
    }
}

void RTKDataTranfer::tranferProto(Channel *mainNet, PtrRockerMessage msg,
                                  NetworkNodeConfig *extConf) {
    if (msg == nullptr) {
        return;
    }
    mainNet->Send((char *) msg, sizeof(*msg), [=](int status, char *buf, int len) {
        ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
        RockerDeviceProto *frame = (RockerDeviceProto *) buf;
        if ((frame->dlen != sizeof(frame->datas)) && (++_oprint_freq % 5 == 0)) {
            COMMON_INFO("trans {}, id: {}, len {}", (int) mainNet, msg->number, msg->length);
        }
        if (frame != nullptr) {
            delete frame;
        }
    });
}

void RTKDataTranfer::tranferProto(Channel *mainNet, Channel *slaveNet, RockerProtoQueue &protoCache,
                                  NetworkNodeConfig *extConf) {
    assert(mainNet != nullptr);
    assert(slaveNet != nullptr);
    assert(extConf != nullptr);

    PtrRockerMessage msg = nullptr;
    if (!extConf->enable()) {
        if (rtk_write_cache.size_approx() > 0 && rtk_write_cache.try_dequeue(msg)) {
            delete msg;
        }
        return;
    }

    if (extConf->copyMode()) {
        /// 每次进行5次数据循环操作
        for (int ix = 0; ix < 2; ix++){
            ///席位有权限且有数据的权限的时候转发RTK消息数据
            if ((rtk_avail_func != nullptr) && (rtk_avail_func()) &&
                rtk_write_cache.size_approx() > 0 && rtk_write_cache.try_dequeue(msg)) {
                RockerDeviceProto *msg2 = nullptr;
                if (extConf && extConf->copyMode()) {
                    msg2 = new RockerDeviceProto();
                    memcpy(msg2, msg, sizeof(RockerDeviceProto));
                }
                tranferProtoMain(msg, mainNet);
                tranferProtoSlave(msg2, slaveNet);
            }
        }
    }
    // else if (mainNet->isMatch(_rtk_dst_addr, _rtk_dst_port)) {
    //     if (rtk_write_cache.size_approx() > 0 && rtk_write_cache.try_dequeue(msg)) {
    //         tranferProto(mainNet, msg, nullptr);
    //     }
    // } else if (slaveNet->isMatch(_rtk_dst_addr, _rtk_dst_port)) {
    //     if (rtk_write_cache.size_approx() > 0 && rtk_write_cache.try_dequeue(msg)) {
    //         tranferProto(slaveNet, msg, nullptr);
    //     }
    // } else {
    //     if (rtk_write_cache.size_approx() > 0 && rtk_write_cache.try_dequeue(msg)) {
    //         if (msg != nullptr) { // check msg pointer is nullptr or not
    //             if ((msg->dlen != sizeof(msg->datas)) && (++_oprint_freq % 5 == 0)) {
    //                 COMMON_INFO("trans id: {}, len {}", msg->number, msg->length);
    //             }
    //             protoCache.enqueue(msg);
    //         }
    //     }
    // }
}

bool RTKDataTranfer::enableNetworkService(RtkAvailCallback rtkAvail, CallNumberCallback call,
                                          LoopHandle *loop, NetworkNodeConfig *ext,
                                          NetworkNodeConfig *main, NetworkNodeConfig *slave) {
    assert(ext != nullptr);
    assert(loop != nullptr);
    if (ext == nullptr || main == nullptr || slave == nullptr) {
        COMMON_WARN("invalid param");
        return false;
    }

    if (!ext->enable()) {
        COMMON_INFO("trans flag is false");
        return false;
    }

    call_number = call; // 消息帧ID
    rtk_avail_func = rtkAvail; // 席位的权限

    _rtk_dst_port = ext->multiTxPort();
    _rtk_dst_addr = ext->multiTxIp().toStdString();

    main_config = main;
    slave_config = slave;

    // QVariant tt(1);
    // ChannelConfig &configmain = main->config();
    // QHostAddress address(QString::fromStdString(configmain.address));
    // udpsocket_rtkmain.bind(address, configmain.multiTxPort,
    //                        QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    // udpsocket_rtkmain.setSocketOption(QAbstractSocket::MulticastLoopbackOption, tt);
    // COMMON_INFO("data trans {}:{}", configmain.address, configmain.multiTxPort);

    // ChannelConfig &configslave = slave->config();
    // QHostAddress address1(QString::fromStdString(configslave.address));
    // udpsocket_rtkslave.bind(address1, configslave.multiTxPort,
    //                         QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    // udpsocket_rtkslave.setSocketOption(QAbstractSocket::MulticastLoopbackOption, tt);
    // COMMON_INFO("data trans {}:{}", configslave.address, configslave.multiTxPort);

    if (ext->tcpMode()) {
        return tcpNetworkService(loop, ext, rtkAvail);
    }

    return multiNetworkService(loop, ext, rtkAvail);
}

void RTKDataTranfer::stopNetworkService() {

    rtk_rw_client.stopClient();

    // if (rtk_read_write.IsConnected()) {
    //     rtk_read_write.Destroy();
    // }

    PtrRockerMessage msg = nullptr;
    while (rtk_write_cache.size_approx() > 0 && rtk_write_cache.try_dequeue(msg)) {
        delete msg;
    }
}

void RTKDataTranfer::cacheTransData(RtkAvailCallback rtkAvail, const char *buffer, int length) {
    int uncheck = 0;
    int dataLen = sizeof(RockerDeviceProto::datas);
    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    if (rtkAvail()) {
        while ((uncheck < length)) {
            PtrRockerMessage msg = new RockerDeviceProto();
            assert(msg != nullptr);
            int val = (++_cmd_number) % 256;
            memset(msg, 0, sizeof(RockerDeviceProto));
            msg->magic1 = decoder.ProtoSplitWordA;
            msg->magic2 = decoder.ProtoSplitWordB;
            msg->length = sizeof(RockerDeviceProto);
            msg->number = call_number ? call_number() : val;
            msg->cmd_type = decoder.RTKTransferFrame;
            FillProtoTime(msg);
            int actLen = length - uncheck;
            actLen = actLen > dataLen ? dataLen : actLen;
            memcpy(msg->datas, buffer + uncheck, actLen);
            msg->dlen = actLen;
            ProtoCRC8((uint8_t *) msg, sizeof(RockerDeviceProto), true);
            rtk_write_cache.enqueue(msg);
            uncheck += actLen;
        }
    }
    _totalNumber += length;
    COMMON_TRACE("trans data size {}", _totalNumber);
}

bool RTKDataTranfer::tcpNetworkService(LoopHandle *loop, NetworkNodeConfig *ext,
                                       RtkAvailCallback rtkAvail) {
    if (loop == nullptr || ext == nullptr || rtkAvail == nullptr) {
        COMMON_WARN("invalid param");
        return false;
    }

    ChannelConfig &conf = ext->config();

    rtk_rw_client.initSocket(
        QString::fromStdString(conf.address), conf.netPort,
        [=](const QByteArray &datas) { cacheTransData(rtkAvail, datas.data(), datas.length()); });

    return true;
}

bool RTKDataTranfer::multiNetworkService(LoopHandle *loop, NetworkNodeConfig *ext,
                                         RtkAvailCallback rtkAvail) {
    // if (loop != nullptr && ext != nullptr) {
    //     rtk_read_write.SetCloseCallback(
    //         [&](long value) { COMMON_INFO("channel closed {}", value); });
    //     rtk_read_write.SetRecvCallback(nullptr);
    //     rtk_read_write.SetDecoder([=](ProtoDataCache &dataCache, const void *addr) {
    //         cacheTransData(rtkAvail, (char *) dataCache.data, dataCache.length);
    //         dataCache.length = 0;
    //         dataCache.lastPos = 0;
    //     });
    //     ChannelConfig &conf = ext->config();
    //     rtk_read_write.Initialize(conf, loop, ext->localLoop());
    //     rtk_read_write.Create(loop);
    //     return true;
    // }

    return false;
}
