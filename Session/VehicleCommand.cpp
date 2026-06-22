#include "VehicleCommand.h"
#include "log_category.h"
#include "prototelemetry.h"
#include <QBitArray>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <filesystem>

VehicleCommand::~VehicleCommand() {}

VehicleCommand::VehicleCommand(ControlAdjustProtoQueue &serial, ControlAdjustProtoQueue &mainqueue,
                               ControlAdjustProtoQueue &slavequeue)
    : serial_write_cache(serial), main_write_cache(mainqueue), slave_write_cache(slavequeue) {}

bool VehicleCommand::reloadAdjusts(const QStringList &file) { return vehAdjustControl.reloadMoreAdjust(file); }

bool VehicleCommand::reloadCommands(const QString &file) { return vehAdjustControl.reloadCommands(file); }

void VehicleCommand::setRCMode(bool type) {
    _rcModeType = type;
    emit rcModeChanged();
}

void VehicleCommand::fillProtoTime(ControlAdjustProto &proto) {
    QTime curtime = QTime::currentTime();
    uint32_t tm = (curtime.hour() << 19) | (curtime.minute() << 13) | (curtime.second() * 100 + curtime.msec() / 10);
    proto.hour = (tm >> 16) & 0xff;
    proto.minute = (tm >> 8) & 0xff;
    proto.second = (tm >> 0) & 0xff;
}

void VehicleCommand::parseRCBytes(QTextStream &out, QByteArray &next) {
    const uint8_t *pdata = (const uint8_t *) next.data();
    ProtoDecoderInfo &decoderInfo = GetProtoDecoderInfo();
    ControlAdjustProto *proto = (ControlAdjustProto *) pdata;

    if (proto->cmd_type == decoderInfo.AdjustControlType && proto->adjust_code != 0) {
        RCAdjustItem *adjust = vehAdjustControl.getAdjustByfeedback(proto->feedback);
        if (adjust != nullptr) {
            QString name = adjust->name();
            FactMetaData *dat = adjust->metaData();
            if (dat != nullptr) {
                switch (dat->metaValueType()) {
                case valueTypeUint8: {
                    uint8_t *val = (uint8_t *) (proto->adjust_detail);
                    float act = (*val) * dat->resolution();
                    name = QString("%1 cmd: [%2], origin: [%3]").arg(adjust->name()).arg(*val).arg(act);
                    break;
                }
                case valueTypeInt8: {
                    int8_t *val = (int8_t *) (proto->adjust_detail);
                    float act = (*val) * dat->resolution();
                    name = QString("%1 cmd: [%2], origin: [%3]").arg(adjust->name()).arg(*val).arg(act);
                    break;
                }
                case valueTypeUint16: {
                    uint16_t *val = (uint16_t *) (proto->adjust_detail);
                    float act = (*val) * dat->resolution();
                    name = QString("%1 cmd: [%2], origin: [%3]").arg(adjust->name()).arg(*val).arg(act);
                    break;
                }
                case valueTypeInt16: {
                    int16_t *val = (int16_t *) (proto->adjust_detail);
                    float act = (*val) * dat->resolution();
                    name = QString("%1 cmd: [%2], origin: [%3]").arg(adjust->name()).arg(*val).arg(act);
                    break;
                }
                case valueTypeUint32: {
                    uint32_t *val = (uint32_t *) (proto->adjust_detail);
                    float act = (*val) * dat->resolution();
                    name = QString("%1 cmd: [%2], origin: [%3]").arg(adjust->name()).arg(*val).arg(act);
                    break;
                }
                case valueTypeInt32: {
                    int32_t *val = (int32_t *) (proto->adjust_detail);
                    float act = (*val) * dat->resolution();
                    name = QString("%1 cmd: [%2], origin: [%3]").arg(adjust->name()).arg(*val).arg(act);
                    break;
                }
                }
            }

            QString hex = ToHexStr((const uint8_t *) next.data(), next.length());
            out << name << " == " << hex << "\r\n";
        }

    } else if (proto->cmd_type == decoderInfo.AdjustControlType && proto->ctrl_type != 0) {
        RCCommandItem *ctrl = vehAdjustControl.getControlByFeedback(proto->feedback);
        if (ctrl != nullptr) {
            QString name = ctrl->name();
            QString hex = ToHexStr((const uint8_t *) next.data(), next.length());
            out << name << " == " << hex << "\r\n";
        }
    }
}

