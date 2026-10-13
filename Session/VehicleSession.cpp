#include "VehicleSession.h"
#include "ConfigController.h"
#include "log_category.h"
#include "prototelemetry.h"
#include "stringtranslate.h"
#include <QNetworkDatagram>
#include <QTextCodec>
#include <filesystem>

#define AdjustVerifyOutFreq  10
#define DATAPROCESS_ACCURACY 10
#define DATAPROCESS_DELAY_MS 10

void VehicleSession::registerType() {
    qmlRegisterUncreatableType<FlightLanding>("FlightLanding", 1, 0, "FlightLanding", "Reference only");
    qmlRegisterUncreatableType<FlightLandCurve>("FlightLandCurve", 1, 0, "FlightLandCurve", "Reference only");
    qmlRegisterUncreatableType<AppLocateConfig>("AppLocateConfig", 1, 0, "AppLocateConfig", "Reference");
    qmlRegisterUncreatableType<DataLinkerSummary>("DataLinkerSummary", 1, 0, "DataLinkerSummary", "Reference");
    qmlRegisterUncreatableType<NetworkNodeConfig>("NetworkNodeConfig", 1, 0, "NetworkNodeConfig", "Reference");
    qmlRegisterUncreatableType<SoftwareCommConfig>("SoftwareCommConfig", 1, 0, "SoftwareCommConfig", "Reference");
    qmlRegisterUncreatableType<SerialNodeConfig>("SerialNodeConfig", 1, 0, "SerialNodeConfig", "Reference");
    qmlRegisterUncreatableType<ConfigController>("ConfigController", 1, 0, "ConfigController", "Reference only");
    qmlRegisterUncreatableType<VehicleSession>("VehicleSession", 1, 0, "VehicleSession", "Reference only");
    qmlRegisterUncreatableType<VehicleCommand>("VehicleCommand", 1, 0, "VehicleCommand", "Reference only");
    qmlRegisterType<ChannelSelection>("ChannelSelection", 1, 0, "ChannelSelection");
    qmlRegisterType<GcsStationRole>("GcsStationRole", 1, 0, "GcsStationRole");
}

void VehicleSession::switchStationControlRole() { switchStationRoleMsg(); }

void VehicleSession::switchStationTaskRole() { switchStationRoleMsg(); }

VehicleSession::VehicleSession(ConfigController *config)
    : configCtrl(config), _vehicleIndex(0),
      vehAdjustControl(serial_write_cache, mainnet_write_cache, slavenet_write_cache) {
    currDatetime = QDateTime::currentDateTime();
    next_keepalive = 0;
}

VehicleSession::~VehicleSession() { stopService(); }

void VehicleSession::lanchProcessSignal() {
    //    std::unique_lock<std::mutex> lck(process_mutex);
    //    process_condition.notify_one();
}

void VehicleSession::sendControlCommand(RCCommandItem *data) {
    FactDecoderMgr *fact = VehicleSession::protoManager(_vehicleIndex);
    vehAdjustControl.sendControlCommand(data, fact);
    lanchProcessSignal();
}

void VehicleSession::sendMissionCommand(RCAdjustItem *data) {
    FactDecoderMgr *fact = VehicleSession::protoManager(_vehicleIndex);
    vehAdjustControl.sendMissionCommand(data, fact);
    lanchProcessSignal();
}

void VehicleSession::sendDeviceCommand(RCAdjustItem *data) {
    FactDecoderMgr *fact = VehicleSession::protoManager(_vehicleIndex);
    vehAdjustControl.sendDeviceCommand(data, fact);
    lanchProcessSignal();
}

void VehicleSession::sendSimpleCommand(RCAdjustItem *data) {
    FactDecoderMgr *fact = VehicleSession::protoManager(_vehicleIndex);
    vehAdjustControl.sendSimpleCommand(data, fact);
    lanchProcessSignal();
}

void VehicleSession::sendDeviceOnOffCommand(RCAdjustItem *data) {
    FactDecoderMgr *fact = VehicleSession::protoManager(_vehicleIndex);
    if (_currentVehicle != nullptr) {
        int subItemVal = data->selectId();
        const FactMetaData *mData = data->fectElem(subItemVal);
        if ((mData != nullptr)) {
            FlightDisplay *display = nullptr;
            display = _currentVehicle->getDeviceByName(mData->groupName());
            if (display != nullptr) {
                QStringList mlist = mData->enumValStr();
                int mx = mData->enumSelect();
                display->setDisplay(mlist[mx]);
                cmdOperateQueue.enqueue(display);
            }
        }
    }

    vehAdjustControl.sendDeviceOnOffCommand(data, fact);
    lanchProcessSignal();
}

void VehicleSession::sendDeviceSetCommand(RCAdjustItem *data, QString value) {
    FactDecoderMgr *fact = VehicleSession::protoManager(_vehicleIndex);
    vehAdjustControl.sendDeviceSetCommand(data, value, fact);
    lanchProcessSignal();
}

void VehicleSession::sendAdjustCommand(RCAdjustItem *data) {
    FactDecoderMgr *fact = VehicleSession::protoManager(_vehicleIndex);
    vehAdjustControl.sendAdjustCommand(data, fact);
    lanchProcessSignal();
}

void VehicleSession::sendAdjustATCCommand(RCAdjustItem *item, int value) {
    assert(item != nullptr);
    item->setWorking(true);

    FactDecoderMgr *fact = VehicleSession::protoManager(_vehicleIndex);

    if (value <= 0) {
        vehAdjustControl.sendAdjustATCCommand(item, fact);
    } else {
        vehAdjustControl.sendAdjustATCSpecial(item, fact, value);
    }

    lanchProcessSignal();
}

void VehicleSession::mockRTKDataFrame() {
    NetworkNodeConfig *net = configCtrl->dataTransfer();
    if (net != nullptr && !net->mockData()) {
        return;
    }
    static uint8_t cmdNumber = 0;
    int dataLen = rand() % sizeof(RockerDeviceProto::datas) + 1;
    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    PtrRockerMessage msg = new RockerDeviceProto();
    assert(msg != nullptr);
    memset(msg, 0, sizeof(RockerDeviceProto));
    msg->magic1 = decoder.ProtoSplitWordA;
    msg->magic2 = decoder.ProtoSplitWordB;
    msg->length = sizeof(RockerDeviceProto);
    msg->number = (++cmdNumber) % 256;
    msg->cmd_type = decoder.RTKTransferFrame;
    FillProtoTime(msg);
    msg->dlen = dataLen;
    memset(msg->datas, cmdNumber, dataLen);
    ProtoCRC8((uint8_t *) msg, sizeof(RockerDeviceProto), true);
    serial_rtk_cache.enqueue(msg);
}

void VehicleSession::addSendingCallback(MsgSendingCallback callback) {
    assert(callback != nullptr);
    msgSendHandlers.append(callback);
}

void VehicleSession::addProcessCallback(MsgProcessCallback callback) {
    assert(callback != nullptr);
    msgProcessHandlers.append(callback);
}

void VehicleSession::addFeedbackCallback(FeedbackCallback callback) {
    assert(callback != nullptr);
    feedbackHandlers.append(callback);
}

FactDecoderMgr *VehicleSession::createProtoDecoder(uint8_t id) {
    std::shared_ptr<FactDecoderMgr> protoManger(new FactDecoderMgr(id, currDatetime));
    if (!protoManger->loadExcelProto(metaDataFile)) {
        COMMON_INFO("load excel proto {} failed", metaDataFile.toStdString().c_str());
        return nullptr;
    }

    veh_proto_manager.insert(id, protoManger);
    return protoManger.get();
}

