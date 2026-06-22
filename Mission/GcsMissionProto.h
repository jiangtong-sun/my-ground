#pragma once

#include "MissionItem.h"
#include "prototelemetry.h"
#include <QVector>

#pragma pack(push, 1)
struct NormalWaypoint {
    uint16_t speed = 0;
    uint8_t att_a_0 : 5; // WaypointType
    uint8_t att_a_5 : 3; //
    uint8_t att_b_0 : 2; //
    uint8_t att_b_2 : 1; // TurnaroundRadius -- turnMode
    uint8_t att_b_3 : 1; // TurnaroundType -- nextMode
    uint8_t att_b_4 : 2; // ClimpDeclineType -- clampSlide
    uint8_t att_b_6 : 2; // FlightHeightType -- highType
    uint8_t att_c_0 : 2; // LandingGearAction
    uint8_t att_c_2 : 6; //
    uint8_t att_d = 0;   // 航点属性
    uint16_t yaw_radius = 0; // att_a_0 == 1 起飞航向; 2 着陆航向; 7 迫降航向; 9 转弯半径
    int16_t rth_height = 0; // att_a_0 == 6 返场高度（标高）
    uint8_t crc8 = 0;
};

struct ProtoNormWaypoint {
    SimpleWaypoint base;
    NormalWaypoint attr;
};

struct MissionLineDataProto : public VehicleMissionProto { // 航线数据传输
    uint8_t number : 5;
    uint8_t stage : 3;
    uint8_t totalPoint;
    uint8_t attr;
    uint8_t groupNo;
    uint8_t pointCount;
    ProtoNormWaypoint point[4];
    uint8_t crc;
};

struct MissionLineRespProto : public VehicleMissionProto { // 航线数据反馈
    uint8_t number : 5;
    uint8_t stage : 3;
    uint8_t groupNo;
    uint8_t errCode;
    uint8_t crc;
};

struct MissionLineCRCXProto : public VehicleMissionProto { // 航线数据校验
    uint8_t verify[16];
    uint8_t crc;
};

struct MissionUploadReqProto : public VehicleMissionProto { // 航线数据上传请求
    uint8_t normal;
    uint8_t departarrive;
    uint8_t ergency;
    uint8_t reserve;
    uint8_t crc;
};

struct TemplineUploadReqProto : public VehicleMissionProto { // 临时航线上传请求
    uint8_t templine;
    uint8_t reserve;
    uint8_t crc;
};

struct MissionDownloadReqProto : public VehicleMissionProto { // 航线数据下载请求
    uint8_t typeline;
    uint8_t crc;
};

// struct FlightTaskList {
//     uint8_t count;
//     uint8_t stage[48];
//     uint8_t reserve;
// };

// struct FlightExecList {
//     uint8_t count;
//     uint8_t stage[22];
//     uint8_t reserve;
// };

// struct FlightStage {
//     uint8_t stage : 3;
//     uint8_t number : 5;
//     uint8_t attri;
//     uint16_t startNo;
//     uint16_t endNo;
//     uint8_t counts;
//     uint8_t reserve;
// };

// struct TakeoffStage {
//     uint8_t stage : 3;
//     uint8_t number : 5;
//     uint8_t relativeHeight;
//     uint8_t runwayLen;
//     uint8_t reserve;
//     uint16_t takeoffAlt;
//     uint16_t yaw;
//     int32_t takeoffLng;
//     int32_t takeoffLat;
// };

// struct FlightWaypoint {
//     SimpleWaypoint simple;
//     NormalWaypoint normal;
// };

// struct FlightLineMem {
//     uint8_t line[50];
//     uint8_t linespace[14];
//     uint8_t exec[24];
//     uint8_t execspace[8];
//     uint8_t enterdepart[64];
//     uint8_t takeoff[128];
//     uint8_t landing[64];
//     uint8_t curise[128];
//     uint8_t ergency[64];
//     uint8_t stagespace[16];
//     uint8_t waypoint[70656];
//     uint8_t pointspace[16];
//     uint8_t fence[8 * (1024 + 8)];
//     uint8_t fencespace[16];
//     uint8_t templine[8 * (1024 + 8)];
//     uint8_t templinespace[16];
// };

#pragma pack(pop)
