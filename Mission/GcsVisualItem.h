#pragma once

#include "GcsMissionDef.h"
#include "GcsMissionProto.h"
#include "MissionItem.h"
#include <QObject>
#include <QVariant>

enum WaypointType {
    NormalPoint = 0,       // 常规
    TakeoffPoint = 1,      // 起飞
    LandingPoint = 2,      // 着陆
    DeclinePoint = 3,      // 陡下滑
    DepartCircle = 4,      // 离场盘旋
    ArriveCircle = 5,      // 进场盘旋
    RtnHomePoint = 6,      // 返场点
    ForceLandPoint = 7,    // 迫降点
    ParkingPoint = 8,      // 停机位
    TurningsPoint = 9,     // 驶入驶出转弯
    PointATakeoff = 10,    // 跑道A起飞
    PointATurnaround = 11, // 跑道A掉头
    PointBTakeoff = 12,    // 跑道B起飞
    PointBTurnaround = 13  // 跑道B掉头
};

enum FlightHeightType {
    QNEType = 0,  // 标准大气压
    QNHType = 1,  // 修正海平面气压
    QFEType = 2,  // 场面气压高
    Relative = 3, // 相对场面
};

enum ClimpDeclineType {
    DirectType = 0, //直线爬升
    LeftCircle = 1, //左盘旋爬升/下降
    RightCircle = 2 //右盘旋爬升/下降
};

enum TurnaroundType {
    TurnEarly = 0,   // 提前
    PassThrought = 1 // 过点
};

enum TurnaroundRadius {
    FixSlopeType = 0,  //固定坡度转弯
    FixRadiusType = 1, //固定半径转弯
};

enum LandingGearAction {
    NoAction = 0,      //无动作
    PutDownAction = 1, //放起落架
    StowAction = 2,    //收起落架
};

class QVisualItem : public QMissionItem {
    Q_OBJECT
    Q_DISABLE_COPY(QVisualItem)
    QPROPERTY_AUTO(int, stageCode)
    QPROPERTY_AUTO(bool, canDrag)
    QPROPERTY_AUTO(bool, neareast)

  public:
    QVisualItem(int stage);
    Q_PROPERTY(bool dirty READ dirty WRITE setDirty NOTIFY notifyDirty)
    Q_PROPERTY(int stage READ stage WRITE setStage NOTIFY notifyStage)
    Q_PROPERTY(bool terrainState READ terrainState CONSTANT)
    Q_PROPERTY(int terrain READ terrain CONSTANT)
    bool dirty(void) const { return _dirty; }
    int stage(void) const { return _stage; }
    bool terrainState(void) const { return _terrainState; }
    void setTerrainState(bool va) { _terrainState = va; }
    int terrain(void) const { return _terrain; }
    void setTerrain(int va) { _terrain = va; }
    void setDirty(bool va) {
        _dirty = va;
        emit notifyDirty();
    }
    void setStage(int va) {
        _stage = va;
        emit notifyStage();
    }
  signals:
    void notifyDirty();
    void notifyAirspd();
    void notifyStage();

  protected:
    bool _dirty = false;
    bool _terrainState = false;
    int _terrain = 0;
    int _stage = 0;
};