void VehicleSession::verifyFeedback(FlightVechicle *vehicle, FactTableModel *feedTable) {
    FeedbackPositionInfo &info = GetFeedbackPositionInfo();
    auto ctrlIter = feedback_verify.begin();
    while (ctrlIter != feedback_verify.end()) {
        FactMetaData *back1 = nullptr;
        FactMetaData *back2 = nullptr;
        FactMetaData *back3 = nullptr;

        back1 = feedTable != nullptr ? feedTable->at(info.ControlFeedbackPos) : nullptr;
        back2 = feedTable != nullptr ? feedTable->at(info.AdjustFeedbackPos) : nullptr;

        if (info.VerifyFeedbackSeq > 0) {
            back3 = feedTable != nullptr ? feedTable->at(info.VerifyFeedbackSeq) : nullptr;
        }

        int feedValue1 = back1 != nullptr ? back1->display(_net_selected).toInt() : -1;
        int feedValue2 = back2 != nullptr ? back2->display(_net_selected).toInt() : -1;
        int feedValue3 = back3 != nullptr ? back3->display(_net_selected).toInt() : -1;

        if (info.VerifyFeedbackSeq > 0) {
            if ((feedValue1 > 0 || feedValue2 > 0) && (feedValue3 >= 0) &&
                ((*ctrlIter)->vechile() == vehicle->vehicleIdx())) {
                (*ctrlIter)->setFeedback(feedValue1, feedValue2, feedValue3);
            }
        } else {
            if ((feedValue1 > 0 || feedValue2 > 0) && ((*ctrlIter)->vechile() == vehicle->vehicleIdx())) {
                (*ctrlIter)->setFeedback(feedValue1, feedValue2);
            }
        }

        ++ctrlIter;
    }
}

void VehicleSession::updateCommandAdjust(int vehKey, std::shared_ptr<FactDecoderMgr> value) {
    ProtoDecoderInfo &conf = GetProtoDecoderInfo();
    FlightVechicle *vehicle = serachVehicle(vehKey);
    assert(vehicle != nullptr);

    if ((value != nullptr) && (vehicle != nullptr)) {
        verifyFeedback(vehicle, value->protoframe(conf.VehicleFeedbackFrame));
    }
}

void VehicleSession::verifyCommandAdjust(int value) {
    std::lock_guard<std::mutex> lock(command_mutex);
    ProtoDecoderInfo &conf = GetProtoDecoderInfo();
    FeedbackPositionInfo &info = GetFeedbackPositionInfo();

    auto get = veh_proto_manager.begin();
    while ((value > 0) && (get != veh_proto_manager.end())) {
        FlightVechicle *vehicle = serachVehicle(get.key());
        assert(vehicle != nullptr);
        auto posTable = get.value()->protoframe(conf.VehiclePositionFrame);
        auto feedTable = get.value()->protoframe(conf.VehicleFeedbackFrame);
        //  mission module, by the control & adjust proto, those mission messages have been
        //  sended to vehicle, there are needed to check those messages have been received by
        //  vehicle or not.
        FactMetaData *back1 = nullptr;
        FactMetaData *back2 = nullptr;
        back1 = feedTable != nullptr ? feedTable->at(info.ControlFeedbackPos) : nullptr;
        back2 = feedTable != nullptr ? feedTable->at(info.AdjustFeedbackPos) : nullptr;
        int feedValue1 = back1 != nullptr ? back1->display(_net_selected).toInt() : -1;
        int feedValue2 = back2 != nullptr ? back2->display(_net_selected).toInt() : -1;

        auto xm = feedbackHandlers.begin();
        for (; (feedValue1 > 0 || feedValue2 > 0) && xm != feedbackHandlers.end(); xm++) {
            if ((*xm)(feedValue1, feedValue2)) {
                COMMON_INFO("feedbackHandlers {}  {}", veh_proto_manager.size(), value);
                break;
            }
        }
        // update vehicle position and head
        int result = (1 << get.key()) & value;
        if (result >= 0 && (vehicle != nullptr) && (posTable != nullptr) && (feedTable != nullptr)) {
            FactMetaData *mData = posTable->at(info.ProtoPositionHead);
            FactMetaData *lng = posTable->at(info.ProtoPositionLng);
            FactMetaData *lat = posTable->at(info.ProtoPositionLat);
            FactMetaData *alt = posTable->at(info.ProtoPositionAlt);

            vehicle->setHeading(mData->display(_net_selected).toDouble());

            if (lng != nullptr && lat != nullptr && alt != nullptr) {
                vehicle->uptVechiclePos(lng->display(_net_selected).toDouble(),
                                        lat->display(_net_selected).toDouble(),
                                        alt->display(_net_selected).toDouble());
            }

            verifyFeedback(vehicle, feedTable);
        }
        //
        ++get;
    }

    auto ctrlIter = feedback_verify.begin();
    while (ctrlIter != feedback_verify.end()) {
        int rtn = (*ctrlIter)->isExpectValue(vehAdjustControl.commands(), info.VerifyFeedbackSeq > 0 ? true : false);
        if (rtn & 0xf) { /// 判断是否正确执行备接受
            VehicleFeedback *current = feedbackByVehicle((*ctrlIter)->vechile());
            current->updateFeedback((*ctrlIter)->expectName(), (*ctrlIter)->verifyRow(), (*ctrlIter)->duringTime(),
                                    rtn == 0x01 ? true : false);
            COMMON_INFO("vehicle {} success, during {} ", (*ctrlIter)->vechile(), (*ctrlIter)->duringTime());
            ctrlIter = feedback_verify.erase(ctrlIter);
            /// 正常响应处理对象显示数据
            FlightDisplay *disp = nullptr;
            if (cmdOperateQueue.try_dequeue(disp) && (disp != nullptr)) {
                disp->afterUpdateDisplay();
            }
            emit verticalBarPos();
        } else if ((*ctrlIter)->isTimeout()) {
            VehicleFeedback *current = feedbackByVehicle((*ctrlIter)->vechile());
            current->updateFeedback((*ctrlIter)->expectName(), (*ctrlIter)->verifyRow(), (*ctrlIter)->duringTime(),
                                    false);
            COMMON_INFO("vehicle {} timeout, during {}, seq {}, adjust {}, control {}", (*ctrlIter)->vechile(),
                        (*ctrlIter)->duringTime(), (*ctrlIter)->feedbackSeq(), (*ctrlIter)->adjustCode(),
                        (*ctrlIter)->controlCode());
            ctrlIter = feedback_verify.erase(ctrlIter);
            /// 异常清除发送显示对象空间
            FlightDisplay *disp = nullptr;
            cmdOperateQueue.try_dequeue(disp);
            emit verticalBarPos();
        } else {
            ++ctrlIter;
        }
    }
}

void VehicleSession::channelNotAvail(ControlAdjustProto *nodeProto, const QString &msg) {
    assert(nodeProto != nullptr);
    if (nodeProto == nullptr) {
        COMMON_INFO("invalid param");
        return;
    }

    auto xm = veh_proto_manager.begin();
    FlightVechicle *vehicle = nullptr;
    while (xm != veh_proto_manager.end()) {
        uint32_t adr = xm.value()->getVehicleAddr();
        if (adr == AircraftAddress(nodeProto)) {
            vehicle = serachVehicle(xm.value()->getVehicleId());
            break;
        }
        ++xm;
    }

    if (vehicle != nullptr) {
        emit addWarnLevel(vehicle->vehicleIdx(), LevelOne, 0, msg);
    }
}

bool VehicleSession::controlAdjustProto(PtrMessage msg) {

    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    ControlAdjustProto *nodeProto = (ControlAdjustProto *) msg->buffer;
    assert(nodeProto != nullptr);

    if (nodeProto->cmd_type == decoder.keepaliveCommand) {
        deleteProtoMessage(msg);
        return true;
    }

    auto xm = veh_proto_manager.begin();
    FlightVechicle *vehicle = nullptr;
    while (xm != veh_proto_manager.end()) {
        uint32_t adr = xm.value()->getVehicleAddr();
        if (adr == AircraftAddress(nodeProto)) {
            vehicle = serachVehicle(xm.value()->getVehicleId());
            break;
        }
        ++xm;
    }

    std::shared_ptr<ControlAdjustVerify> verify = nullptr;
    verify = vehAdjustControl.feedbackVerify(vehicle, nodeProto);
    if (verify != nullptr) {
        std::lock_guard<std::mutex> lock(command_mutex);
        COMMON_INFO("add command, seq {}, adjust {}, control {}", verify->commandSeq(), verify->adjustCode(),
                    verify->controlCode());
        feedback_verify.push_back(verify);
        emit verticalBarPos();
    }

    deleteProtoMessage(msg);
    return true;
}

