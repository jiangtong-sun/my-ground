#include "VehicleMission.h"
#include "ConfigController.h"
#include "FlightAircraft.h"
#include "GcsLineStage.h"
#include "log_category.h"
#include <QUdpSocket>

#define TimeoutForNextStep 500

class SoftwareCRC32 {
  public:
    uint32_t calcCRC32Value(int32_t f_data_r) const {
        uint32_t ulCRC = f_data_r & 0xFF;
        for (int i = 0; i < 8; ++i) {
            if (ulCRC & 1) {
                ulCRC = (ulCRC >> 1) ^ 0xEDB88320L;
            } else {
                ulCRC >>= 1;
            }
        }
        return ulCRC;
    }

    uint32_t compute(const uint8_t *data, size_t len) const {
        uint32_t crc = crc32_start();
        for (size_t i = 0; i < len; ++i) {
            uint8_t byte = data[i];
            uint32_t ulCrcDark = (crc >> 8) & 0x00FFFFFFL;
            uint32_t ulCrcWhite = calcCRC32Value(static_cast<int32_t>((crc ^ byte) & 0xFF));
            crc = ulCrcDark ^ ulCrcWhite;
        }
        crc = crc32_end(crc);
        return crc;
    }

    inline uint32_t crc32_start(void) const { return 0xFFFFFFFFU; }
    inline uint32_t crc32_end(uint32_t crc) const { return crc ^ 0xFFFFFFFFU; }
};

static SoftwareCRC32 softCrc32;
// 0x8005 ==== x^16 + x^15 + x^2 + 1
static unsigned short const crc16_table[] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1,
    0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40,
    0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1,
    0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1,
    0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40,
    0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1,
    0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0,
    0x2080, 0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740,
    0xA501, 0x65C0, 0x6480, 0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0,
    0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1,
    0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181, 0x5140,
    0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440, 0x9C01, 0x5CC0,
    0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0,
    0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341,
    0x4100, 0x81C1, 0x8081, 0x4040};