class QNormalSItem : public QVisualItem {
    Q_OBJECT
    Q_PROPERTY(int heightMode READ heightMode WRITE setHeightMode NOTIFY heightModeChanged)
    Q_PROPERTY(int clampSlide READ clampSlide WRITE setClampSlide NOTIFY clampSlideChanged)
    Q_PROPERTY(int turnMode READ turnMode WRITE setTurnMode NOTIFY turnModeChanged)
    Q_PROPERTY(int nextMode READ nextMode WRITE setNextMode NOTIFY nextModeChanged)
    Q_PROPERTY(int pointType READ pointType WRITE setPointType NOTIFY pointTypeChanged)
    Q_PROPERTY(int rtnHeight READ rtnHeight WRITE setRtnHeight NOTIFY rtnHeightChanged)
    Q_PROPERTY(float yawAngle READ yawAngle WRITE setYawAngle NOTIFY yawAngleChanged)
    Q_PROPERTY(float turnRadius READ turnRadius WRITE setTurnRadius NOTIFY turnRadiusChanged)
    Q_PROPERTY(float speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(QGeoCoordinate nextPos READ nextPos CONSTANT)
    Q_PROPERTY(QString typeName READ typeName CONSTANT)
    QPROPERTY_AUTO(float, relativeHight)
    QPROPERTY_AUTO(float, runwayLen)

  signals:
    void yawAngleChanged();
    void turnModeChanged();
    void nextModeChanged();
    void clampSlideChanged();
    void pointTypeChanged();
    void heightModeChanged();
    void rtnHeightChanged();
    void turnRadiusChanged();
    void nextPosChanged(QGeoCoordinate);
    void speedChanged();

  public:
    QNormalSItem(int stage);
    QGeoCoordinate nextPos() { return _nextPos; }
    float speed() const { return normal.speed * MISSION_PRECISION2; }
    float yawAngle() const { return normal.yaw_radius * ALTITUDE_PRECISION; }
    float turnRadius() const { return normal.yaw_radius * MISSION_PRECISION2; }
    int rtnHeight() const { return normal.rth_height; }
    int turnMode() const { return normal.att_b_2; }
    int nextMode() const { return normal.att_b_3; }
    int clampSlide() const { return normal.att_b_4; }
    int pointType() const { return normal.att_a_0; }
    int heightMode() const { return normal.att_b_6; }

    QString typeName();

    void setSpeed(float val) {
        normal.speed = val / MISSION_PRECISION2;
        emit speedChanged();
    }

    void setRtnHeight(int val) {
        normal.rth_height = val;
        emit rtnHeightChanged();
    }

    void setYawAngle(float val) {
        normal.yaw_radius = val / ALTITUDE_PRECISION;
        emit yawAngleChanged();
    }

    void setTurnRadius(float val) {
        normal.yaw_radius = val / MISSION_PRECISION2;
        emit turnRadiusChanged();
    }

    void setTurnMode(int val) {
        normal.att_b_2 = val;
        emit turnModeChanged();
    }

    void setNextMode(int val) {
        normal.att_b_3 = val;
        emit nextModeChanged();
    }

    void setClampSlide(int val) {
        normal.att_b_4 = val;
        emit clampSlideChanged();
    }

    void setPointType(int val) {
        normal.att_a_0 = val;
        emit pointTypeChanged();
    }

    void setHeightMode(int val) {
        normal.att_b_6 = val;
        emit heightModeChanged();
    }

    NormalWaypoint &refNormal() { return normal; }

    const NormalWaypoint &normalInfo() const { return normal; }

    virtual QJsonObject item2Json() override;

    virtual void json2Item(const QJsonObject &) override;

  private slots:
    void takeoffChanged();

  protected:
    QGeoCoordinate _nextPos;
    NormalWaypoint normal;
};

class QTakeoffItem : public QVisualItem {
    Q_OBJECT
    Q_PROPERTY(float yawAngle READ yawAngle WRITE setYawAngle NOTIFY yawAngleChanged)
    QPROPERTY_AUTO(double, relativeHight)
    QPROPERTY_AUTO(double, runwayLen)
    QPROPERTY_AUTO(int, reserve)

  public:
    QTakeoffItem(int stage) : QVisualItem(stage), _relativeHight(100), _runwayLen(800), _reserve(0) { ; }

    float yawAngle() const { return _yawAngle * ALTITUDE_PRECISION; }

    void setYawAngle(float val) {
        _yawAngle = val / ALTITUDE_PRECISION;
        emit yawAngleChanged();
    }

    virtual QJsonObject item2Json() override;
    virtual void json2Item(const QJsonObject &) override;

  signals:
    void yawAngleChanged();

  private:
    uint16_t _yawAngle = 0;
};