void VehicleCommand::exportRCData() {
    std::filesystem::path path1("RawData/rawdata");
    if (!std::filesystem::exists(path1)) {
        return;
    }

    std::filesystem::path path2("RawData/exports");
    if (!std::filesystem::exists(path2)) {
        std::filesystem::create_directories(path2);
    }

    QDir dir("RawData/rawdata");
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();
    const quint32 BUFFER_SIZE = 256;
    QByteArray buffer(BUFFER_SIZE, 0);

    for (const QFileInfo &fileInfo : list) {
        QString path = fileInfo.absoluteFilePath();
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) {
            continue;
        }

        int len = 2;
        QByteArray header = file.read(2);
        while (!file.atEnd()) {
            QByteArray next = file.read(2);
            if (next == header) {
                break;
            }
            len += 2;
        }

        file.seek(0);

        QString mainRc = QString("RawData/exports/%1").arg(fileInfo.fileName());

        QFile out(mainRc);
        out.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream outStream(&out);
        outStream.setCodec("UTF-8");

        while (!file.atEnd()) {
            QByteArray next = file.read(len);
            if (next.length() == len) {
                parseRCBytes(outStream, next);
            }
        }

        out.close();
        file.close();
    }
}

void VehicleCommand::exportRMData() {}

bool VehicleCommand::isSameCommand(uint8_t seq) {
    QDateTime utc_time = QDateTime::currentDateTimeUtc();
    qint64 curr = utc_time.currentMSecsSinceEpoch();
    /// same command
    if (last_operate_cmd == seq) {
        return true;
    }
    /// delay 100ms
    if (abs(last_operate_tm - curr) < 100) {
        return true;
    }

    last_operate_cmd = seq;
    last_operate_tm = curr;
    return false;
}

std::shared_ptr<ControlAdjustVerify> VehicleCommand::feedbackVerify(FlightVechicle *vehicle,
                                                                    ControlAdjustProto *nodeProto) {
    if (vehicle == nullptr || nodeProto == nullptr) {
        COMMON_WARN("vehicle {}, control & adjust {}", (int) vehicle, (int) nodeProto);
        return nullptr;
    }

    VehicleFeedback *feedback = vehicle->feedback();
    assert(feedback != nullptr);

    RCCommandItem *foundcmd1 = nullptr;
    RCCommandItem *foundcmd2 = nullptr;

    if (nodeProto->ctrl_type > 0 && nodeProto->ctrl_code[0] > 0) {
        foundcmd1 = vehAdjustControl.getControlByFeedback(nodeProto->feedback);
        foundcmd2 = vehAdjustControl.getControlByFeedback(nodeProto->feedback);
    }

    if ((foundcmd1 == foundcmd2) && (foundcmd1 != nullptr)) {

#if defined(AFC21_PROJECT_CONF)
        int frameNo = nodeProto->frame_no;
        if (isSameCommand(nodeProto->frame_no)) {
            QString name = foundcmd2->name();
            QByteArray byte8Str = name.toLocal8Bit();
            COMMON_INFO("same command {} {}", byte8Str.toStdString(), frameNo);
            return nullptr;
        }
#else
        int frameNo = nodeProto->number;
        if (isSameCommand(nodeProto->number)) {
            QString name = foundcmd2->name();
            QByteArray byte8Str = name.toLocal8Bit();
            COMMON_INFO("same command {} {}", byte8Str.toStdString(), frameNo);
            return nullptr;
        }
#endif

        /// 通过控制命令来检索控制命令相关信息，并将发送信息记录到Table表格中
        /// foundcmd1->setWorking(false); /// 告知UI消息已经被发送
        int rowId = feedback->recordCommand(foundcmd2->name());
        COMMON_INFO("frame {}, control code {}, control feedback {}", frameNo, nodeProto->ctrl_code[0],
                    nodeProto->feedback);
        std::shared_ptr<ControlAdjustVerify> feedbackVerify(new ControlAdjustVerify());
        feedbackVerify->control = foundcmd2;
        feedbackVerify->setExpect(foundcmd2->name(), frameNo, vehicle->vehicleIdx(), rowId, foundcmd2->feed());
        return feedbackVerify;

    } else {
        /// 通过遥调命令Feedback 与 Number 来检索遥调指令的相关信息
        int cmdfed = nodeProto->feedback;
        int cmdtype = nodeProto->adjust_code;
        int subtype = nodeProto->adjust_detail[0];
        RCAdjustItem *item = vehAdjustControl.getAdjustByfeedback(cmdfed, cmdtype, subtype);
        if (item != nullptr) {
            QString name = item->groupName();
            bool nullname = true;
            if ((subtype >= 0)) {
                name = item->getCmdNameByCode(subtype);
                if (name.isEmpty()) {
                    name = item->enumStrByKey(subtype);
                }
                nullname = name.isEmpty();
            }

            /// 有多个数据行的协议时候选择第一行扩展名称
            /// 判断该Item的FactMetaData子项目的状态
            if (nullname) {
                int select = item->selectSub();
                if (item->countSubMeta() <= 1) {
                    name = item->mData()->name();
                } else {
                    name = item->fectElem(0)->name();
                    if (item->countSubMeta() > 1) {
                        name.append(item->fectElem(1)->name());
                    }
                }
            }

            item->setMapSelect(-1);
            item->setSelectId(-1);
            item->setSelectSub(-1);

#if defined(AFC21_PROJECT_CONF)
            int frameNo = nodeProto->frame_no;
            if (isSameCommand(nodeProto->frame_no)) {
                QByteArray byte8Str = name.toLocal8Bit();
                COMMON_INFO("same command {} {}", byte8Str.toStdString(), frameNo);
                return nullptr;
            }
#else
            int frameNo = nodeProto->number;
            if (isSameCommand(nodeProto->number)) {
                QByteArray byte8Str = name.toLocal8Bit();
                COMMON_INFO("same command {} , name: {}, number: {}", cmdtype, byte8Str.toStdString(), frameNo);
                return nullptr;
            }
#endif
            std::shared_ptr<ControlAdjustVerify> feedbackVerify(new ControlAdjustVerify());
            COMMON_INFO("frame {}, adjust code {}, adjust feedback {}, subtype {}", frameNo, nodeProto->feedback,
                        cmdtype, subtype);
            int rowId = feedback->recordCommand(name);
            feedbackVerify->adjusts = item;
            feedbackVerify->setExpect(name, frameNo, vehicle->vehicleIdx(), rowId, item->feedback());
            return feedbackVerify;
        }
    }

    return nullptr;
}

