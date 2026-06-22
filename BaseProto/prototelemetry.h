#pragma once

#include "uvloop.h"
#include "networkdata.h"
#include "protometadef.h"
#include <QUdpSocket>
#include <QString>
#include <functional>
#include <stdint.h>

struct BASEPROTO_EXPORT Channel{
    ProtoDataCache read_cache;
    ProtoDataCache write_cache;
    sockaddr_in rw_multiRx_addr;
    sockaddr_in rw_multiTx_addr;
    QUdpSocket sock_read;
    QUdpSocket sock_write;
    ChannelConfig configmain;

    bool IsConnected() { return true; }
    int Send(char *data, int size, OnSentCallback send_cb);
    bool Initialize(const ChannelConfig &conf, LoopHandle *loop_handle, bool loop);
    bool Create(LoopHandle *loop_handle);
    bool CreateEx(LoopHandle *loop_handle);
    bool Destory();
};

using OnDecoderPosition = std::function<void(int, double lat, double lng, double alt)>;
using OnDecoderFeedback = std::function<void(int, int feed)>;
using OnDecoderHeading = std::function<void(int, double heading)>;
using CallNumberCallback = std::function<int()>;

struct BASEPROTO_EXPORT ProtoDecoderInfo {
    uint16_t adjustCommand = 0xCC;         //遥调
    uint16_t controlCommand = 0xBB;        //遥控
    uint16_t keepaliveCommand = 0x41ff;    //心跳
    uint16_t RTKTransferFrame = 0x5452;    // RTK
    uint16_t MissionOptFrame = 0x5057;     //任务
    int32_t AdjustCommandType = 0x4141;    //分割符号
    int32_t ControlCommandType = 0x4141;   //分割符号
    int32_t AdjustControlType = 0x4141;    //
    int32_t ProtoSplitWordA = 0xEB;        //帧头
    int32_t ProtoSplitWordB = 0x90;        //帧头
    int32_t ProtoPackageLength = 64;       //协议消息长度
    int32_t ProtoSinglePackage = 16;       //每个消息帧数据长度
    int32_t VehiclePositionFrame = 0x2;    //
    int32_t VehicleFeedbackFrame = 0x4300; //
    int32_t FirstProtoTypeOffset = 0x8;    //
    int32_t OtherProtoTypeOffset = 0x0;    //
    int32_t VehicleLabelAddr = 0x6;        //
    int32_t ProtoTimestampOffset = 0x07;   //
    bool DataBigLittleEndian = 0x0;        //
    QString ProtoTimestampName;            //
};


struct BASEPROTO_EXPORT FeedbackPositionInfo {
    int ControlFeedbackPos = 26;
    int AdjustFeedbackPos = 27;
    int VerifyFeedbackSeq = 0;
    int ProtoPositionLng = 3;
    int ProtoPositionLat = 4;
    int ProtoPositionAlt = 5;
    int ProtoPositionHead = 12;
};

struct BASEPROTO_EXPORT AdjustCmdConfig {
    float resolution;
    int range_low;
    int range_high;
    QString units;
    QString name;
};

BASEPROTO_EXPORT ProtoDecoderInfo &GetProtoDecoderInfo();
BASEPROTO_EXPORT FeedbackPositionInfo &GetFeedbackPositionInfo();
BASEPROTO_EXPORT uint8_t ProtoCRC8Ext(const uint8_t *addr, uint8_t len, bool set);
BASEPROTO_EXPORT uint8_t ProtoCRC8(const uint8_t *addr, uint8_t len, bool set);
BASEPROTO_EXPORT QString ToHexStr(const uint8_t *addr, uint8_t len);

#pragma pack(push, 1)

struct BASEPROTO_EXPORT ControlFrameBase {
    uint8_t magic1;
    uint8_t magic2;
    uint8_t length;
    uint8_t number; // number or gcs address
};

#define GCS_COMM_MAGIC_WORDA 0XEB
#define GCS_COMM_MAGIC_WORDB 0X90
#define SWITCH_STATION_ROLE 0X01

/// ident === 0x81 === 切换角色
/// ident === 0x82 === 数据回放中
struct BASEPROTO_EXPORT SoftwareCommProto : public ControlFrameBase {
    uint8_t devid;   // device id
    uint8_t ident;   // type of command
    uint16_t detail; // detail info
    uint16_t special; //
    uint8_t feedback; //
    uint8_t crc;
};

enum MissionProtoType {
    MissionLineTrans = 0xc1,  // 航线数据传输
    MissionLineResp = 0xc2,   // 航线数据反馈
    MissionLineCRCX = 0xc3,   // 航线数据校验
    MissionUploadReq = 0xc4,  // 航线数据上传请求
    TemplineUploadReq = 0xc5, // 临时航线上传请求
    MissionDownloadReq = 0xc6 // 临时航线数据下载请求
};

struct BASEPROTO_EXPORT VehicleMissionProto : public ControlFrameBase {
    uint16_t dst_addr;
    uint16_t src_addr;
    uint16_t cmd_type;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t ident; // MissionProtoType
};

struct BASEPROTO_EXPORT ControlAdjustProto : public ControlFrameBase {
    uint16_t dst_addr;
    uint16_t src_addr;
    uint16_t cmd_type;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t ctrl_type;    // ctrl=0xBB
    uint8_t ctrl_code[5]; //
    uint8_t adjust_type;  // adjust=0xCC
    uint8_t adjust_code;  //
    uint8_t adjust_detail[14];
    uint8_t ext_space[8]; //
    uint8_t feedback;
    uint8_t crc;
};
BASEPROTO_EXPORT uint16_t AircraftAddress(ControlAdjustProto *proto);
struct BASEPROTO_EXPORT RockerDeviceProto : public ControlFrameBase {
    uint16_t dst_addr;
    uint16_t src_addr;
    uint16_t cmd_type; // RTKTransferFrame
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t dlen;       // the length of datas content
    uint8_t datas[110]; //
    uint8_t crc;
};

struct BASEPROTO_EXPORT ControlAdjustExtent{
    uint8_t magic1;
    uint8_t magic2;
    uint8_t type1;
    uint8_t type2;
    uint8_t type3;
    uint8_t RC[42];
    uint8_t Space[42];
    uint8_t crc;
};

#pragma pack(pop)

using ProtoMsgCallback = std::function<void(int status)>;

typedef VehicleMissionProto *PtrMissionMessage;
typedef ControlAdjustProto *PtrProtoMessage;
typedef RockerDeviceProto *PtrRockerMessage;
typedef SoftwareCommProto *PtrSoftwareMessage;

BASEPROTO_EXPORT void FillProtoTime(PtrRockerMessage proto);
BASEPROTO_EXPORT void FillProtoTime(ControlAdjustProto * proto);
BASEPROTO_EXPORT void FillProtoTime(VehicleMissionProto *proto);