int VehicleSession::autoSelectLinker() {
    if (_net_selected == ChannelSelection::AllNetwork) {
        int slaveQ = slave_linker_summary.quality();
        int mainQ = main_linker_summary.quality();
        return slaveQ > mainQ ? ChannelSelection::SlaveNetwork : ChannelSelection::MainNetwork;
    }
    return _net_selected;
}

void VehicleSession::calculateLinkQuality(int timeVal) {
    if (tm_linker_quality <= 0) {
        tm_linker_quality = currentTimeMs() + 1000;
        return;
    }

    if (currentTimeMs() < tm_linker_quality) {
        return;
    }

    tm_linker_quality = currentTimeMs() + 1000;
    if (configCtrl == nullptr) {
        return;
    }

    SerialNodeConfig *net = configCtrl->serailNode();
    NetworkNodeConfig *netmain = configCtrl->mainNetNode();
    NetworkNodeConfig *netslave = configCtrl->slaveNetNode();
    main_linker_summary.calculateQuality(netmain);
    slave_linker_summary.calculateQuality(netslave);
    serial_linker_summary.calculateQuality(net);
}

void VehicleSession::decodeSoftComm(SoftwareCommProto *msg) {
    assert(msg != nullptr);
    if (msg == nullptr) {
        return;
    }

    QString hex = ToHexStr((uint8_t *) msg, sizeof(SoftwareCommProto));
    if (ProtoCRC8((uint8_t *) msg, sizeof(SoftwareCommProto), false) != 0) {
        COMMON_INFO("error package {} ", hex.toStdString());
        return;
    }

    // 判断切换地面站角色是否已经执行
    if (_station_switch == msg->special) {
        COMMON_INFO("switch station role  {}, message: {}", _station_role.load(), hex.toStdString().c_str());
        emit stationRoleChanged();
        return; //
    }

    int lastrole = _station_role.load();
    _station_switch = msg->special;
    switch (msg->ident) {
    case SWITCH_STATION_ROLE: {
        if (_station_role == GcsStationRole::ControlRole) {
            _station_role = GcsStationRole::TaskRole;
        } else if (_station_role == GcsStationRole::TaskRole) {
            _station_role = GcsStationRole::ControlRole;
        }
        COMMON_INFO("station role {} switch to {}, message: {}", lastrole, _station_role.load(),
                    hex.toStdString().c_str());
        emit stationRoleChanged();
        break;
    }
    }
}

void VehicleSession::switchStationRoleMsg() {
    SoftwareCommProto softProto;
    srand(currentTimeMs());             // rand
    int randvalue = rand() % 65535 + 1; //
    memset(&softProto, 0, sizeof(softProto));
    softProto.magic1 = GCS_COMM_MAGIC_WORDA;
    softProto.magic2 = GCS_COMM_MAGIC_WORDB;
    softProto.length = sizeof(SoftwareCommProto);
    softProto.number = randvalue % 255;
    softProto.devid = 0xff;
    softProto.ident = SWITCH_STATION_ROLE;
    softProto.detail = _station_role;
    softProto.special = randvalue;
    softProto.feedback = 0;
    ProtoCRC8((uint8_t *) &softProto, sizeof(SoftwareCommProto), true);
    COMMON_INFO("switch station role {}", _station_role.load());
    PtrSoftwareMessage softProto1 = new SoftwareCommProto();
    PtrSoftwareMessage softProto2 = new SoftwareCommProto();
    // softcomm_write_cache.enqueue(softProto);
    if (softProto1 != nullptr) {
        memcpy(softProto1, &softProto, sizeof(SoftwareCommProto));
        softcomm_proto_cache.enqueue(softProto1);
    }
    if (softProto2 != nullptr) {
        memcpy(softProto2, &softProto, sizeof(SoftwareCommProto));
        softcomm_proto_cache.enqueue(softProto2);
    }
}

///
/// \brief adjustControlProcess
///     一次性将遥测遥控数据全部发送完成
/// \param read_write
///     从指定的通道发送数据，发送数据采用阻塞式发送消息
///

bool VehicleSession::adjustControlProcess(Channel *read_write, ControlAdjustProtoQueue &cachequeue) {
    bool rtn = false;
    PtrProtoMessage tempProto = nullptr;
    if (read_write && read_write->IsConnected() && cachequeue.try_dequeue(tempProto) && (tempProto != nullptr)) {
        /// if the role of the current station is not 'ControlRole',
        /// don't send anything message to vehicle..
        ///
        if (_station_role != GcsStationRole::ControlRole) {
            ControlAdjustProto *frame = (ControlAdjustProto *) tempProto;
            if (frame != nullptr) {
                delete frame;
            }
            // continue;
            return true;
        }

        transExtenMsg(read_write, tempProto);

        /// 消息发送结束后，通过回调函数清除发送数据的内存空间
        read_write->Send((char *) tempProto, sizeof(*tempProto), [=](int status, char *buf, int len) {
            ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
            ControlAdjustProto *frame = (ControlAdjustProto *) buf;
            /// 如果消息不是心跳消息，输出发送消息的内容到日志
            if (frame->ctrl_type != decoder.keepaliveCommand) {
                QString hex = ToHexStr((uint8_t *) frame, sizeof(ControlAdjustProto));
                COMMON_INFO("adjust & control command: {} ", hex.toStdString().c_str());
            }
            // if (status != len) {
            //     QString msg = QString::fromStdString(read_write->configmain.address);
            //     msg.append(" "); // add space
            //     msg.append(uv_err_name(status));
            //     channelNotAvail(frame, msg);
            // }
            /// 删除消息体存储空间
            if (frame != nullptr) {
                delete frame;
            }
        });

        rtn = true;
    }

    return rtn;
}

void VehicleSession::enableUpKeepalive() { _upload_keepalive = true; }

void VehicleSession::transExtenMsg(Channel *channel, ControlAdjustProto *proto) {
    if ((proto != nullptr) && (channel != nullptr) && configCtrl && configCtrl->transExtentMode()) {
        // int extMark = configCtrl->transExtMark();
        // ControlAdjustExtent extentMsg; //
        // memset(&extentMsg, 0, sizeof(extentMsg));
        // extentMsg.magic1 = proto->magic1;
        // extentMsg.magic2 = proto->magic2;
        // extentMsg.type1 = (extMark >> 16) & 0xff;
        // extentMsg.type2 = (extMark >> 8) & 0xff;
        // extentMsg.type3 = (extMark >> 0) & 0xff;
        // int offset = common_offsetoff(ControlAdjustProto, cmd_type);
        // char * paddr = (char *)proto; // proto msg address
        // memcpy(extentMsg.RC, paddr + offset, 42);
        // uint8_t * extaddr = (uint8_t*)&extentMsg;
        // ProtoCRC8Ext(extaddr+2, sizeof(extentMsg)-2, true);
        // channel->Send((char *) &extentMsg, sizeof(extentMsg), nullptr);
        // QString hex = ToHexStr((uint8_t *) &extentMsg, sizeof(extentMsg));
        // COMMON_INFO("trans exten command: {} ", hex.toStdString().c_str());
    }
}

bool VehicleSession::keepalive(Channel *mainNet, Channel *slaveNet) {
    // if (_station_role != GcsStationRole::ControlRole) {
    //     return false;
    // }

    static int countkeepalive = 0;

    NetworkNodeConfig *netslave = configCtrl->slaveNetNode();
    NetworkNodeConfig *netmain = configCtrl->mainNetNode();

    ControlAdjustProto keepalive1;
    vehAdjustControl.networkKeepalive(false, keepalive1, ChannelSelection::MainNetwork);

    ControlAdjustProto keepalive2;
    vehAdjustControl.networkKeepalive(true, keepalive2, ChannelSelection::SerialNetwork);

    if (netmain->enable() && (mainNet != nullptr)) {

        transExtenMsg(mainNet, &keepalive1);

        mainNet->Send((char *) &keepalive1, sizeof(keepalive1), [=](int status, char *buf, int len) {
            ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
            ControlAdjustProto *frame = (ControlAdjustProto *) buf;
            if (++countkeepalive % 51 == 0) {
                QString hex = ToHexStr((uint8_t *) frame, sizeof(ControlAdjustProto));
                COMMON_INFO("keepalive command: {} ", hex.toStdString().c_str());
            }
            // if (status != len) {
            //     QString msg = QString::fromStdString(mainNet->configmain.address);
            //     msg.append(" "); // add space
            //     msg.append(uv_err_name(status));
            //     channelNotAvail(frame, msg);
            // }
        });
    }

    if (netslave->enable() && (slaveNet != nullptr)) {

        transExtenMsg(mainNet, &keepalive1);

        slaveNet->Send((char *) &keepalive2, sizeof(keepalive2), [=](int status, char *buf, int len) {
            ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
            ControlAdjustProto *frame = (ControlAdjustProto *) buf;
            if (++countkeepalive % 50 == 0) {
                QString hex = ToHexStr((uint8_t *) frame, sizeof(ControlAdjustProto));
                COMMON_INFO("keepalive command: {} ", hex.toStdString().c_str());
            }
            // if (status != len) {
            //     QString msg = QString::fromStdString(slaveNet->configmain.address);
            //     msg.append(" "); // add space
            //     msg.append(uv_err_name(status));
            //     channelNotAvail(frame, msg);
            // }
        });
    }

    return true;
}