void VehicleCommand::networkKeepalive(bool serial, ControlAdjustProto &adjust, int chl) {
    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();

    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.length = sizeof(ControlAdjustProto);
    adjust.number = (++_cmdNumber) % 256;
    adjust.cmd_type = decoder.keepaliveCommand;
    adjust.src_addr = 0xff;
    adjust.dst_addr = 0xff;
    fillProtoTime(adjust);

    memset(adjust.ext_space, chl, sizeof(adjust.ext_space));
    ProtoCRC8((uint8_t *) &adjust, sizeof(ControlAdjustProto), true);

    if (serial) {
        PtrProtoMessage msg2 = newControlAdjustProto(adjust);
        if (!serial_write_cache.enqueue(msg2)) {
            delete msg2; //
        }
    }
}

void VehicleCommand::contrlRefAdjustCmdExt(RCAdjustItem *data, FactDecoderMgr *fact, int code){
    assert(data != nullptr);
    if (data == nullptr || fact == nullptr) {
        COMMON_INFO("invalid input param");
        return;
    }

    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    ControlAdjustProto adjust;

    int8_t val = code & 0xff;
    data->setRawValue(QString::number(val));

    memset(&adjust, 0, sizeof(ControlAdjustProto));

    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.length = sizeof(ControlAdjustProto);
    adjust.number = (++_cmdNumber) % 256;
    adjust.cmd_type = decoder.AdjustCommandType;
    adjust.adjust_type = decoder.adjustCommand;
    adjust.adjust_code = data->itemNum();

    uint32_t value = data->mData()->value();
    for (int i = 0; i < data->mData()->byteCount() / 8; i++) {
        adjust.adjust_detail[i] = (value >> (8 * i)) & 0xff;
    }

    fillProtoTime(adjust);

    doAdjust(&adjust, data, fact);
}

void VehicleCommand::contrlRefAdjustCmd(RCAdjustItem *data, RCCommandItem *ctl, FactDecoderMgr *fact) {
    assert(data != nullptr);
    if (data == nullptr || fact == nullptr) {
        COMMON_INFO("invalid input param");
        return;
    }

    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    ControlAdjustProto adjust;

    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.length = sizeof(ControlAdjustProto);
    adjust.number = (++_cmdNumber) % 256;
    adjust.cmd_type = decoder.AdjustCommandType;
    adjust.adjust_type = decoder.adjustCommand;
    adjust.adjust_code = data->itemNum();
    adjust.adjust_detail[0] = ctl->feed();

    fillProtoTime(adjust);

    doAdjust(&adjust, data, fact);
}

