#pragma once

#include <QJsonArray>
#include <QMap>
#include <QObject>
#include <QVector>

#include "GcsLineStage.h"
#include "GcsListmodel.h"
#include "GcsVisualItem.h"
#include "gcstreenode.h"

struct TypeWaypoint : public QObject {
    Q_OBJECT
    QPROPERTY_AUTO(int, value)
    QPROPERTY_AUTO(QString, name)
  public:
    TypeWaypoint() : _name(""), _value(0) {}
    TypeWaypoint(const QString &n, int v) : _name(n), _value(v) { ; }
};

struct StageCodeName : public QObject {
    Q_OBJECT
    QPROPERTY_AUTO(int, code)
    QPROPERTY_AUTO(bool, visible)
    QPROPERTY_AUTO(QString, name)
  public:
    StageCodeName() : _name(""), _code(0), _visible(true) {}
    StageCodeName(int code) : _code(code), _name(""), _visible(true) {}
    StageCodeName(int code, const QString &name) : _code(code), _name(name), _visible(true) {}
};

class StageController : public GcsTreeNode {
    Q_OBJECT
    Q_PROPERTY(GcsListmodel *visualPoints READ visualPoints NOTIFY visualPointsChanged)
    Q_PROPERTY(QVariantList dispayPoints READ dispayPoints NOTIFY dispayPointsChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString update READ update WRITE setUpdate NOTIFY updateChanged)
    Q_PROPERTY(QString color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(int stage READ stage WRITE setStage NOTIFY stageChanged)
    Q_PROPERTY(GcsListmodel *stagePointType READ stagePointType CONSTANT)
    Q_PROPERTY(QString stageAbbr READ stageAbbr CONSTANT)
    Q_PROPERTY(QString stageName READ stageName CONSTANT)
    Q_PROPERTY(int counts READ counts CONSTANT)
    QPROPERTY_AUTO(bool, canRMWaypoint)
    QPROPERTY_AUTO(bool, distState)
    QPROPERTY_AUTO(bool, visible)
    QPROPERTY_AUTO(bool, action)

  public:
    StageController(int stage = GcsLineStage::Templine);
    ~StageController();

    void initWaypointType();
    QString stageAbbr() const;
    QString stageName() const;
    QVariant columnValue(int column) const;

    void stage2Json(QJsonArray &jsonArr);
    void json2Stage(const QJsonObject &json);
    void json2Waypoint(const QByteArray &jsonArr);

    QByteArray waypoint2Json() const;

    int counts() const { return _currWaypoint.count(); }

    GcsListmodel *stagePointType() { return &_stagePointType; }

    QString color(void) const { return _color; }

    void setColor(QString d) {
        _color = d;
        emit colorChanged();
    }

    int stage(void) const { return _stage; }

    void setStage(int d);

    bool canRemove(void) {
        if (_manual.fetch_add(-1) <= 1) {
            return true;
        }
        return false;
    }

    int referCount() { return _manual.load(); }

    int addRefer();

    void setName(const QString &d) {
        _name = d;
        emit nameChanged();
    }

    QString name(void) const { return _name; }

    void setUpdate(const QString &d) {
        _udate = d;
        emit updateChanged();
    }

    QString update(void) const { return _udate; }

    int getMaxId();

    const QVisualItem *at(int i);

    Q_INVOKABLE void syncStateI(int type, int value);
    Q_INVOKABLE void syncStateF(int type, float value);

    Q_INVOKABLE StageController *clone();
    Q_INVOKABLE void saveAs(int vehId);
    Q_INVOKABLE void syncHeight(QString value);
    Q_INVOKABLE void removeDisplayResource(void);
    Q_INVOKABLE bool insertItem(QGeoCoordinate coordinate);
    Q_INVOKABLE bool removeWaypoint(QObject *);
    Q_INVOKABLE void recalWaypointPath();

    void recalAllWaypoint();

    ///
    /// \brief appendPoints
    /// \param points
    ///
    void appendPoints(GcsListmodel *points);
    void removePoints(GcsListmodel *points);

    ///
    /// \brief addItem
    /// \param point
    ///
    void addOrUpdate(ProtoNormWaypoint &point, int idx);

    ///
    /// \brief loadStageLine
    /// \param filename
    /// \return
    ///
    Q_INVOKABLE QGeoCoordinate loadStageLine(QString filename);

    ///
    /// \brief removeWaypointPath
    ///
    Q_INVOKABLE void removeWaypointPath();

    ///
    /// \brief terrain
    /// \param elemArr
    /// \param tempMap
    ///
    void terrain(QJsonArray &elemArr, QMap<int, QVisualItem *> &tempMap);

    GcsListmodel *visualPoints() { return &_currWaypoint; }
    QVariantList dispayPoints() { return _dispayPoints; }

    void releaseWaypointResource();
    void releasePathResource();
    void callDrawTerrainSignal();

    void updateWaypointColor(QString color);

    const QVisualItem *first();
    const QVisualItem *last();

  protected:
    // void takeoffConnect(QTakeoffItem *);
    // bool takeoffItem(QGeoCoordinate &coordinate);
    bool ergencyItem(QGeoCoordinate &coordinate);
    bool templineItem(QGeoCoordinate &coordinate);
    void addWaypoint(QGeoCoordinate &coordinate, int pointType);
    bool enterDepartItem(QGeoCoordinate &coordinate);
    void enterDepartBase(QGeoCoordinate &coordinate);
    bool landingItem(QGeoCoordinate &coordinate);

  private slots:
    void doUpdatePos();
    void takeoffPosChanged();

  signals:
    void dispayPointsChanged();
    void visualPointsChanged();
    void waypointPathChanged();
    void colorChanged();
    void updateChanged();
    void stageChanged();
    void nameChanged();

  protected:
    int _stage = 0;
    QString _name = "";
    QString _datas = "";
    QString _udate = "";
    QString _color = "#1080f0";
    std::atomic_int _manual = 0;
    QMap<uint64_t, QObject *> _path_map;
    QVariantList _dispayPoints;
    GcsListmodel _currWaypoint;
    GcsListmodel _stagePointType;
};