void VehicleSession::networkSend(Channel *mainNet, Channel *slaveNet) {
    int64_t updateTm = currentTimeMs();
    int selectChl = _net_selected;
    /// 心跳消息构建与下一次心跳消息的发送时间计算
    if ((updateTm > next_keepalive) && (configCtrl != nullptr)) {
        int freq = configCtrl->keepaliveFreq();
        if (freq < 1 || freq > 50) {
            freq = 1;
        }

        if (configCtrl != nullptr) {
            configCtrl->updateRunTime();
        }

        next_keepalive = updateTm + 1000 / freq;
        if (selectChl == ChannelSelection::MainNetwork) {
            main_linker_summary.working(true);
            slave_linker_summary.working(false);
            serial_linker_summary.working(false);
        }
        if (selectChl == ChannelSelection::SlaveNetwork) {
            slave_linker_summary.working(true);
            main_linker_summary.working(false);
            serial_linker_summary.working(false);
        }
        if (selectChl == ChannelSelection::SerialNetwork) {
            slave_linker_summary.working(false);
            main_linker_summary.working(false);
            serial_linker_summary.working(true);
        }

        calculateLinkQuality(updateTm / 1000);

        /// 是否心跳设置
        if (configCtrl->keepaliveFreq() > 0) {
            keepalive(mainNet, slaveNet);
        }

        emit checkWarnMessage();

        mockRTKDataFrame();
    }

    /// 是否有快捷命令消息
    ///
    RCCommandItem *cmd = nullptr;
    if ((cmd = win_shortcut.nextControl()) != nullptr) {
        sendControlCommand(cmd);
    }
    RCAdjustItem *adjst = nullptr;
    if ((adjst = win_shortcut.nextAdjust()) != nullptr) {
        sendSimpleCommand(adjst);
    }
    RCAdjustItem *adjstEx = nullptr;
    if ((adjstEx = win_shortcut.nextAdjustExt()) != nullptr) {
        sendAdjustCommand(adjstEx);
    }

    if ((softcomm_proto_cache.size_approx() > 0) && (mainNet != nullptr)) {
        SoftwareCommProto *tempProto = nullptr;
        if (softcomm_proto_cache.try_dequeue(tempProto) && (tempProto != nullptr)) {
            mainNet->Send((char *) tempProto, sizeof(SoftwareCommProto), [=](int status, char *buf, int len) {
                SoftwareCommProto *frame = (SoftwareCommProto *) buf;
                if (frame != nullptr) {
                    /// 如果消息不是心跳消息，输出发送消息的内容到日志
                    QString hex = ToHexStr((uint8_t *) frame, sizeof(SoftwareCommProto));
                    COMMON_INFO("software command: {} ", hex.toStdString().c_str());
                    delete frame; //
                    frame = nullptr;
                }
            });
        }
    }

    if ((softcomm_proto_cache.size_approx() > 0) && (slaveNet != nullptr)) {
        SoftwareCommProto *tempProto2 = nullptr;
        if (softcomm_proto_cache.try_dequeue(tempProto2) && (tempProto2 != nullptr)) {
            slaveNet->Send((char *) tempProto2, sizeof(SoftwareCommProto), [=](int status, char *buf, int len) {
                SoftwareCommProto *frame = (SoftwareCommProto *) buf;
                if (frame != nullptr) {
                    /// 如果消息不是心跳消息，输出发送消息的内容到日志
                    QString hex = ToHexStr((uint8_t *) frame, sizeof(SoftwareCommProto));
                    COMMON_INFO("software command: {} ", hex.toStdString().c_str());
                    delete frame; //
                    frame = nullptr;
                }
            });
        }
    }

    bool ctrladjust1 = adjustControlProcess(mainNet, mainnet_write_cache);
    bool ctrladjust2 = adjustControlProcess(slaveNet, slavenet_write_cache);

    /// 处理其他注册的消息handle,例如在任务系统需要执行相关的操作
    /// 任务类的操作之类的优先级低于控制与遥调之类
    if (selectChl == ChannelSelection::MainNetwork && (!ctrladjust1 && !ctrladjust2)) {
        for (auto xm = msgSendHandlers.begin(); xm != msgSendHandlers.end(); xm++) {
            (*xm)(mainNet, serial_mission_cache);
        }
    }
    if (selectChl == ChannelSelection::SlaveNetwork && (!ctrladjust1 && !ctrladjust2)) {
        for (auto xm = msgSendHandlers.begin(); xm != msgSendHandlers.end(); xm++) {
            (*xm)(slaveNet, serial_mission_cache);
        }
    }
    if (selectChl == ChannelSelection::SerialNetwork && (!ctrladjust1 && !ctrladjust2)) {
        for (auto xm = msgSendHandlers.begin(); xm != msgSendHandlers.end(); xm++) {
            (*xm)(nullptr, serial_mission_cache);
        }
    }

    /// 任务后处理接口
    dataAfterProcess(mainNet, slaveNet, _net_selected);
}

int VehicleSession::dataAnalysis(Channel *mainNet, Channel *slaveNet, ProtoMessage *msg){
    int vehUpdate = 0;
    if (msg == nullptr || mainNet == nullptr || slaveNet == nullptr) {
        return vehUpdate;
    }

    sockaddr_in dataChl;
    dataChl.sin_addr.S_un.S_addr = msg->address;
    dataChl.sin_port = msg->port;

           /// 将系统全部遥测遥控数据存储到数据文件中
    if (msg->length != sizeof(RockerDeviceProto)) {
        saveService(msg->channel, &dataChl, msg->buffer, msg->length);
    }

    bool isDel = false;
    /// 处理外部设置消息回调函数接口，如果消息被处理后，即消息被删除，回调函数返回真
    /// 如果消息被删除，或消息提为空则处理下一条消息
    for (auto xm = msgProcessHandlers.begin(); xm != msgProcessHandlers.end(); xm++) {
        isDel = (*xm)(msg);
        if (isDel || msg == nullptr) {
            continue;
        }
    }

           /// 软件之间通信数据对象
    if (msg->length == sizeof(SoftwareCommProto)) {
        decodeSoftComm((SoftwareCommProto *) msg->buffer);
        deleteProtoMessage(msg);
        isDel = true;
    }
    /// 判断消息是否是摇杆消息或数据转发消息
    else if (msg->length == sizeof(RockerDeviceProto)) {
        deleteProtoMessage(msg);
        isDel = true;
    } else {
        /// 判断消息是否是遥控消息或遥测消息
        ///
        ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
        ControlAdjustProto *nodeProto = (ControlAdjustProto *) msg->buffer;
        if (msg->length == sizeof(ControlAdjustProto) && (nodeProto->cmd_type == decoder.AdjustCommandType ||
                                                          nodeProto->cmd_type == decoder.ControlCommandType ||
                                                          nodeProto->cmd_type == decoder.keepaliveCommand)) {
            if (msg->channel == ChannelSelection::MainNetwork) {
                main_linker_summary.upState(true);
            }
            if (msg->channel == ChannelSelection::SlaveNetwork) {
                slave_linker_summary.upState(true);
            }
            isDel = controlAdjustProto(msg);
        }
    }

           /// 如果消息是遥测数据则需要填充数据的有效通道信息
    if (!isDel) {
        /// 统计信号的质量
        if (msg->channel == ChannelSelection::MainNetwork) {
            main_linker_summary.addRecvFrame(msg->length);
        }
        if (msg->channel == ChannelSelection::SlaveNetwork) {
            slave_linker_summary.addRecvFrame(msg->length);
        }
        if (msg->channel == ChannelSelection::SerialNetwork) {
            serial_linker_summary.addRecvFrame(msg->length);
        }
        /// 当前有效通道
        msg->select = _net_selected;
    }


    /// 调用解码库，根据消息类型进行数据解析
    auto first1 = veh_proto_manager.begin();
    while (!isDel && (first1 != veh_proto_manager.end())) {
        std::shared_ptr<FactDecoderMgr> factMgr = first1.value();
        if (factMgr && factMgr->protoDecoder(msg)) {
            vehUpdate = (1 << first1.key());
            deleteProtoMessage(msg);
            isDel = true;
            break;
        }
        ++first1;
    }

    if (!isDel) {
        deleteProtoMessage(msg);
    }

    return vehUpdate;
}