void VehicleCommand::sendControlAdjustCommand(RCCommandItem *data, FactDecoderMgr *fact){
    QString refAdjust = data->refAdjust();
    int reffeed = refAdjust.toInt();
    if (refAdjust.contains("0x") || refAdjust.contains("0X")) {
        reffeed = refAdjust.toInt(nullptr, 16);
    }

    if (reffeed > 0xffff) {
        RCAdjustItem *elem = vehAdjustControl.getAdjustByfeedback((reffeed>>16)&0xff, (reffeed >>8)&0xff, 0xffff);
        contrlRefAdjustCmdExt(elem, fact, reffeed);
        data->setWorking(false);
    }
    else {
        int seq = data->index();
        RCAdjustItem *elem = vehAdjustControl.getAdjustByfeedback(reffeed, seq, 0xffff);
        contrlRefAdjustCmd(elem, data, fact);
        data->setWorking(false);
    }
}

void VehicleCommand::sendControlCommand(RCCommandItem *data, FactDecoderMgr *fact) {
    if (data == nullptr || fact == nullptr) {
        COMMON_WARN("invalid param");
        return;
    }

    QString refAdjust = data->refAdjust();
    if (!refAdjust.isEmpty()) {
        sendControlAdjustCommand(data, fact);
        return;
    }

    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    ControlAdjustProto adjust;

    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.length = sizeof(ControlAdjustProto);
    adjust.number = (++_cmdNumber) % 256;
    adjust.cmd_type = decoder.ControlCommandType;
    adjust.ctrl_type = decoder.controlCommand;
    adjust.ctrl_code[0] = data->feed();
    adjust.ctrl_code[1] = data->feed();
    adjust.ctrl_code[2] = data->feed();
    adjust.ctrl_code[3] = data->feed();
    adjust.ctrl_code[4] = data->feed();
    adjust.feedback = data->feed();
    adjust.dst_addr = fact->getVehicleAddr();
    adjust.src_addr = 0xff;
    fillProtoTime(adjust);


    ProtoCRC8((uint8_t *) (&adjust), sizeof(adjust), true);
    QString hex = ToHexStr((uint8_t *) (&adjust), sizeof(adjust));
    COMMON_INFO("TX:{}", hex.toStdString().c_str());

    PtrProtoMessage m1 = newControlAdjustProto(adjust);
    PtrProtoMessage m2 = newControlAdjustProto(adjust);
    PtrProtoMessage m3 = newControlAdjustProto(adjust);

    if ((m1 != nullptr) && !serial_write_cache.enqueue(m1)) {
        delete m1; //
    }
    if ((m2 != nullptr) && !serial_write_cache.enqueue(m2)) {
        delete m2; //
    }
    if ((m3 != nullptr) && !serial_write_cache.enqueue(m3)) {
        delete m3; //
    }

    PtrProtoMessage m11 = newControlAdjustProto(adjust);
    PtrProtoMessage m21 = newControlAdjustProto(adjust);
    PtrProtoMessage m31 = newControlAdjustProto(adjust);

    if ((m11 != nullptr) && !main_write_cache.enqueue(m11)) {
        delete m11; //
    }

    if ((m21 != nullptr) && !main_write_cache.enqueue(m21)) {
        delete m21; //
    }

    if ((m31 != nullptr) && !main_write_cache.enqueue(m31)) {
        delete m31; //
    }

    PtrProtoMessage m12 = newControlAdjustProto(adjust);
    PtrProtoMessage m22 = newControlAdjustProto(adjust);
    PtrProtoMessage m32 = newControlAdjustProto(adjust);

    if ((m12 != nullptr) && !slave_write_cache.enqueue(m12)) {
        delete m12; //
    }

    if ((m22 != nullptr) && !slave_write_cache.enqueue(m22)) {
        delete m22; //
    }

    if ((m32 != nullptr) && !slave_write_cache.enqueue(m32)) {
        delete m32; //
    }
}

PtrProtoMessage VehicleCommand::newControlAdjustProto(ControlAdjustProto &adjust) {
    PtrProtoMessage rnt = new ControlAdjustProto();
    memcpy(rnt, &adjust, sizeof(ControlAdjustProto));
    return rnt;
}

void VehicleCommand::sendMissionCommand(RCAdjustItem *data, FactDecoderMgr *fact) {
    assert(data != nullptr);
    if (data == nullptr || fact == nullptr) {
        COMMON_INFO("invalid input param");
        return;
    }

    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    ControlAdjustProto adjust;

    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.length = sizeof(ControlAdjustProto);
    adjust.number = (++_cmdNumber) % 256;
    adjust.cmd_type = decoder.AdjustCommandType;
    adjust.adjust_type = decoder.adjustCommand;
    adjust.adjust_code = data->mData()->itemNum();
    adjust.dst_addr = fact->getVehicleAddr();
    adjust.src_addr = 0xff;

    FactListModel *subModel = data->moreMetaModel();
    if (subModel == nullptr) {
        COMMON_INFO("invalid input param");
        return;
    }

    fillProtoTime(adjust);

    int offset = 0;
    for (int i = 0; i < subModel->count(); i++) {
        const FactMetaData *node = nullptr;
        if ((node = (const FactMetaData *) subModel->get(i)) != nullptr) {
            uint32_t value = node->value();
            for (int im = 0; im < node->byteCount() / 8; im++) {
                adjust.adjust_detail[offset++] = (value >> (8 * im)) & 0xff;
            }
        }
    }

    doAdjust(&adjust, data, fact);
}