// 0x1021 ==== x16+x12+x5+1
static const unsigned short crctable[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad,
    0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a,
    0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b,
    0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861,
    0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
    0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87,
    0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,
    0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3,
    0x5004, 0x4025, 0x7046, 0x6067, 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290,
    0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e,
    0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f,
    0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c,
    0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83,
    0x1ce0, 0x0cc1, 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
    0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

int VehicleMission::vehicleIdx() {
    assert(curr_vehicle != nullptr);
    if (curr_vehicle != nullptr) {
        return curr_vehicle->vehicleIdx();
    }
    return 0;
}

QString VehicleMission::vehName() {
    assert(curr_vehicle != nullptr);
    if (curr_vehicle != nullptr) {
        return curr_vehicle->name();
    }
    return QString();
}

void VehicleMission::setShowDown(bool val) {
    show_downline = val;
    emit showDownChanged();
    emit currFLineChanged();
}

void VehicleMission::setSessionVehicle(VehicleSession *session, FlightVechicle *vehicle) {
    veh_session = session;
    curr_vehicle = vehicle;
    assert(curr_vehicle != nullptr);
    if (curr_vehicle != nullptr) { //
        connect(curr_vehicle, &FlightVechicle::positionChanged, this, [=](QGeoCoordinate pos) {

        });
    }
}

void VehicleMission::switchWaypoint(StageController *fline, int waypointIdx) {
    assert(fline != nullptr);
    assert(veh_session != nullptr);
    if ((fline == nullptr) || (veh_session == nullptr) || (veh_session->vehicleRC() == nullptr)) {
        COMMON_WARN("invalid session param");
        return;
    }

    int code = up_down_fightline.getTaskLineCode(fline);
    if (code == GcsTaskLine::FTaskInvalid) {
        COMMON_WARN("invalid stage object");
        return;
    }

    AdjustControl *commands = veh_session->vehicleRC()->commands();
    if (commands == nullptr) {
        COMMON_WARN("unable get command object");
        return;
    }

    ///任务航线
    RCAdjustItem *item = commands->getAdjustByfeedback(0x38);
    if (item == nullptr) {
        COMMON_WARN("unable get mission object");
        return;
    }

    FactListModel *metaModel = item->moreMetaModel();
    if ((metaModel != nullptr) && (metaModel->count() > 1)) {
        FactMetaData *linecode = metaModel->value<FactMetaData *>(0);
        FactMetaData *seq = metaModel->value<FactMetaData *>(1);
        if (linecode != nullptr) {

            uint8_t acode = 0;
            if (GcsTaskLine::FTaskLineA == code || GcsTaskLine::FTaskLineB == code) {
                acode = ((code & 0xf) << 5) | fline->uuid();
                linecode->setRawValue(QString::number(acode));
            } else {
                acode = ((code & 0xf) << 5) | fline->uuid();
                linecode->setRawValue(QString::number(acode));
            }

            COMMON_WARN("switch line code: {}, acode: {}, uuid: {}, point: {}", code, acode, fline->uuid(),
                        waypointIdx + 1);
        }

        if (seq != nullptr) {
            seq->setRawValue(QString::number(waypointIdx + 1));
        }

        veh_session->sendMissionCommand(item);
        return;
    }
}

int VehicleMission::loadFlightLineFromFile(QString filename) {
    if (filename.isEmpty()) {
        return GcsLineOperate::InvalidParameter;
    }

    int rtn = up_down_fightline.loadFlightLine(filename, false);
    up_down_fightline.getLandingSetpoints(GcsTaskLine::FTaskLineA);
    return rtn;
}

int VehicleMission::switchFlightline(StageController *fline) {
    assert(veh_session != nullptr);
    if (veh_session == nullptr || fline == nullptr) {
        COMMON_WARN("invalid session param");
        return false;
    }

    assert(fline != nullptr);
    if (veh_session && veh_session->vehicleRC()) {

    } else {
        COMMON_WARN("unknown remote control object");
    }

    return 0;
}

void VehicleMission::mergeFlightLine(FLineController *task) {
    assert(task != nullptr);
    QByteArray jsonStr = task->fline2Json();
    up_down_fightline.removeStageResource();
    up_down_fightline.name(task->name());
    up_down_fightline.number(task->number());
    up_down_fightline.update(task->update());
    up_down_fightline.uuid(task->uuid());
    up_down_fightline.mergeItem(jsonStr);

    up_down_fightline.getLandingSetpoints(GcsTaskLine::FTaskLineA);

    emit currFLineChanged();
}

int VehicleMission::exportFlightLine(QString file) {
    if (file.isEmpty()) {
        return GcsLineOperate::InvalidParameter;
    }

    return up_down_fightline.exportFlightLine(file);
}

VehicleMission::VehicleMission() : _checked(false), curr_vehicle(nullptr) {
    connect(this, &VehicleMission::downloadDataFinish, this, &VehicleMission::processMessionDownload);
}

VehicleMission::~VehicleMission() {}

uint16_t VehicleMission::missionCRC16(const uint8_t *head, uint16_t len) {
    unsigned short crc = 0xffff;
    for (int i = 0; i < len; i++) {
        // crc = (crc << 8) ^ crctable[(crc >> 8) ^ head[i]];
        crc = (crc >> 8) ^ crc16_table[(crc ^ head[i]) & 0xff];
    }
    crc ^= 0x0000;
    return crc;
}

uint16_t VehicleMission::missionCRC32(const uint8_t *head, uint16_t len) {
    uint32_t crc32 = softCrc32.compute(head, len);
    return (crc32 >> 16) & 0xffff | crc32 & 0xffff;
}

int64_t VehicleMission::currentTimeMs() {
    typedef std::chrono::milliseconds ms_precision;
    typedef std::chrono::time_point<std::chrono::system_clock, ms_precision> time_precision;
    time_precision tp = std::chrono::time_point_cast<ms_precision>(std::chrono::system_clock::now());
    return tp.time_since_epoch().count();
}

FLineController *VehicleMission::currFline() { return &up_down_fightline; }

void VehicleMission::releaseResource() {
    up_down_fightline.calcWaypointPath();
    up_down_fightline.clearStageResource();
}

void VehicleMission::processMessionDownload(int stage) {
    if (stage > 0) {
        templineMission();
        return;
    }

    offlineMission();
}

void VehicleMission::templineMission() {
    std::shared_ptr<FlineStageDetail> stageDetail = nullptr;
    stageDetail = up_down_fightline.getStageDetial(GcsTaskLine::FTaskTempline);
    assert(stageDetail != nullptr);
    if (stageDetail == nullptr) {
        COMMON_INFO("allocate space");
        return;
    }

    int count = up_down_cache.countDownCache();
    for (int im = 0; (im < count); im++) {
        std::shared_ptr<MissionLineDataProto> cache = nullptr;
        cache = up_down_cache.downCache(im);
        assert(cache != nullptr);

        StageController *curstage = stageDetail->isExist(cache->number, cache->stage);
        assert(curstage != nullptr);

        for (int iy = 0; curstage && iy < cache->pointCount; iy++) {
            curstage->addOrUpdate(cache->point[iy], (cache->groupNo - 1) * 4 + iy);
        }
    }

    if (stageDetail) {

        stageDetail->recalWaypointPath();
    }
}

void VehicleMission::offlineMission() {

    std::shared_ptr<FlineStageDetail> stageDetail = nullptr;
    int count = up_down_cache.countDownCache();
    int lastStage = -1;
    int lineNumber = -1;

    for (int im = 0; (im < count); im++) {
        std::shared_ptr<MissionLineDataProto> cache = nullptr;
        cache = up_down_cache.downCache(im);
        assert(cache != nullptr);
        if ((lastStage != (32 + cache->number)) && (cache->stage == 0) && (cache->number == 1)) {
            stageDetail = up_down_fightline.getStageDetial(GcsTaskLine::FTaskLineA);
            stageDetail->removeAllStages();
            assert(stageDetail != nullptr);
            if (stageDetail == nullptr) {
                COMMON_INFO("allocate space");
                return;
            }
            lastStage = (32 + cache->number);
        } else if ((lastStage != (32 + cache->number)) && (cache->stage == 0) && (cache->number == 2)) {
            stageDetail = up_down_fightline.getStageDetial(GcsTaskLine::FTaskLineB);
            stageDetail->removeAllStages();
            assert(stageDetail != nullptr);
            if (stageDetail == nullptr) {
                COMMON_INFO("allocate space");
                return;
            }
            lastStage = (32 + cache->number);
        } else if ((lastStage != cache->stage) && (cache->stage == 1)) {
            stageDetail = up_down_fightline.getStageDetial(GcsTaskLine::FTaskDepart);
            stageDetail->removeAllStages();
            assert(stageDetail != nullptr);
            if (stageDetail == nullptr) {
                COMMON_INFO("allocate space");
                return;
            }
            lastStage = cache->stage;
        } else if ((lastStage != cache->stage) && (cache->stage == 2)) {
            stageDetail = up_down_fightline.getStageDetial(GcsTaskLine::FTaskErgency);
            stageDetail->removeAllStages();
            assert(stageDetail != nullptr);
            if (stageDetail == nullptr) {
                COMMON_INFO("allocate space");
                return;
            }
            lastStage = cache->stage;
        }

        if (stageDetail == nullptr) {
            COMMON_ERROR("stage {}, number {}, group {}, points {} ", (int) cache->stage, (int) cache->number,
                         cache->groupNo, cache->pointCount);
            continue;
        }

        StageController *curstage = stageDetail->isExist(cache->number, cache->stage);
        if (curstage == nullptr) {
            COMMON_ERROR("stage {}, number {}, group {}, points {} ", (int) cache->stage, (int) cache->number,
                         cache->groupNo, cache->pointCount);
            continue;
        }

        curstage->canRMWaypoint(false);
        COMMON_INFO("{}, stage {}, number {}, group {}, points {} ", (int) curstage, lastStage, (int) cache->number,
                    cache->groupNo, cache->pointCount);

        for (int iy = 0; curstage && (iy < cache->pointCount); iy++) {
            curstage->addOrUpdate(cache->point[iy], (cache->groupNo - 1) * 4 + iy);
        }
    }

    if (stageDetail != nullptr) {
        stageDetail->recalWaypointPath();
        up_down_fightline.getLandingSetpoints(GcsTaskLine::FTaskLineA);
    }
}

void VehicleMission::nextDownloadDataVerify(VehicleMissionProto *protomsg) {
    assert(protomsg != nullptr);

    COMMON_INFO("updown flag: {}, cmd type: {}, downline code: {}", upload_download.load(), protomsg->cmd_type,
                up_down_proto->downtype);

    if (upload_download != 0) { //
        upload_download = 0;

        emit downloadDataFinish(up_down_proto->downtype);

        mapNoticeMessage(GcsLineOperate::MissionDownloadSuccess);
    }
}

void VehicleMission::nextDownloadData(VehicleMissionProto *protomsg) {
    assert(protomsg != nullptr);
    if (up_down_proto == nullptr) {
        return;
    }

    if (protomsg->ident == MissionLineCRCX) {
        nextDownloadDataVerify(protomsg);

    } else if (protomsg->ident == MissionLineTrans) {

        std::shared_ptr<MissionLineDataProto> dataProto = nullptr;
        dataProto = std::make_shared<MissionLineDataProto>();
        memcpy(dataProto.get(), protomsg, sizeof(MissionLineDataProto));
        up_down_cache.setDownloadData(dataProto);

        up_down_proto->resendCount = 0;
        up_down_proto->timeout = currentTimeMs() - 1;
        up_down_proto->taskProto = std::make_shared<MissionLineRespProto>();
        up_down_proto->nextoperate = [=](VehicleMissionProto *msg) { nextDownloadData(msg); };
        MissionLineRespProto *dataproto = (MissionLineRespProto *) (up_down_proto->taskProto.get());
        dataproto->length = sizeof(MissionLineRespProto);
        dataproto->ident = MissionLineResp;
        dataproto->stage = dataProto->stage;
        dataproto->number = dataProto->number;
        dataproto->groupNo = dataProto->groupNo;

        COMMON_INFO("mission number: {}, state: {}, group: {}", (int) dataProto->number, (int) dataProto->stage,
                    dataProto->groupNo);
    }
}

void VehicleMission::nextUploadDataVerify(VehicleMissionProto *protomsg) {
    assert(protomsg != nullptr);
    MissionLineCRCXProto *dataproto = (MissionLineCRCXProto *) (protomsg);
    COMMON_INFO("data crc result: {}", dataproto->verify[0]);
    mapNoticeMessage(GcsLineOperate::MissionUploadSuccess);
    upload_download = 0; //
}

void VehicleMission::missionOperateTimeout(int cmdtype, int timeout) {
    COMMON_INFO("opreate timeout ", timeout);
    mapNoticeMessage(GcsLineOperate::MOperateTimeout);
}

void VehicleMission::nextUploadData(VehicleMissionProto *protomsg) {
    assert(protomsg != nullptr);
    if (up_down_proto == nullptr) {
        return;
    }

    //
    up_down_proto->resendCount = 0;
    up_down_proto->timeout = currentTimeMs() - 1;
    up_down_proto->taskProto = up_down_cache.nextUploadData();

    if (up_down_proto->taskProto == nullptr) {
        up_down_proto->expectKey = MissionLineCRCX;
        up_down_proto->taskProto = std::shared_ptr<MissionLineCRCXProto>(new MissionLineCRCXProto());
        up_down_proto->nextoperate = [=](VehicleMissionProto *msg) { nextUploadDataVerify(msg); };
        MissionLineCRCXProto *dataproto = (MissionLineCRCXProto *) (up_down_proto->taskProto.get());
        dataproto->length = sizeof(MissionLineCRCXProto);
        dataproto->ident = MissionLineCRCX;
        upload_download = UploadCRC;

        COMMON_INFO("next data crc ");

    } else {
        up_down_proto->nextoperate = [=](VehicleMissionProto *msg) { nextUploadData(msg); };
        MissionLineDataProto *dataproto = (MissionLineDataProto *) (up_down_proto->taskProto.get());
        dataproto->length = sizeof(MissionLineDataProto);
        dataproto->ident = MissionLineTrans;
        upload_download = UploadData;

        COMMON_INFO("next stage {}, line {}, group {}, points {}", (int) dataproto->stage, (int) dataproto->number,
                    (int) dataproto->groupNo, (int) dataproto->pointCount);
    }
}

void VehicleMission::missionFeedbackVerify(int feed) {
    //
    // COMMON_INFO("mission verify {}", feed);
}

void VehicleMission::mapNoticeMessage(int feed) {
    COMMON_INFO("mission notice message {}", feed);
    switch (feed) {
    case GcsLineOperate::TaskOperationSuccess:
        emit notifyOperateResult("TaskOperationSuccess");
        break;
    case GcsLineOperate::MissionUploadSuccess:
        emit notifyOperateResult("MissionUploadSuccess");
        break;
    case GcsLineOperate::MissionDownloadSuccess:
        emit notifyOperateResult("MissionDownloadSuccess");
        break;
    case GcsLineOperate::UploadActionStart:
        emit notifyOperateResult("UploadActionStart");
        break;
    case GcsLineOperate::DownloadActionStart:
        emit notifyOperateResult("DownloadActionStart");
        break;
    case GcsLineOperate::InvalidParameter:
        emit notifyOperateResult("InvalidParameter");
        break;
    case GcsLineOperate::LineStageIsFull:
        emit notifyOperateResult("LineStageIsFull");
        break;
    case GcsLineOperate::UnableUptWaypoint:
        emit notifyOperateResult("UnableUptWaypoint");
        break;
    case GcsLineOperate::InvalidWaypoints:
        emit notifyOperateResult("InvalidWaypoints");
        break;
    case GcsLineOperate::LineStageIsLock:
        emit notifyOperateResult("LineStageIsLock");
        break;
    case GcsLineOperate::NoWaypointSpace:
        emit notifyOperateResult("NoWaypointSpace");
        break;
    case GcsLineOperate::UnableUpdateStage:
        emit notifyOperateResult("UnableUpdateStage");
        break;
    case GcsLineOperate::ResendOverCount:
        emit notifyOperateResult("ResendOverCount");
        break;
    case GcsLineOperate::MOperateTimeout:
        emit notifyOperateResult("MOperateTimeout");
        break;
    default:
        break;
    }
}

void VehicleMission::mockDownloadResponse(VehicleMissionProto *protomsg) {
    NetworkNodeConfig *netmain = unit_config.mainNetNode();
    ChannelConfig &configmain = netmain->config();
    QHostAddress group(QString::fromStdString(configmain.multiRxAddr));
    int hport = (configmain.multiRxPort); // ntohs
    int dataLen = sizeof(MissionLineDataProto);

    if (protomsg->ident == MissionDownloadReq) {
        start_down_req = 0;
        std::shared_ptr<VehicleMissionProto> data = nullptr;
        data = up_down_cache.getUploadData(start_down_req);
        assert(data != nullptr);

        MissionLineDataProto *resproto = nullptr;
        resproto = (MissionLineDataProto *) data.get();
        resproto->crc = 0; // reset crc
        FillProtoTime(resproto);
        ProtoCRC8((uint8_t *) resproto, dataLen, true);
        unit_udpsocket.writeDatagram((char *) resproto, dataLen, group, hport);
        QString hex = ToHexStr((uint8_t *) resproto, dataLen);
        COMMON_INFO("mock data {}", hex.toStdString());

    } else if (protomsg->ident == MissionLineResp) {

        std::shared_ptr<VehicleMissionProto> data = nullptr;
        data = up_down_cache.getUploadData(++start_down_req);
        if (data != nullptr) {
            MissionLineDataProto *resproto = nullptr;
            resproto = (MissionLineDataProto *) data.get();
            resproto->crc = 0; // reset crc
            FillProtoTime(resproto);
            ProtoCRC8((uint8_t *) resproto, sizeof(MissionLineDataProto), true);
            unit_udpsocket.writeDatagram((char *) resproto, dataLen, group, hport);
            QString hex = ToHexStr((uint8_t *) resproto, dataLen);
            COMMON_INFO("moc data {}", hex.toStdString());

        } else {
            MissionLineCRCXProto crcproto;
            memset(&crcproto, 0, sizeof(crcproto));
            memcpy(&crcproto, protomsg, sizeof(VehicleMissionProto));
            memset(crcproto.verify, protomsg->number, sizeof(crcproto.verify));
            crcproto.length = sizeof(MissionLineCRCXProto);
            crcproto.ident = MissionLineCRCX;
            crcproto.crc = 0;
            FillProtoTime(&crcproto);
            ProtoCRC8((uint8_t *) &crcproto, sizeof(crcproto), true);
            unit_udpsocket.writeDatagram((char *) &crcproto, sizeof(crcproto), group, hport);
            QString hex = ToHexStr((uint8_t *) &crcproto, sizeof(crcproto));
            COMMON_INFO("mock crc {}", hex.toStdString());
        }
    } else {
        COMMON_WARN("unknow message id {}", protomsg->ident);
    }
}

void VehicleMission::mockUploadResponse(VehicleMissionProto *protomsg) {
    NetworkNodeConfig *netmain = unit_config.mainNetNode();
    ChannelConfig &configmain = netmain->config();
    QHostAddress group(QString::fromStdString(configmain.multiRxAddr));
    int hport = (configmain.multiRxPort); // ntohs

    if (protomsg->ident == MissionUploadReq || protomsg->ident == TemplineUploadReq) {
        MissionLineRespProto resproto;
        memset(&resproto, 0, sizeof(resproto));
        memcpy(&resproto, protomsg, sizeof(VehicleMissionProto));
        resproto.ident = MissionLineResp;
        resproto.length = sizeof(MissionLineRespProto);
        resproto.crc = 0;
        FillProtoTime(&resproto);
        ProtoCRC8((uint8_t *) &resproto, sizeof(resproto), true);
        unit_udpsocket.writeDatagram((char *) &resproto, sizeof(resproto), group, hport);
        QString hex = ToHexStr((uint8_t *) &resproto, sizeof(resproto));
        COMMON_INFO("mock req -> resp {}", hex.toStdString());

    } else if (protomsg->ident == MissionLineTrans) {
        MissionLineRespProto resproto;
        memset(&resproto, 0, sizeof(resproto));
        memcpy(&resproto, protomsg, sizeof(VehicleMissionProto));
        resproto.length = sizeof(MissionLineRespProto);
        resproto.ident = MissionLineResp;
        resproto.crc = 0;
        FillProtoTime(&resproto);
        ProtoCRC8((uint8_t *) &resproto, sizeof(resproto), true);
        unit_udpsocket.writeDatagram((char *) &resproto, sizeof(resproto), group, hport);
        QString hex = ToHexStr((uint8_t *) &resproto, sizeof(resproto));
        COMMON_INFO("mock trans -> resp {}", hex.toStdString());

    } else if (protomsg->ident == MissionLineCRCX) {
        MissionLineCRCXProto resproto;
        memset(&resproto, 0, sizeof(resproto));
        memcpy(&resproto, protomsg, sizeof(VehicleMissionProto));
        memset(resproto.verify, protomsg->number, sizeof(resproto.verify));
        resproto.length = sizeof(MissionLineCRCXProto);
        resproto.ident = MissionLineCRCX;
        resproto.crc = 0;
        FillProtoTime(&resproto);
        ProtoCRC8((uint8_t *) &resproto, sizeof(resproto), true);
        unit_udpsocket.writeDatagram((char *) &resproto, sizeof(resproto), group, hport);
        upload_download = UploadEnd;

        QString hex = ToHexStr((uint8_t *) &resproto, sizeof(resproto));
        COMMON_INFO("mock crc -> crc verify {}", hex.toStdString());

    } else {
        COMMON_WARN("unknow message id {}", protomsg->ident);
    }
}

void VehicleMission::processMissionUpload(VehicleMissionProto *protomsg) {

    if (unit_test_mock) {
        // 模拟初始状态与数据传输过程
        if ((protomsg->ident == MissionUploadReq || protomsg->ident == TemplineUploadReq ||
             protomsg->ident == MissionLineTrans)) {
            mockUploadResponse(protomsg);
            return;
        }
        // 模拟数据校验
        if (upload_download == UploadCRC && protomsg->ident == MissionLineCRCX) {
            mockUploadResponse(protomsg);
            return;
        }
    }

    if ((upload_download == UploadCRC || upload_download == UploadEnd) && protomsg->ident == MissionLineResp) {
        MissionLineRespProto *respProto = (MissionLineRespProto *) protomsg;
        COMMON_INFO("upload errcode {}, expect {}, active {}", respProto->errCode, up_down_proto->expectKey,
                    protomsg->ident);
        mapNoticeMessage(respProto->errCode);
        up_down_proto = nullptr;
        upload_download = 0;
        return;
    }

    if ((up_down_proto != nullptr) && (protomsg != nullptr) && (up_down_proto->expectKey != protomsg->ident)) {
        COMMON_INFO("mission expect {}, active {}", up_down_proto->expectKey, protomsg->ident);
        return;
    }

    if (up_down_proto == nullptr) {
        mapNoticeMessage(GcsLineOperate::MOperateTimeout);
        return;
    }

    VehicleMissionProto *req = (VehicleMissionProto *) up_down_proto->taskProto.get();
    if ((req != nullptr) && (protomsg != nullptr) && (protomsg->ident == MissionLineResp) &&
        (req->ident == MissionLineTrans)) {
        MissionLineRespProto *resProto = (MissionLineRespProto *) protomsg;
        MissionLineDataProto *reqProto = (MissionLineDataProto *) up_down_proto->taskProto.get();
        if (resProto == nullptr || reqProto == nullptr) {
            COMMON_ERROR("unable get request & response object");
            mapNoticeMessage(GcsLineOperate::MOperateTimeout);
            return;
        }

        if ((resProto->number != reqProto->number) || (resProto->stage != reqProto->stage) ||
            (resProto->groupNo != reqProto->groupNo)) {
            COMMON_ERROR("request linedata {} {} {},  response {} {} {}", (int) reqProto->stage, (int) reqProto->number,
                         reqProto->groupNo, (int) resProto->stage, (int) resProto->number, resProto->groupNo);
            mapNoticeMessage(GcsLineOperate::ProtoDataTransferErr);
            return;
        }
    }

    if (up_down_proto->resendCount < 3) {
        if (up_down_proto->nextoperate) {
            up_down_proto->nextoperate(protomsg);
        } else {
            COMMON_WARN("undefine next operation");
        }
    } else if (up_down_proto->timeouts) {
        int during = currentTimeMs() - up_down_proto->timeout;
        up_down_proto->timeouts(protomsg->ident, during);
        COMMON_WARN("operation timeout {}", during);
        up_down_proto = nullptr;
    }
}

void VehicleMission::processMissionDownload(VehicleMissionProto *protomsg) {

    /// 自测试条件
    if (unit_test_mock && (protomsg->ident == MissionDownloadReq || protomsg->ident == MissionLineResp)) {
        mockDownloadResponse(protomsg);
        return;
    }

    /// 期待反馈消息类型, 否则重传消息
    if ((protomsg->ident != MissionLineCRCX) && (up_down_proto->expectKey != protomsg->ident)) {
        COMMON_INFO("mission expect {}, active {}", up_down_proto->expectKey, protomsg->ident);
        return;
    }

    if (up_down_proto == nullptr) {
        mapNoticeMessage(GcsLineOperate::MOperateTimeout);
        return;
    }

    if (up_down_proto->resendCount < 3) {
        if (up_down_proto->nextoperate) {
            up_down_proto->nextoperate(protomsg);
        } else {
            COMMON_WARN("undefine next operation");
        }

    } else if (up_down_proto->timeouts) {
        int during = currentTimeMs() - up_down_proto->timeout;
        up_down_proto->timeouts(protomsg->ident, up_down_proto->timeout);
        COMMON_WARN("operation timeout {}", during);
        up_down_proto = nullptr;
    }
}

void VehicleMission::processMissionMsg(VehicleMissionProto *protomsg) {
    assert(protomsg != nullptr);
    if ((upload_download <= 0)) {
        return;
    }

    if (upload_download & DOWNLOAD_OPERATION) {
        processMissionDownload(protomsg);
    }

    if (upload_download & UPLOAD_OPERATION) {
        processMissionUpload(protomsg);
    }
}

void VehicleMission::missionProcess(Channel *read_write, MissionMessageQueue &taskqueue) {
    qint64 cur = currentTimeMs();
    if ((upload_download <= 0) || (up_down_proto == nullptr)) {
        return;
    }

    if (up_down_proto->resendCount > 3) {
        int during = currentTimeMs() - up_down_proto->timeout;
        mapNoticeMessage(GcsLineOperate::ResendOverCount);
        COMMON_WARN("mission req over count {}", during);
        up_down_proto = nullptr;
        upload_download = 0;
        return;
    }

    if (cur >= up_down_proto->timeout && cur >= _bandwidth.next_time) {
        up_down_proto->resendCount += 1;
        up_down_proto->timeout = cur + TimeoutForNextStep;
        ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
        VehicleCommand *vehicleRC = veh_session->vehicleRC();
        VehicleMissionProto *proto = up_down_proto->taskProto.get();
        uint8_t *address = (uint8_t *) proto;
        proto->cmd_type = decoder.MissionOptFrame;
        proto->magic1 = decoder.ProtoSplitWordA;
        proto->magic2 = decoder.ProtoSplitWordB;
        proto->number = vehicleRC->cmdNumber();
        address += proto->length - 1;
        *address = 0;

        FillProtoTime(proto);
        ProtoCRC8((uint8_t *) proto, proto->length, true);
        QString hex = ToHexStr((uint8_t *) proto, proto->length);
        COMMON_INFO("mission: {} ", hex.toStdString());

        if (read_write == nullptr) {
            char *buffer = new char[proto->length + 1];
            memcpy(buffer, proto, proto->length);
            taskqueue.enqueue((VehicleMissionProto *) buffer);
        } else {
            read_write->Send((char *) proto, proto->length, nullptr);
        }

        /// 控制任务数据的上行频率
        _bandwidth.databytes += proto->length;
        _bandwidth.next_time = cur + TimeoutForNextStep / 5;
    }
}

int VehicleMission::downloadFlightLine(int linecode) {
    assert(veh_session != nullptr);
    if (veh_session == nullptr) {
        COMMON_WARN("invalid session param");
        return GcsLineOperate::InvalidParameter;
    }

    if (!(linecode == 0 || linecode == 1 || linecode == 2)) {
        COMMON_WARN("invalid input param");
        return GcsLineOperate::LineStageIsFull;
    }

    up_down_cache.clearMissionCache();

    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    up_down_proto = std::shared_ptr<ExpectFeedback>(new ExpectFeedback());
    up_down_proto->expectKey = MissionLineTrans;
    up_down_proto->expectVal = 0;
    up_down_proto->resendCount = 0;
    up_down_proto->downtype = linecode;
    up_down_proto->timeout = currentTimeMs() - TimeoutForNextStep / 2; //
    up_down_proto->taskProto = std::shared_ptr<VehicleMissionProto>(new MissionDownloadReqProto());
    up_down_proto->nextoperate = [=](VehicleMissionProto *msg) { nextDownloadData(msg); };
    up_down_proto->timeouts = [=](int type, int timeout) { missionOperateTimeout(type, timeout); };
    MissionDownloadReqProto *proto = (MissionDownloadReqProto *) (up_down_proto->taskProto.get());
    memset(proto, 0, sizeof(*proto));
    proto->ident = MissionDownloadReq;
    proto->typeline = linecode; // 0 == offline
    proto->length = sizeof(MissionDownloadReqProto);
    upload_download = DownloadInit;
    mapNoticeMessage(GcsLineOperate::DownloadActionStart);
    return GcsLineOperate::DownloadActionStart;
}

int VehicleMission::uploadFline(const FlineStageDetail *val) {
    // ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    // up_down_proto = std::shared_ptr<ExpectFeedback>(new ExpectFeedback());
    // up_down_proto->expectKey = MissionLineResp;
    // up_down_proto->expectVal = 0;
    // up_down_proto->resendCount = 0;
    // up_down_proto->timeout = currentTimeMs() - TimeoutForNextStep / 2; //
    // up_down_proto->taskProto = std::shared_ptr<VehicleMissionProto>(new MissionUploadReqProto());
    // up_down_proto->nextoperate = [=](VehicleMissionProto *msg) { nextUploadData(msg); };
    // up_down_proto->timeouts = [=](int type, int timeout) { missionOperateTimeout(type, timeout);
    // }; MissionUploadReqProto *proto = (MissionUploadReqProto *) (up_down_proto->taskProto.get());
    // memset(proto, 0, sizeof(*proto));
    // proto->ident = MissionUploadReq;
    // proto->length = sizeof(MissionUploadReqProto);
    // assert(proto != nullptr);
    // if (val->code() == GcsTaskLine::FTaskLineA) {
    //     proto->normal += val->counts() >= FlightStageLimit::instance()->normalALimit() ? 1 : 0;
    // } else if (val->code() == GcsTaskLine::FTaskLineB) {
    //     proto->normal += val->counts() >= FlightStageLimit::instance()->normalBLimit() ? 1 : 0;
    // } else if (val->code() == GcsTaskLine::FTaskErgency) {
    //     proto->normal += val->counts() >= FlightStageLimit::instance()->ftaskErgency() ? 1 : 0;
    // } else if (val->code() == GcsTaskLine::FTaskDepart) {
    //     proto->departarrive += val->counts();
    // } else {
    //     return GcsLineOperate::InvalidParameter;
    // }
    // up_down_fightline.fillUpReqProto(proto);
    // up_down_cache.produceUploadData(&up_down_fightline);
    // upload_download = UploadInit;
    mapNoticeMessage(GcsLineOperate::UploadActionStart);
    return GcsLineOperate::UploadActionStart;
}

void VehicleMission::mockEnable() {
    unit_config.reloadNetComConfig("xfres/networkserail.ini");
    unit_config.loadLocalIP();
    unit_config.checkingLocalIP();

    QVariant tt(1);
    NetworkNodeConfig *netmain = unit_config.mainNetNode();
    ChannelConfig &configmain = netmain->config();
    QHostAddress group(QString::fromStdString(configmain.multiRxAddr));
    QHostAddress address(QString::fromStdString(configmain.address));
    unit_udpsocket.bind(address, configmain.multiRxPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    unit_udpsocket.setSocketOption(QAbstractSocket::MulticastLoopbackOption, tt);
    unit_udpsocket.joinMulticastGroup(group);
    unit_test_mock = true;
}

int VehicleMission::uploadStage(StageController *fline) {
    assert(veh_session != nullptr);
    if (veh_session == nullptr) {
        COMMON_WARN("invalid session param");
        return GcsLineOperate::InvalidParameter;
    }

    if (fline == nullptr) {
        COMMON_WARN("invalid param");
        return GcsLineOperate::InvalidParameter;
    }

    if (fline->stage() != GcsLineStage::Templine) {
        COMMON_WARN("input is not templine");
        return GcsLineOperate::UnsupportNotTempline;
    }

    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    up_down_proto = std::shared_ptr<ExpectFeedback>(new ExpectFeedback());
    up_down_proto->expectKey = MissionLineResp;
    up_down_proto->expectVal = 0;
    up_down_proto->resendCount = 0;
    up_down_proto->timeout = currentTimeMs() - TimeoutForNextStep / 2; //
    up_down_proto->taskProto = std::shared_ptr<VehicleMissionProto>(new TemplineUploadReqProto());
    up_down_proto->nextoperate = [=](VehicleMissionProto *msg) { nextUploadData(msg); };
    up_down_proto->timeouts = [=](int type, int timeout) { missionOperateTimeout(type, timeout); };
    TemplineUploadReqProto *proto = (TemplineUploadReqProto *) (up_down_proto->taskProto.get());
    memset(proto, 0, sizeof(*proto));
    proto->ident = TemplineUploadReq;
    proto->length = sizeof(TemplineUploadReqProto);
    proto->templine = fline->uuid();
    up_down_cache.produceUploadData(fline, GcsTaskLine::FTaskTempline);
    upload_download = UploadInit;
    mapNoticeMessage(GcsLineOperate::UploadActionStart);
    return GcsLineOperate::UploadActionStart;
}

int VehicleMission::uploadFline() {
    if (veh_session == nullptr) {
        COMMON_WARN("invalid session param");
        return GcsLineOperate::InvalidParameter;
    }

    ProtoDecoderInfo &decoder = GetProtoDecoderInfo();
    up_down_proto = std::shared_ptr<ExpectFeedback>(new ExpectFeedback());
    up_down_proto->expectKey = MissionLineResp;
    up_down_proto->expectVal = 0;
    up_down_proto->resendCount = 0;
    up_down_proto->timeout = currentTimeMs() - TimeoutForNextStep / 2; //
    up_down_proto->taskProto = std::shared_ptr<VehicleMissionProto>(new MissionUploadReqProto());
    up_down_proto->nextoperate = [=](VehicleMissionProto *msg) { nextUploadData(msg); };
    up_down_proto->timeouts = [=](int type, int timeout) { missionOperateTimeout(type, timeout); };
    MissionUploadReqProto *proto = (MissionUploadReqProto *) (up_down_proto->taskProto.get());
    memset(proto, 0, sizeof(*proto));
    proto->ident = MissionUploadReq;
    proto->length = sizeof(MissionUploadReqProto);
    proto->cmd_type = decoder.MissionOptFrame;
    up_down_fightline.fillUpReqProto(proto);
    up_down_cache.produceUploadData(&up_down_fightline);
    upload_download = UploadInit;
    mapNoticeMessage(GcsLineOperate::UploadActionStart);
    return GcsLineOperate::UploadActionStart;
}