void VehicleSession::dataProcess(Channel *mainNet, Channel *slaveNet) {

    int vehUpdate = 0;
    int64_t updateTm = currentTimeMs();

    for (int ixm = 0; ixm < 50; ixm++) {
        ProtoMessage *msg1 = nullptr;
        ProtoMessage *msg2 = nullptr;

        if (data_write_cache.size_approx()>0 && data_write_cache.try_dequeue(msg1) && (msg1 != nullptr)) {
            vehUpdate |= dataAnalysis(mainNet, slaveNet, msg1);
        }

        if (data_read_cache.size_approx()>0 && data_read_cache.try_dequeue(msg2) && (msg2 != nullptr)) {
            vehUpdate |= dataAnalysis(mainNet, slaveNet, msg2);
        }
    }

    if ((updateTm > draw_updatefreq) && (configCtrl != nullptr)) {
        int val = configCtrl->dataUpdateFreq();
        int dtm = (val > 100) ? 100 : (val < 1 ? 1 : val);
        draw_updatefreq = updateTm + 1000 / dtm;
        main_linker_summary.notifyUpdate();
        slave_linker_summary.notifyUpdate();
        serial_linker_summary.notifyUpdate();
        updateDrawing();
    }

    /// 反馈命令消息处理
    verifyCommandAdjust(vehUpdate);

    /// 处理告警信息
    processSysWarn(vehUpdate, _net_selected);
}

void VehicleSession::processSysWarn(bool update, int selectChl) {

    /// 控制告警的频率
    // int64_t curTime = currentTimeMs()/1000;
    // auto mfilter = warningFilter.begin();
    // while (mfilter != warningFilter.end()){
    //     int64_t filter = (mfilter.value() >> 8) + 2000;
    //     if (curTime > filter) {
    //         mfilter = warningFilter.erase(mfilter);
    //     } else {
    //         ++mfilter;
    //     }
    // }

    if ((warn_updatefreq <= 0) || (currentTimeMs() > warn_updatefreq)) {
        warn_updatefreq = currentTimeMs() + 4000; /// 临时告警2秒过滤
        warningFilter.clear();
    }

    /// 告警信息处理
    auto get = veh_proto_manager.begin();
    while ((vehDataUpdate || update) && (get != veh_proto_manager.end())) {
        FlightVechicle *vehicle = serachVehicle(get.key());
        assert(vehicle != nullptr);
        if (vehicle == nullptr) {
            COMMON_INFO("not found vehicle by id {}", get.key());
            break;
        }
        /// 不同车辆有不同车辆地址
        int vehId = get.value()->getVehicleId();
        /// 特殊的车辆告警规则
        // vehicle->decodeWarnData(
        //     [&](PWarnElemNode node) {
        //         if (!thread_status) {
        //             return;
        //         }
        //         node->vehicle(vehId);
        //         vehicle_warn_export.add(node);
        //         emit addWarnLevel(vehId, node->level(), node->priority(), node->group());
        //     },
        //     [=](int frame) { return get.value()->protoframe(frame); }, selectChl);

        /// 一般车辆的告警信息通知
        get->get()->notifyDataUpdate(
            [&](PWarnElemNode node) {
                if (!thread_status) {
                    return;
                }
                node->vehicle(vehId);
                if (warningFilter.find(node->group()) == warningFilter.end()) {
                    int64_t filterTime = currentTimeMs() / 1000;
                    vehicle_warn_export.add(node); /// 记录告警信息
                    warningFilter.insert(node->group(), (filterTime << 8) | vehId);
                    emit addWarnLevel(vehId, node->level(), node->priority(), node->group());
                }
            },
            _net_selected);
        /// 下一个车辆告警信息遍历
        ++get;
    }

    vehDataUpdate = false;
}

void VehicleSession::calReadTimeout() {
    typedef std::chrono::milliseconds time_precision;
    typedef std::chrono::time_point<std::chrono::system_clock, time_precision> time_point_precision;
    time_point_precision tp = std::chrono::time_point_cast<time_precision>(std::chrono::system_clock::now());
    _timeoutCheck = tp.time_since_epoch().count() / 1000;
}

bool VehicleSession::isReadTimeout() {
    typedef std::chrono::milliseconds time_precision;
    typedef std::chrono::time_point<std::chrono::system_clock, time_precision> time_point_precision;
    time_point_precision tp = std::chrono::time_point_cast<time_precision>(std::chrono::system_clock::now());
    int mm = tp.time_since_epoch().count() / 1000;
    return std::abs(mm - _timeoutCheck) > 2000 ? true : false;
}

void VehicleSession::clearSerailResource() {

    PtrProtoMessage nodeProto = nullptr;
    PtrRockerMessage rtkProto = nullptr;
    PtrMissionMessage taskProto = nullptr;

    while (serial_write_cache.size_approx() > 0) {
        serial_write_cache.try_dequeue(nodeProto);
        delete nodeProto;
    }

    while (serial_rtk_cache.size_approx() > 0) {
        serial_rtk_cache.try_dequeue(rtkProto);
        delete rtkProto;
    }

    while (serial_mission_cache.size_approx() > 0) {
        serial_mission_cache.try_dequeue(taskProto);
        delete taskProto;
    }
}

static bool initSerialService(itas109::CSerialPort &main_serail, SimpleSerialListener &listener,
                              ConfigController *configCtrl) {
    SerialNodeConfig *net = configCtrl->serailNode();
    if (openSerialService(main_serail, listener, net)) {
        return true;
    }
    if (configCtrl != nullptr) {
        configCtrl->resetSerial();
    }
    return false;
}

void transExtenMsgBySerial(ConfigController *configCtrl, itas109::CSerialPort *channel, ControlAdjustProto *proto) {
    if ((configCtrl != nullptr) && (proto != nullptr) && (channel != nullptr) && configCtrl &&
        configCtrl->transExtentMode()) {
        int extMark = configCtrl->transExtMark();
        ControlAdjustExtent extentMsg; //
        memset(&extentMsg, 0, sizeof(extentMsg));
        extentMsg.magic1 = proto->magic1;
        extentMsg.magic2 = proto->magic2;
        extentMsg.type1 = (extMark >> 16) & 0xff;
        extentMsg.type2 = (extMark >> 8) & 0xff;
        extentMsg.type3 = (extMark >> 0) & 0xff;
        int offset = common_offsetoff(ControlAdjustProto, cmd_type);
        char *paddr = (char *) proto; // proto msg address
        memcpy(extentMsg.RC, paddr + offset, 42);
        uint8_t *extaddr = (uint8_t *) &extentMsg;
        ProtoCRC8Ext(extaddr + 2, sizeof(extentMsg) - 2, true);
        channel->writeData((char *) &extentMsg, sizeof(extentMsg));
    }
}

void VehicleSession::manualSerailCommand(ControlAdjustProto * newproto){
    SerialNodeConfig *net = configCtrl->serailNode();
    if (newproto != nullptr && net && net->enable()) {
        serial_write_cache.enqueue(newproto);
    } else if(newproto != nullptr){
        delete newproto;
        newproto = nullptr;
    }
}