template <class Type>
Type VehicleCommand::inputDataTrans(FactListModel * subModel) {
    Type val = 0;
    for (int j = 0; j < subModel->count(); j++) {

        TransMetaData *node = nullptr;
        if ((node = (TransMetaData *) subModel->get(j)) == nullptr) {
            continue;
        }
        QString rawStr = node->rawInValue();
        QString bitset = node->getBitSet();
        MetaValueType type = node->metaValueType();
        Type value = node->enumValue(rawStr);
        int lidx = bitset.indexOf('.');
        if (lidx > 0) {
            QString left = bitset.mid(0, lidx);
            QString right = bitset.mid(lidx + 1);
            Type idx = value << left.toUInt();
            val |= idx;
        } else {
            QString left = bitset.mid(lidx + 1);
            Type idx = value << left.toUInt();
            val |= idx;
        }
    }

    return val;
}

void VehicleCommand::sendDeviceCommand(RCAdjustItem *data, FactDecoderMgr *fact) {
    assert(data != nullptr);
    if (data == nullptr || fact == nullptr) {
        COMMON_INFO("invalid input param");
        return;
    }

    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    ControlAdjustProto adjust;

#if defined(AFC21_PROJECT_CONF)
    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.cmd_type = decoder.AdjustControlType;
    adjust.adjust_type = decoder.adjustCommand;
    adjust.adjust_code = data->mData()->itemNum();
    const FactListModel *subModel = data->moreMetaModel();
    if ((subModel != nullptr)) {
        int idex = 0;
        const FactMetaData *node = nullptr;
        for (int i = 0; i < subModel->count(); i++) {
            if ((node = (FactMetaData *) subModel->get(i)) != nullptr) {
                if (node->isEnum()) {
                    int selectId = node->enumSelect();
                    int value = node->enumValue(selectId);
                    adjust.adjust_detail[idex++] = value;
                } else {
                    uint32_t value = node->value();
                    for (int im = 0; im < node->byteCount() / 8; im++) {
                        adjust.adjust_detail[idex++] = (value >> (8 * im)) & 0xff;
                    }
                }
            }
        }
    }
#elif defined(AFXF_PROJECT_CONF)
    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.length = sizeof(ControlAdjustProto);
    adjust.number = (++_cmdNumber) % 256;
    adjust.cmd_type = decoder.AdjustCommandType;
    adjust.adjust_type = decoder.adjustCommand;
    adjust.adjust_code = data->itemNum();
    adjust.dst_addr = fact->getVehicleAddr();
    adjust.src_addr = 0xff;

    FactListModel *subModel = data->moreMetaModel();
    assert(subModel != nullptr);
    if ((subModel != nullptr)) {
        int idex = 0;
        FactMetaData *node = nullptr;
        for (int i = 0; i < subModel->count(); i++) {
            if ((node = (FactMetaData *) subModel->get(i)) != nullptr) {
                if (node->isEnum()) {
                    int selectId = node->enumSelect();
                    int value = node->enumValue(selectId);
                    adjust.adjust_detail[idex++] = value;
                } else {
                    uint32_t value = node->value();
                    for (int im = 0; im < node->byteCount() / 8; im++) {
                        adjust.adjust_detail[idex++] = (value >> (8 * im)) & 0xff;
                    }
                }
            }
        }
    }
    fillProtoTime(adjust);
#elif defined(WHXQ_PROJECT_CONF)
    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.length = sizeof(ControlAdjustProto);
    adjust.number = (++_cmdNumber) % 256;
    adjust.cmd_type = decoder.AdjustCommandType;
    adjust.adjust_type = decoder.adjustCommand;
    adjust.adjust_code = data->itemNum();
    FactListModel *subModel = data->moreMetaModel();
    assert(subModel != nullptr);
    if ((subModel != nullptr)) {
        int idex = 0;
        FactMetaData *node = nullptr;
        for (int i = 0; i < subModel->count(); i++) {
            if ((node = (FactMetaData *) subModel->get(i)) != nullptr) {
                if (node->isEnum()) {
                    int selectId = node->enumSelect();
                    int value = node->enumValue(selectId);
                    adjust.adjust_detail[idex++] = value;
                } else {
                    uint32_t value = node->value();
                    for (int im = 0; im < node->byteCount() / 8; im++) {
                        adjust.adjust_detail[idex++] = (value >> (8 * im)) & 0xff;
                    }
                }
            }
        }
    }
    fillProtoTime(adjust);
#endif

    doAdjust(&adjust, data, fact);
}

