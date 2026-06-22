#pragma once

#include <QMap>
#include <QObject>

class GcsLineOperate : public QObject {
    Q_OBJECT
  public:
    GcsLineOperate() : QObject() { ; }
    enum GcsOperate {
        UploadActionStart = 0,
        DownloadActionStart = 1,
        InvalidParameter = 2,
        LineStageIsFull = 3,
        UnableUptWaypoint = 4,
        InvalidWaypoints = 5,
        LineStageIsLock = 6,
        NoWaypointSpace = 7,
        UnableUpdateStage = 8,
        ResendOverCount = 9,
        UnsupportNotTempline = 10,
        MOperateTimeout = 11,
        CreateMissionFail = 12,
        MissionExportFailed = 13,
        MissionExportSuccess = 14,
        MissionLoadFailed = 15,
        MissionLoadSuccess = 16,
        MissionUploadSuccess = 17,
        MissionDownloadSuccess = 18,
        TaskOperationSuccess = 19,
        TaskStageOverLimit = 20,
        ProtoDataTransferErr = 21,
    };
    Q_ENUM(GcsOperate)
};

struct TaskLimit {
    uint8_t _TaskNorm = 0; //飞行任务
    uint8_t _Ergency = 0;  //迫降
    uint8_t _GisFence = 0; //围栏
    uint8_t _Templine = 0; //临时
    uint8_t _Depart = 0;   //驶入驶出
    uint8_t _Landing = 0;  //着陆
    uint8_t _Curise = 0;   //巡航

    int counts() { return _TaskNorm + _Ergency + _GisFence + _Templine + _Depart + _Landing + _Curise; }
};

class GcsLineStage : public QObject {
    Q_OBJECT
  public:
    GcsLineStage() : QObject() { ; }
    enum LineStage {
        TaskNorm = 0, //飞行任务
        Ergency = 1,  //迫降
        GisFence = 2, //围栏
        Templine = 3, //临时
        Depart = 4,   //驶入驶出
        Landing = 6,  //着陆
        Curise = 7    //巡航
    };
    Q_ENUM(LineStage)
};

class GcsTaskLine : public QObject {
    Q_OBJECT
  public:
    GcsTaskLine() : QObject() { ; }
    enum FTaskLine {
        FTaskInvalid = 0x0000,  // 无效航线
        FTaskLineA = 0x0010,    // 综合航线A
        FTaskLineB = 0x0020,    // 综合航线B
        FTaskDepart = 0x0001,   // 驶入驶出航线
        FTaskErgency = 0x0002,  // 迫降航线
        FTaskTempline = 0x1003, // 临时航线
        FTaskFence = 0x1004     // 围栏航线
    };
    Q_ENUM(FTaskLine)
};

int TaskLineCode(int taskType);
QString LineStage2Str(int);
QStringList GcsStageNames();
QString FTaskLineName(int value);

class GcsWaypointMode : public QObject {
    Q_OBJECT
  public:
    GcsWaypointMode() : QObject() { ; }
    enum WaypointMode {
        NormalPoint = 0,
        TakeoffPoint = 1,   //起飞点
        LandingPoint = 2,   //着陆点
        SlidingPoint = 3,   //陡下滑起点
        LevelPoint = 4,     //平飞起点
        TurnRoundPoint = 5, //盘旋点
        RtnHomePoint = 6,   //返航点
        ErgencyPoint = 7,   //应急迫降点
        StopPoint = 8,      //停机点
        WayPointA = 9,      //跑道A点
        WayPointB = 10      //跑道B点
    };
    Q_ENUM(WaypointMode)
};

class StageLimit : public QObject {
    Q_OBJECT
    Q_PROPERTY(int counts READ counts CONSTANT)
    Q_PROPERTY(int minElem READ minElem CONSTANT)
  public:
    uint8_t counts() const { return _counts; }
    uint8_t minElem() const { return _minElem; }

    StageLimit() : _counts(1), _minElem(1) {}
    StageLimit(uint8_t count, uint8_t min) : _counts(count), _minElem(min) {}

  private:
    uint8_t _counts = 1;
    uint8_t _minElem = 0xff;
};

class FlightStageLimit : public QObject {
    Q_OBJECT

  public:
    int sameStageLimit(int code, int type, int count);
    TaskLimit ftaskNormalA() { return _ftaskNormalA; }
    TaskLimit ftaskNormalB() { return _ftaskNormalB; }
    TaskLimit ftaskErgency() { return _ftaskErgency; }
    TaskLimit ftaskDepart() { return _ftaskDepart; }
    TaskLimit ftaskTempline() { return _ftaskTempline; }
    TaskLimit ftaskFence() { return _ftaskFence; }

    StageLimit *enterInOut();
    StageLimit *takeoff();
    StageLimit *landing();
    StageLimit *ergency();
    StageLimit *curise();
    StageLimit *templine();
    StageLimit *linefence();

    static QString stageTypeName(int stage);
    const StageLimit &GcsStageConfig(int);
    static FlightStageLimit *instance();

    FlightStageLimit();

  private:
    TaskLimit _ftaskNormalA;
    TaskLimit _ftaskNormalB;
    TaskLimit _ftaskErgency;
    TaskLimit _ftaskDepart;
    TaskLimit _ftaskTempline;
    TaskLimit _ftaskFence;

    StageLimit _nullLimit;

    StageLimit _enter = StageLimit(20, 3);
    StageLimit _takeoff = StageLimit(1, 1);
    StageLimit _landing = StageLimit(3, 3);
    StageLimit _curise = StageLimit(92, 1);
    StageLimit _ergency = StageLimit(100, 1);
    StageLimit _templine = StageLimit(100, 1);
    StageLimit _fence = StageLimit(100, 1);
};