void VehicleSession::startSerialService() {

    SerialNodeConfig *comconf = configCtrl->serailNode();
    COMMON_INFO("serail config {}, bautrate: {}, parity: {}, databit: {}, stopbit: {}",
                comconf->commName().toStdString(), comconf->bautrate(), comconf->parity(), comconf->databit(),
                comconf->stopbit());

    serail_thd = std::thread([=]() {
        itas109::CSerialPort main_serail;
        SimpleSerialListener listener(&main_serail, this->data_read_cache);

        int lastWriteCount = 0;
        int64_t lastReadTimestamp = 0;
        PtrProtoMessage nodeProto = nullptr;
        PtrRockerMessage rtkProto = nullptr;
        PtrMissionMessage taskProto = nullptr;
        SerialNodeConfig *net = configCtrl->serailNode();

        while (!serail_status.load() && net && net->enable()) {
            if (configCtrl && configCtrl->updateSerial()) {
                lastWriteCount = 0;
                /// 创建失败延迟1s后尝试进行
                clearSerailResource();
                if (!initSerialService(main_serail, listener, configCtrl)) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    continue;
                }
            }

            if (lastWriteCount-- > 0 && (nodeProto != nullptr || rtkProto != nullptr)) {
                writeSerailData<ControlAdjustProto>(main_serail, nodeProto, serial_linker_summary, lastWriteCount);
                writeSerailData<RockerDeviceProto>(main_serail, rtkProto, serial_linker_summary, lastWriteCount);
                writeSerailData<VehicleMissionProto>(main_serail, taskProto, serial_linker_summary, lastWriteCount);
            } else {
                if (serial_write_cache.size_approx() > 0 && serial_write_cache.try_dequeue(nodeProto)) {
                    transExtenMsgBySerial(configCtrl, &main_serail, (ControlAdjustProto *) nodeProto);
                    if (main_serail.writeData((char *) nodeProto, nodeProto->length) > 0) {
                        serial_linker_summary.upState(true);
                        lastWriteCount = 0;
                        delete nodeProto;
                    } else {
                        lastWriteCount = 3;
                    }
                } else if (serial_rtk_cache.size_approx() > 0 && serial_rtk_cache.try_dequeue(rtkProto)) {
                    if (main_serail.writeData((char *) rtkProto, rtkProto->length) > 0) {
                        serial_linker_summary.upState(true);
                        lastWriteCount = 0;
                        delete rtkProto;
                    } else {
                        lastWriteCount = 3;
                    }
                } else if (serial_mission_cache.size_approx() > 0 && serial_mission_cache.try_dequeue(taskProto)) {
                    if (main_serail.writeData((char *) taskProto, taskProto->length) > 0) {
                        serial_linker_summary.upState(true);
                        lastWriteCount = 0;
                        delete rtkProto;
                    } else {
                        lastWriteCount = 3;
                    }
                } else {
                    SerialNodeConfig *net = nullptr;
                    if (configCtrl && (net = configCtrl->serailNode()) != nullptr) {
                        int serialTimeout = net->timeout() > 1000 ? 1000 : net->timeout();
                        std::this_thread::sleep_for(std::chrono::milliseconds(serialTimeout));
                    }
                }
            }
        }

        if (net && net->enable()) {
            main_serail.flushWriteBuffers();
            main_serail.disconnectReadEvent();
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            main_serail.close();
        }

        clearSerailResource();
    });

    serail_status = false;
}

void VehicleSession::startMainNetService() {
    NetworkNodeConfig *netmain = configCtrl->mainNetNode();
    ChannelConfig &configmain = netmain->config();
    if (!netmain->enable()) {
        return;
    }

    main_read_write.Initialize(configmain, nullptr, true);
    main_read_write.Create(nullptr);

    connect(&main_read_write.sock_read, &QAbstractSocket::errorOccurred,
            [&](QAbstractSocket::SocketError error) { COMMON_INFO("main network result {}", (int) error); });

    connect(&main_read_write.sock_write, &QAbstractSocket::errorOccurred,
            [&](QAbstractSocket::SocketError error) { COMMON_INFO("main network result {}", (int) error); });

    connect(&main_read_write.sock_read, &QUdpSocket::readyRead, [this] {
        while (main_read_write.sock_read.hasPendingDatagrams()) {
            int pending = main_read_write.sock_read.pendingDatagramSize();
            if (pending > 0) {
                // int available = main_read_write.sock_read.bytesAvailable();
                // if (available > 0)
                {
                    int maxsize = qMin(pending, pending);
                    if (main_read_write.read_cache.length + maxsize >= PROTO_PACKAGE_LEN) {
                        COMMON_ERROR("network read buffer error");
                        FactDecoderMgr::dataDecoder(main_read_write.read_cache, &main_read_write.rw_multiRx_addr,
                                                    ChannelSelection::MainNetwork, data_read_cache);
                    }
                    uint8_t *buffer = (uint8_t *) main_read_write.read_cache.data;
                    buffer += main_read_write.read_cache.length;
                    int len = main_read_write.sock_read.readDatagram((char *) buffer, maxsize);
                    if (len > 0) {
                        // writeOriginData(cacheTelemetry[ChannelSelection::MainNetwork],
                        //                 _chlTelemetry[ChannelSelection::MainNetwork], buffer, len);
                        main_read_write.read_cache.length += len;
                        FactDecoderMgr::dataDecoder(main_read_write.read_cache, &main_read_write.rw_multiRx_addr,
                                                    ChannelSelection::MainNetwork, data_read_cache);
                    } else {
                        COMMON_ERROR("network read buffer error");
                    }
                }
            }
        }
    });

    connect(&main_read_write.sock_write, &QUdpSocket::readyRead, [this] {
        while (main_read_write.sock_write.hasPendingDatagrams()) {
            int pending = main_read_write.sock_write.pendingDatagramSize();
            if (pending > 0) {
                // int available = main_read_write.sock_write.bytesAvailable();
                // if (available > 0)
                {
                    int maxsize = qMin(pending, pending);
                    if (main_read_write.write_cache.length + maxsize >= PROTO_PACKAGE_LEN) {
                        COMMON_ERROR("network read buffer error");
                        FactDecoderMgr::dataDecoder(main_read_write.write_cache, &main_read_write.rw_multiTx_addr,
                                                    ChannelSelection::MainNetwork, data_write_cache);
                    }
                    uint8_t *buffer = (uint8_t *) main_read_write.write_cache.data;
                    buffer += main_read_write.write_cache.length;
                    int len = main_read_write.sock_write.readDatagram((char *) buffer, maxsize);
                    if (len > 0) {
                        // writeOriginData(cacheControl[ChannelSelection::MainNetwork],
                        //                 _chlControl[ChannelSelection::MainNetwork], buffer, len);
                        main_read_write.write_cache.length += len;
                        FactDecoderMgr::dataDecoder(main_read_write.write_cache, &main_read_write.rw_multiTx_addr,
                                                    ChannelSelection::MainNetwork, data_write_cache);
                    } else {
                        COMMON_ERROR("network read buffer error");
                    }
                }
            }
        }
    });

    // main_read_write.SetCloseCallback([&](long long value) { COMMON_INFO("channel closed {}", value); });
    // main_read_write.SetRecvCallback([&](const void *sockaddr, const uint8_t *databuf, int datalen) {
    //     assert(databuf != nullptr);
    //     if (savefile_status) {
    //         saveService(ChannelSelection::MainNetwork, sockaddr, databuf, datalen);
    //     }
    // });
    // main_read_write.SetDecoder([&](ProtoDataCache &decoder, const void *addr) {
    //     if (savefile_status) {
    //         FactDecoderMgr::dataDecoder(decoder, addr, ChannelSelection::MainNetwork, data_read_cache);
    //     } else {
    //         decoder.reset();
    //     }
    // });
    // main_read_write.Initialize(configmain, &sock_loop, netmain->localLoop());
    // bool ret = main_read_write.Create(&sock_loop);
    // COMMON_INFO("main network result {}", ret);
}