void VehicleCommand::sendSimpleCommand(RCAdjustItem *data, FactDecoderMgr *fact) {
    assert(data != nullptr);
    if (data == nullptr || fact == nullptr) {
        COMMON_INFO("invalid input param");
        return;
    }

    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    ControlAdjustProto adjust;

    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.length = sizeof(ControlAdjustProto);
    adjust.number = (++_cmdNumber) % 256;
    adjust.cmd_type = decoder.AdjustCommandType;
    adjust.adjust_type = decoder.adjustCommand;
    adjust.adjust_code = data->itemNum();
    adjust.dst_addr = fact->getVehicleAddr();
    adjust.src_addr = 0xff;

    int enumVal = data->selectId();
    int subSelect = data->selectSub();
    int mapSelect = data->mapSelect();
    const FactMetaData *mData = data->mData();
    if (subSelect >= 0) {
        mData = data->fectElem(subSelect);
    }

    int val = mapSelect > 0 ? mData->enumValueCheck(mapSelect) : mData->enumValue(enumVal);
    if (mData == nullptr || val < 0) {
        COMMON_INFO("invalid input param");
        return;
    }

    adjust.adjust_detail[0] = val;
    fillProtoTime(adjust);


    doAdjust(&adjust, data, fact);
}

void VehicleCommand::sendDeviceOnOffCommand(RCAdjustItem *data, FactDecoderMgr *fact) {
    assert(data != nullptr);
    if (data == nullptr || fact == nullptr) {
        COMMON_INFO("invalid input param");
        return;
    }

    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    ControlAdjustProto adjust;

#if defined(AFC21_PROJECT_CONF)
    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.cmd_type = decoder.AdjustControlType;
    adjust.adjust_type = decoder.adjustCommand;
    adjust.adjust_code = data->mData()->itemNum();
    int subItemVal = data->selectId();
    data->setSelectSub(subItemVal); /// 在反馈的时使用子元素
    const FactMetaData *mData = data->fectElem(subItemVal);
    if (mData == nullptr) {
        COMMON_INFO("invalid input param");
        return;
    }
    adjust.adjust_detail[0] = mData->enumValue(mData->enumSelect());
#endif

    doAdjust(&adjust, data, fact);
}

void VehicleCommand::sendDeviceSetCommand(RCAdjustItem *data, const QString &value, FactDecoderMgr *fact) {

    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    ControlAdjustProto adjust;

    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.cmd_type = decoder.AdjustControlType;
    adjust.adjust_type = decoder.adjustCommand;
    adjust.adjust_code = data->mData()->itemNum();

    if (((value.indexOf("0x") >= 0) || (value.indexOf("0X") >= 0))) {
        bool ok = false;
        uint32_t defValue = value.toUInt(&ok, 16);
        for (int i = 0; i < data->mData()->byteCount() / 8; i++) {
            adjust.adjust_detail[i] = (defValue >> (8 * i)) & 0xff;
        }
    } else {
        data->setRawValue(value);
        uint32_t value = data->mData()->value();
        for (int i = 0; i < data->mData()->byteCount() / 8; i++) {
            adjust.adjust_detail[i] = (value >> (8 * i)) & 0xff;
        }
    }

    doAdjust(&adjust, data, fact);
}

void VehicleCommand::sendAdjustATCSpecial(RCAdjustItem *data, FactDecoderMgr *fact, int Acode) {
    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    ControlAdjustProto adjust;
    doAdjust(&adjust, data, fact);
}

void VehicleCommand::sendAdjustATCCommand(RCAdjustItem *data, FactDecoderMgr *fact) {
    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    ControlAdjustProto adjust;

#if defined(AFC21_PROJECT_CONF)
    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.cmd_type = decoder.AdjustControlType;
    adjust.adjust_type = decoder.adjustCommand;

    FactListModel *moreMetaModel = data->moreMetaModel();
    if (moreMetaModel && moreMetaModel->count() > 0) {
        const FactMetaData *value0 = data->fectElem(0);
        int selectId = value0->enumSelect();
        int seleValue = value0->enumValue(selectId);
        adjust.adjust_code = value0->itemNum();
        adjust.adjust_detail[0] = seleValue;

        bool type = false;
        if (moreMetaModel->count() > 1) {
            value0 = data->fectElem(1);
            if (value0->byteCount() > 0) {
                type = true;
            }
        }

        if (!type) {
            uint8_t command = 0;
            for (int ix = 1; ix < moreMetaModel->count(); ix++) {
                const FactMetaData *value = data->fectElem(ix);
                assert(value != nullptr);
                int selectId = value->enumSelect();
                int bitSet = value->getBitSet().toInt();
                int seleValue = value->enumValue(selectId);
                command = command | (seleValue << bitSet);
                COMMON_INFO("selectId {} == {} == {} == {}", selectId, bitSet, seleValue, command);
            }
            adjust.adjust_detail[1] = command;
        } else {

            uint8_t inx = 1;
            bool ok = false;
            const FactMetaData *value = data->fectElem(1);
            assert(value != nullptr);

            QString valueStr = value->rawInValue();
            while (valueStr.length() < 4) {
                valueStr.append('0');
            }

            QByteArray bytes = valueStr.toLatin1();
            uint cur1 = ((((uint) bytes[0] - 0x30) & 0x7) << 3) | (((uint) bytes[1] - 0x30) & 0x7);
            uint cur2 = ((((uint) bytes[2] - 0x30) & 0x7) << 3) | (((uint) bytes[3] - 0x30) & 0x7);
            adjust.adjust_detail[inx++] = cur1;
            adjust.adjust_detail[inx++] = cur2;

            value = data->fectElem(2);
            assert(value != nullptr);
            if (value->rawInValue().compare("000000") == 0 || value->rawInValue().compare("FFFFFF") == 0) {
                return;
            }

            valueStr = value->rawInValue();
            while (valueStr.length() < 6) {
                valueStr.append('0');
            }

            bytes = valueStr.toLatin1();
            cur1 = ((((uint) bytes[0] - 0x30) & 0xf) << 4) | (((uint) bytes[1] - 0x30) & 0xf);
            cur2 = ((((uint) bytes[2] - 0x30) & 0xf) << 4) | (((uint) bytes[3] - 0x30) & 0xf);
            uint cur3 = ((((uint) bytes[4] - 0x30) & 0xf) << 4) | (((uint) bytes[5] - 0x30) & 0xf);
            adjust.adjust_detail[inx++] = cur1;
            adjust.adjust_detail[inx++] = cur2;
            adjust.adjust_detail[inx++] = cur3;

            value = data->fectElem(3);
            assert(value != nullptr);
            valueStr = value->rawInValue();
            while (valueStr.length() < 8) {
                valueStr.append(' ');
            }

            bytes = valueStr.toLatin1();
            adjust.adjust_detail[inx++] = bytes[0];
            adjust.adjust_detail[inx++] = bytes[1];
            adjust.adjust_detail[inx++] = bytes[2];
            adjust.adjust_detail[inx++] = bytes[3];
            adjust.adjust_detail[inx++] = bytes[4];
            adjust.adjust_detail[inx++] = bytes[5];
            adjust.adjust_detail[inx++] = bytes[6];
            adjust.adjust_detail[inx++] = bytes[7];

            value = data->fectElem(4);
            assert(value != nullptr);
            adjust.adjust_detail[inx++] = value->rawInValue().trimmed().toInt(&ok);
        }
    }
#endif

    doAdjust(&adjust, data, fact);
}