void VehicleSession::startSlaveNetService() {
    NetworkNodeConfig *netslave = configCtrl->slaveNetNode();
    ChannelConfig &configslave = netslave->config();

    if (!netslave->enable()) {
        return;
    }

    slave_read_write.Initialize(configslave, nullptr, true);
    slave_read_write.Create(nullptr);

    connect(&slave_read_write.sock_read, &QAbstractSocket::errorOccurred,
            [&](QAbstractSocket::SocketError error) { COMMON_INFO("slave network result {}", (int) error); });

    connect(&slave_read_write.sock_write, &QAbstractSocket::errorOccurred,
            [&](QAbstractSocket::SocketError error) { COMMON_INFO("slave network result {}", (int) error); });

    connect(&slave_read_write.sock_read, &QUdpSocket::readyRead, [&] {
        while (slave_read_write.sock_read.hasPendingDatagrams()) {
            int pending = slave_read_write.sock_read.pendingDatagramSize();
            if (pending > 0) {
                // int available = slave_read_write.sock_read.bytesAvailable();
                // if (available > 0)
                {
                    int maxsize = qMin(pending, pending);
                    if (slave_read_write.read_cache.length + maxsize >= PROTO_PACKAGE_LEN) {
                        COMMON_ERROR("network read buffer error");
                        FactDecoderMgr::dataDecoder(slave_read_write.read_cache, &slave_read_write.rw_multiRx_addr,
                                                    ChannelSelection::SlaveNetwork, data_read_cache);
                    }
                    uint8_t *buffer = (uint8_t *) slave_read_write.read_cache.data;
                    buffer += slave_read_write.read_cache.length;
                    int len = slave_read_write.sock_read.readDatagram((char *) buffer, maxsize);
                    if (len > 0) {
                        // writeOriginData(cacheTelemetry[ChannelSelection::SlaveNetwork],
                        //                 _chlTelemetry[ChannelSelection::SlaveNetwork], buffer, len);
                        slave_read_write.read_cache.length += len;
                        FactDecoderMgr::dataDecoder(slave_read_write.read_cache, &slave_read_write.rw_multiRx_addr,
                                                    ChannelSelection::SlaveNetwork, data_read_cache);
                    } else {
                        COMMON_ERROR("network read buffer error");
                    }
                }
            }
        }
    });

    connect(&slave_read_write.sock_write, &QUdpSocket::readyRead, [&] {
        while (slave_read_write.sock_write.hasPendingDatagrams()) {
            int pending = slave_read_write.sock_write.pendingDatagramSize();
            if (pending > 0) {
                // int available = slave_read_write.sock_write.bytesAvailable();
                // if (available > 0)
                {
                    int maxsize = qMin(pending, pending);
                    if (slave_read_write.write_cache.length + maxsize >= PROTO_PACKAGE_LEN) {
                        COMMON_ERROR("network read buffer error");
                        FactDecoderMgr::dataDecoder(slave_read_write.write_cache, &slave_read_write.rw_multiTx_addr,
                                                    ChannelSelection::SlaveNetwork, data_write_cache);
                    }
                    uint8_t *buffer = (uint8_t *) slave_read_write.write_cache.data;
                    buffer += slave_read_write.write_cache.length;
                    int len = slave_read_write.sock_write.readDatagram((char *) buffer, maxsize);
                    if (len > 0) {
                        // writeOriginData(cacheControl[ChannelSelection::SlaveNetwork],
                        //                 _chlControl[ChannelSelection::SlaveNetwork], buffer, len);
                        slave_read_write.write_cache.length += len;
                        FactDecoderMgr::dataDecoder(slave_read_write.write_cache, &slave_read_write.rw_multiTx_addr,
                                                    ChannelSelection::SlaveNetwork, data_write_cache);
                    } else {
                        COMMON_ERROR("network read buffer error");
                    }
                }
            }
        }
    });

    // slave_read_write.SetCloseCallback([&](long long value) { COMMON_INFO("channel closed {}", value); });
    // slave_read_write.SetRecvCallback([&](const void *sockaddr, const uint8_t *databuf, int datalen) {
    //     assert(databuf != nullptr);
    //     if (savefile_status) {
    //         saveService(ChannelSelection::SlaveNetwork, sockaddr, databuf, datalen);
    //     }
    // });
    // slave_read_write.SetDecoder([&](ProtoDataCache &decoder, const void *addr) {
    //     if (savefile_status) {
    //         FactDecoderMgr::dataDecoder(decoder, addr, ChannelSelection::SlaveNetwork, data_read_cache);
    //     } else {
    //         decoder.reset();
    //     }
    // });
    // slave_read_write.Initialize(configslave, &sock_loop, netslave->localLoop());
    // int ret = slave_read_write.Create(&sock_loop);
    // COMMON_INFO("slave network result {}", ret);
}

void VehicleSession::clearTxtSpeech() {
    ;
    COMMON_INFO("clear speech resource");
}

void VehicleSession::configService() {
    if (savefile_status) {
        COMMON_INFO("service is running..");
        return;
    }

    if (configCtrl == nullptr) {
        COMMON_ERROR("config node is null pointer");
        return;
    }

#if defined(AFC21_PROJECT_CONF)
    main_linker_summary.name(QString("U"));
    serial_linker_summary.name(QString("C"));
    slave_linker_summary.name(QString("L"));
#elif defined(AFXF_PROJECT_CONF)
    main_linker_summary.name(QString("L1"));
    serial_linker_summary.name(QString("C"));
    slave_linker_summary.name(QString("L2"));
#elif defined(WHXQ_PROJECT_CONF)
    main_linker_summary.name(QString("U"));
    serial_linker_summary.name(QString("C"));
    slave_linker_summary.name(QString("L"));
#endif

    if (configCtrl) {
        /// 配置系统软件角色
        _station_role = GcsStationRole::TaskRole;
        if (configCtrl->getSysConfigRole()) { ///
            _station_role = GcsStationRole::ControlRole;
        }
        emit stationRoleChanged();
    }

    std::filesystem::path path1("RawData");
    if (!std::filesystem::exists(path1)) {
        std::filesystem::create_directories(path1);
    }

    std::filesystem::path path2("RawData/rawdata");
    if (!std::filesystem::exists(path2)) {
        std::filesystem::create_directories(path2);
    }

    // if (std::filesystem::exists(path2)) {
    //     QString tmp = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");
    //     QString mainRc = QString("RawData/Origin/RCMain%1.data").arg(tmp);
    //     QString slaveRc = QString("RawData/Origin/RCSlave%1.data").arg(tmp);
    //     QString serialRc = QString("RawData/Origin/RCSerial%1.data").arg(tmp);
    //     QString mainChl = QString("RawData/Origin/MainTelemetry%1.data").arg(tmp);
    //     QString slaveChl = QString("RawData/Origin/SlaveTelemetry%1.data").arg(tmp);
    //     QString serialChl = QString("RawData/Origin/SerialTelemtry%1.data").arg(tmp);
    //     fopen_s(&_chlControl[ChannelSelection::MainNetwork], mainRc.toStdString().c_str(), "wb");
    //     fopen_s(&_chlControl[ChannelSelection::SlaveNetwork], slaveRc.toStdString().c_str(), "wb");
    //     fopen_s(&_chlControl[ChannelSelection::SerialNetwork], serialRc.toStdString().c_str(), "wb");
    //     fopen_s(&_chlTelemetry[ChannelSelection::MainNetwork], mainChl.toStdString().c_str(), "wb");
    //     fopen_s(&_chlTelemetry[ChannelSelection::SlaveNetwork], slaveChl.toStdString().c_str(), "wb");
    //     fopen_s(&_chlTelemetry[ChannelSelection::SerialNetwork], serialChl.toStdString().c_str(), "wb");
    // }

    auto get = veh_proto_manager.begin();
    while (get != veh_proto_manager.end()) {
        get->get()->enableFactSave(ChannelSelection::MainNetwork);
        get->get()->enableFactSave(ChannelSelection::SlaveNetwork);
        get->get()->enableFactSave(ChannelSelection::SerialNetwork);
        ++get;
    }

    connect(this, &VehicleSession::checkWarnMessage, this, [&] {
        for (auto mx = _vehicleList.begin(); mx != _vehicleList.end(); mx++) {
            mx->get()->calculateWarnGroup();
        }
    });

    connect(
        this, &VehicleSession::mergeWarnSignal, this,
        [&](int veh) {
            FlightVechicle *vehicle = serachVehicle(veh);
            if (vehicle != nullptr) {
                vehicle->mergeWarnGroup();
            }
        },
        Qt::AutoConnection);

    connect(
        this, &VehicleSession::addWarnLevel, this,
        [&](int veh, int level, int priority, const QString &value) {
            FlightVechicle *vehicle = serachVehicle(veh);
            // txtSpeechQueue.enqueue(value);
            if (vehicle != nullptr) {
                vehicle->addWarnGroup(priority, level, value);
            }
        },
        Qt::QueuedConnection);

    win_shortcut.registerWinHotKey(&vehAdjustControl);

    vehicle_warn_export.startExport();

    startSerialService();

    // sock_loop.Initialize();

    startMainNetService();

    startSlaveNetService();

    extentNetService(&sock_loop);

    txtToSpeech = std::make_shared<QTextToSpeech>();
    txtToSpeech->setVolume(0.9); ///
    thread_status = true;

    net_data_thd = std::thread([&]() {
        // sock_loop.AsyncRun(nullptr);
        while (thread_status) {
            networkSend(&main_read_write, &slave_read_write);
            // QString msg = "";
            // if (txtSpeechQueue.try_dequeue(msg) && !msg.isEmpty()) {
            //     txtToSpeech->say(msg);
            // }
            std::this_thread::sleep_for(std::chrono::milliseconds(35));
        }
        // sock_loop.Destroy();
    });

    net_data_timer = timeSetEvent(
        DATAPROCESS_DELAY_MS, DATAPROCESS_ACCURACY,
        (LPTIMECALLBACK)[](UINT timerID, UINT msg, DWORD_PTR dwUser, DWORD_PTR dwl, DWORD_PTR dw2) {
            VehicleSession *client = (VehicleSession *) (dwUser);
            if ((client != nullptr) && (client->thread_status)) {
                client->dataProcess(&client->main_read_write, &client->slave_read_write);
            }
        },
        ULONG_PTR(this), TIME_PERIODIC);

    savefile_status = true;
}

void VehicleSession::stopService() {

    COMMON_INFO("stop service....");

    win_shortcut.removeHotKey();
    vehicle_warn_export.closeExport();

    if (savefile_status.load()) {
        savefile_status = false;
        // for (int mx = 0; mx < 5; mx++) {
        //     if (_chlControl[mx] != nullptr) {
        //         fwrite(cacheControl[mx].data, 1, cacheControl[mx].length, _chlControl[mx]);
        //         fclose(_chlControl[mx]);
        //         _chlControl[mx] = nullptr;
        //     }
        //     if (_chlTelemetry[mx] != nullptr) {
        //         fwrite(cacheTelemetry[mx].data, 1, cacheTelemetry[mx].length, _chlTelemetry[mx]);
        //         fclose(_chlTelemetry[mx]);
        //         _chlTelemetry[mx] = nullptr;
        //     }
        // }
        auto get = veh_proto_manager.begin();
        while (get != veh_proto_manager.end()) {
            get->get()->closeFactSave();
            ++get;
        }
    }

    stopExtentNetService();

    if (thread_status) {

        slave_read_write.Destory();
        main_read_write.Destory();

        thread_status = false;
        if (net_data_thd.joinable()) {
            net_data_thd.join();
        }
    }

    bool serail_expect = false;
    if (serail_status.compare_exchange_strong(serail_expect, true) && serail_thd.joinable()) {
        serail_status = true;
        serail_thd.join();
    }

    bool process_expect = false;
    if (softcomm_status.compare_exchange_strong(process_expect, true) && softcomm_thd.joinable()) {
        softcomm_status = true;
        softcomm_thd.join();
    }

    if (net_data_timer != 0) {
        timeKillEvent(net_data_timer);
        net_data_timer = 0;
    }

    COMMON_INFO("after stop service....");
}

void VehicleSession::switchVehicel(int idx) {

    if (idx < 0 || idx >= _vehicleList.size()) {
        COMMON_INFO("invalid vehicle index {}", idx);
        return;
    }

    _currentVehicle = _vehicleList.at(idx).get();
    auto get = veh_proto_manager.find(_vehicleIndex);
    if (get != veh_proto_manager.end()) {
        protoDataSet = get->get();
    }

    emit currVehicleChanged();
}

void VehicleSession::loadVehicleEnv(int count, const QString &routefile) {

    for (int i = 0; i < count && configCtrl; i++) {
        //创建一个名为cur的智能指针变量，指向一个动态分配的FlightVechicle对象
        std::shared_ptr<FlightVechicle> cur(new FlightVechicle());
        //设置飞机ID为i
        cur->setVehicleIdx(i);
        //创建一个名为decoder的智能指针变量，指向一个动态分配的FactDecoderMgr对象
        FactDecoderMgr *decoder = createProtoDecoder(i);
        assert(decoder != nullptr);
        cur->loadExcelWarn(metaWarnFile, [&](int frame) { return decoder->protoframe(frame); });
        //nullptr是空指针常量
        if (cur != nullptr && decoder != nullptr) {
            decoder->initSummary([=](int veh, int datas, int frame) {
                if (cur != nullptr) {
                    cur->summaryDataLink(datas, frame);
                } else {
                    COMMON_WARN("unable found vehicle");
                }
            });
            cur->feedback()->initColumnHeader();
            cur->reloadDisplay(decoder, configCtrl->displayConfig());
            if (_currentVehicle == nullptr) {
                _currentVehicle = cur.get();
            }
            _vehicleList.append(cur);
            _aircrafts.append(cur.get());
        } else {
            COMMON_WARN("allocation failed");
        }
    }
}

FlightVechicle *VehicleSession::serachVehicle(int id) {
    if (id < 0 || id >= _vehicleList.count()) {
        COMMON_WARN("invalid vehicle id {} ", id);
        return nullptr;
    }

    return _vehicleList.at(id).get();
}

VehicleFeedback *VehicleSession::feedbackByVehicle(int id) {
    if (id < 0 || id >= _vehicleList.count()) {
        COMMON_WARN("invalid vehicle id {} ", id);
        return nullptr;
    }

    FlightVechicle *veh = _vehicleList.at(id).get();
    if (veh != nullptr) {
        return veh->feedback();
    }

    return nullptr;
}

void VehicleSession::unittestRandSimple(int value, int jump, int type) {
    auto get = veh_proto_manager.begin();
#if defined(AFC21_PROJECT_CONF)
    while (get != veh_proto_manager.end()) {
        get->get()->unitTestDecoder(value, jump, type);
        ++get;
    }
#else
    while (get != veh_proto_manager.end()) {
        NetworkNodeConfig *netmain = configCtrl->mainNetNode();
        ChannelConfig &configmain = netmain->config();
        get->get()->unitTestDecoder(value, jump, type, configmain);
        NetworkNodeConfig *netslave = configCtrl->slaveNetNode();
        ChannelConfig &configslave = netslave->config();
        get->get()->unitTestDecoder(value, jump, type, configslave);
        ++get;
    }
#endif
    vehDataUpdate = true;
}

void VehicleSession::unittestSimpleThd() {
    auto get = veh_proto_manager.begin();
    if (get != veh_proto_manager.end()) {
        NetworkNodeConfig *netmain = configCtrl->mainNetNode();
        ChannelConfig &configmain = netmain->config();
        get->get()->unitTestNetwork(0, configmain);

        NetworkNodeConfig *netslave = configCtrl->slaveNetNode();
        ChannelConfig &configslave = netslave->config();
        get->get()->unitTestNetwork(1, configslave);
    }
}

FlightVechicle *VehicleSession::currVehicle() { return _currentVehicle; }

FactDecoderMgr *VehicleSession::protoManager(uint8_t vehId) {
    auto get = veh_proto_manager.find(vehId);
    if (get != veh_proto_manager.end()) {
        return get->get();
    }

    COMMON_WARN("unknown vehicle id {}, not found FactDecoderMgr object", vehId);
    return nullptr;
}

void VehicleSession::updateDrawing() { emit signalDrawing(); }

void VehicleSession::loadRCProto(const QStringList &adjust, const QString &control) {
    vehAdjustControl.reloadAdjusts(adjust);
    vehAdjustControl.reloadCommands(control);
}

void VehicleSession::loadExcelWarn(const QString &file) { metaWarnFile = file; }

void VehicleSession::loadExcelProto(const QString &file) { metaDataFile = file; }

void VehicleSession::exportRCData() { vehAdjustControl.exportRCData(); }

void VehicleSession::exportRMData() { vehAdjustControl.exportRMData(); }