void VehicleCommand::sendAdjustCommand(RCAdjustItem *data, FactDecoderMgr *fact) {

    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    ControlAdjustProto adjust;

#if defined(AFC21_PROJECT_CONF)
    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.cmd_type = decoder.AdjustControlType;
    adjust.adjust_type = decoder.adjustCommand;
    adjust.adjust_code = data->mData()->itemNum();
    uint32_t value = data->mData()->value();
    for (int i = 0; i < data->mData()->byteCount() / 8; i++) {
        adjust.adjust_detail[i] = (value >> (8 * i)) & 0xff;
    }
#elif defined(AFXF_PROJECT_CONF)
    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.length = sizeof(ControlAdjustProto);
    adjust.number = (++_cmdNumber) % 256;
    adjust.cmd_type = decoder.AdjustCommandType;
    adjust.adjust_type = decoder.adjustCommand;
    adjust.adjust_code = data->itemNum();
    adjust.dst_addr = fact->getVehicleAddr();
    adjust.src_addr = 0xff;

    FactListModel *moreMeta = data->moreMetaModel();
    if (moreMeta->count() > 1) {
        int imx = 0;
        const FactMetaData *metaData = nullptr;
        for (int ix = 0; ix < moreMeta->count(); ix++) {
            metaData = data->fectElem(ix);
            if (metaData->isEnum()) {
                int val = metaData->enumSelect();
                adjust.adjust_detail[imx++] = metaData->enumValue(val) & 0xff;
            } else {
                uint32_t value = metaData->value();
                uint32_t byteCount = metaData->byteCount() / 8;
                for (int i = 0; i < byteCount; i++) {
                    adjust.adjust_detail[imx++] = (value >> (8 * i)) & 0xff;
                }
            }
        }
    } else {
        int enumVal = data->selectId();
        if (enumVal >= 0) {
            const FactMetaData *mData = data->mData();
            int val = mData->enumValue(enumVal);
            adjust.adjust_detail[0] = val;
        } else {
            uint32_t value = data->value();
            uint32_t byteCount = data->byteCount();
            for (int i = 0; i < byteCount; i++) {
                adjust.adjust_detail[i] = (value >> (8 * i)) & 0xff;
            }
        }
    }

    fillProtoTime(adjust);

#elif defined(WHXQ_PROJECT_CONF)
    memset(&adjust, 0, sizeof(ControlAdjustProto));
    adjust.magic1 = decoder.ProtoSplitWordA;
    adjust.magic2 = decoder.ProtoSplitWordB;
    adjust.length = sizeof(ControlAdjustProto);
    adjust.number = (++_cmdNumber) % 256;
    adjust.cmd_type = decoder.AdjustCommandType;
    adjust.adjust_type = decoder.adjustCommand;
    adjust.adjust_code = data->itemNum();
    FactListModel *moreMeta = data->moreMetaModel();
    if (moreMeta->count() > 1) {
        int imx = 0;
        const FactMetaData *metaData = nullptr;
        for (int ix = 0; ix < moreMeta->count(); ix++) {
            metaData = data->fectElem(ix);
            if (metaData->isEnum()) {
                int val = metaData->enumSelect();
                adjust.adjust_detail[imx++] = metaData->enumValue(val) & 0xff;
            } else {
                uint32_t value = metaData->value();
                uint32_t byteCount = metaData->byteCount() / 8;
                for (int i = 0; i < byteCount; i++) {
                    adjust.adjust_detail[imx++] = (value >> (8 * i)) & 0xff;
                }
            }
        }
    } else {
        int enumVal = data->selectId();
        if (enumVal >= 0) {
            const FactMetaData *mData = data->mData();
            int val = mData->enumValue(enumVal);
            adjust.adjust_detail[0] = val;
        } else {
            uint32_t value = data->value();
            uint32_t byteCount = data->byteCount();
            for (int i = 0; i < byteCount; i++) {
                adjust.adjust_detail[i] = (value >> (8 * i)) & 0xff;
            }
        }
    }

    fillProtoTime(adjust);

#endif

    doAdjust(&adjust, data, fact);
}

void VehicleCommand::doAdjust(ControlAdjustProto *adjust, RCAdjustItem *data, FactDecoderMgr *fact) {

    adjust->dst_addr = fact->getVehicleId();
    adjust->feedback = data->feedback();

    ProtoCRC8((uint8_t *) adjust, sizeof(ControlAdjustProto), true);

    PtrProtoMessage m1 = newControlAdjustProto(*adjust);
    PtrProtoMessage m2 = newControlAdjustProto(*adjust);
    PtrProtoMessage m3 = newControlAdjustProto(*adjust);

    if ((m1 != nullptr) && !serial_write_cache.enqueue(m1)) {
        delete m1; //
    }
    if ((m2 != nullptr) && !serial_write_cache.enqueue(m2)) {
        delete m2; //
    }
    if ((m3 != nullptr) && !serial_write_cache.enqueue(m3)) {
        delete m3; //
    }

    PtrProtoMessage m11 = newControlAdjustProto(*adjust);
    PtrProtoMessage m21 = newControlAdjustProto(*adjust);
    PtrProtoMessage m31 = newControlAdjustProto(*adjust);

    if ((m11 != nullptr) && !main_write_cache.enqueue(m11)) {
        delete m11; //
    }

    if ((m21 != nullptr) && !main_write_cache.enqueue(m21)) {
        delete m21; //
    }

    if ((m31 != nullptr) && !main_write_cache.enqueue(m31)) {
        delete m31; //
    }

    PtrProtoMessage m12 = newControlAdjustProto(*adjust);
    PtrProtoMessage m22 = newControlAdjustProto(*adjust);
    PtrProtoMessage m32 = newControlAdjustProto(*adjust);

    if ((m12 != nullptr) && !slave_write_cache.enqueue(m12)) {
        delete m12; //
    }

    if ((m22 != nullptr) && !slave_write_cache.enqueue(m22)) {
        delete m22; //
    }

    if ((m32 != nullptr) && !slave_write_cache.enqueue(m32)) {
        delete m32; //
    }
}
