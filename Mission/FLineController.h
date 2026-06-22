#pragma once

#include "MissionPath.h"
#include "StageController.h"

#include <QJsonObject>
#include <QObject>
#include <QVariant>
#include <QVector>

class FLineController;
class FlineStageDetail : public StageCodeName {
    Q_OBJECT
    Q_PROPERTY(int totalPoint READ totalPoint CONSTANT)
    Q_PROPERTY(int countStage READ countStage NOTIFY countStagesChanged)
    Q_PROPERTY(StageController *selectStage READ selectStage NOTIFY selectStageChanged)
    Q_PROPERTY(GcsListmodel *connPaths READ connPaths NOTIFY connPathChanged)
    Q_PROPERTY(GcsListmodel *stagesMode READ stagesMode NOTIFY stagesModeChanged)
  public:
    FlineStageDetail(FLineController *fline, int lineCode);

    GcsListmodel *stagesMode() { return &stages_mode; }

    QVector<StageController *> &allStages() { return cache_stages; }

    int totalPoint();

    ///
    /// \brief showNeareastStage
    /// \param coordinate
    /// \param distance
    ///
    Q_INVOKABLE void showNeareastStage(QGeoCoordinate coordinate, int distance);

    ///
    /// \brief assignWaypoint
    /// \param point
    /// \param item
    ///
    static void assignWaypoint(ProtoNormWaypoint &point, const QVisualItem *item, int idex);

    bool fillUpDataProto(QVector<MissionLineDataProto *> &proto);
    bool fillUpDataProtoExt(QVector<MissionLineDataProto *> &proto);
    bool fillUpDataProto(QVector<MissionLineDataProto *> &proto, int index);

    ///
    /// \brief removeAllStages
    ///
    Q_INVOKABLE void removeAllStages();

    ///
    /// \brief connStageWaypoint
    ///
    void connStageWaypoint();
    void connectStageLine(StageController *, StageController *);

    ///
    /// \brief stage2Json
    ///
    void stage2Json(QJsonObject &);
    void json2Stage(QJsonObject &);
    void mergeStage(QJsonObject &);
    ///
    /// \brief removeWaypoint
    /// \param item
    /// \return
    ///
    bool removeWaypoint(QVisualItem *item) const;

    ///
    /// \brief isExist
    /// \param stage
    /// \return
    ///
    bool isExist(StageController *const stage) const;

    ///
    /// \brief loadStageFromFile
    /// \param file
    ///
    Q_INVOKABLE int loadStageFromFile(QString file);

    ///
    /// \brief selectStage
    /// \return
    ///
    StageController *selectStage() { return select_stage; }

    ///
    /// \brief deftStage
    /// \param stage
    ///
    void deftStage(StageController *stage);

    ///
    /// \brief newFlineStage
    /// \return
    ///
    Q_INVOKABLE int newFlineStage();

    ///
    /// \brief isfull
    /// \param stage
    /// \return
    ///
    Q_INVOKABLE bool isfull();

    StageController *newStage(GcsLineStage::LineStage);

    ///
    /// \brief isExist
    /// \param uuid
    /// \return
    ///
    StageController *isExist(int uuid, int stage);

    Q_INVOKABLE int appendRemoveStage(StageController *stage);
    Q_INVOKABLE int removeStage(StageController *stage);
    Q_INVOKABLE int appendStage(StageController *stage);

    QPoint getLandingSetpoints(QVariantList &maps);

    ///
    /// \brief currentTimeMs
    /// \return
    ///
    int64_t currentTimeMs();

    ///
    /// \brief stageNumber
    /// \return
    ///
    int stageNumber();

    ///
    /// \brief releseWaypointPath
    ///
    void releseWaypointPath();

    GcsListmodel *connPaths(void) { return &_waypointPath; }

    int countStage() { return stages_mode.count(); }

    Q_INVOKABLE void tryRemoveWaypoint(QObject *);
    Q_INVOKABLE void recalWaypointPath();

  signals:
    void notifyTerrainDraw(FlineStageDetail *);
    void removeWaypointSignal(QObject *);
    void countStagesChanged();
    void selectStageChanged();
    void stagesModeChanged();
    void connPathChanged();

  private slots:
    void doRemoveWaypoint(QObject *);

  private:
    int64_t uptNeareast = 0;
    GcsListmodel stages_mode;
    QMap<QObject *, bool> waypoints;
    FLineController *flineCtrl = nullptr;
    StageController *select_stage = nullptr;
    QVector<StageController *> cache_stages;
    QMap<uint64_t, QMissionPath *> waypath_map;
    GcsListmodel _waypointPath;
};

class FLineController : public GcsTreeNode {
    Q_OBJECT
    QPROPERTY_AUTO(bool, disable)
    QPROPERTY_AUTO(int, number)
    QPROPERTY_AUTO(QString, name)
    QPROPERTY_AUTO(QString, update)
    Q_PROPERTY(GcsListmodel *gmapStages READ gmapStages CONSTANT)
    Q_PROPERTY(GcsListmodel *taskStages READ taskStages CONSTANT)
    Q_PROPERTY(GcsListmodel *allStages READ allStages NOTIFY allStagesChanged)
    Q_PROPERTY(GcsListmodel *stagePath READ stagePath NOTIFY stagePathChanged)
    Q_PROPERTY(QVariantList landingMap READ landingMap NOTIFY flineLandChanged)

  public:
    FLineController();
    ~FLineController();

    QVariantList landingMap() { return _landMapNode; }

    ///
    /// \brief fillUpReqProto
    /// \param proto
    ///
    ///
    bool fillUpDataProto(QVector<MissionLineDataProto *> &, int type);
    bool fillUpDataProto(QVector<MissionLineDataProto *> &, int type, int index);
    bool fillUpDataProtoExt(QVector<MissionLineDataProto *> &, int type);

    ///
    /// \brief getLandingSetpoints
    /// \param maps
    ///
    void getLandingSetpoints(int typeLine);

    ///
    /// \brief fillUpReqProto
    ///     填充上传请求对象
    /// \param proto
    ///
    void fillUpReqProto(MissionUploadReqProto *proto);

    ///
    /// \brief columnValue
    /// \param column
    /// \return
    ///
    QVariant columnValue(int column) const override;

    ///
    /// \brief isExist
    /// \param stage
    /// \return
    ///
    bool isExist(StageController *const stage) const;

    ///
    /// \brief getStageLine
    /// \param stage
    /// \return
    ///
    int getTaskLineCode(StageController *const stage);

    ///
    /// \brief newStage
    /// \param type
    /// \return
    ///
    std::shared_ptr<FlineStageDetail> getStageDetial(int type);

    ///
    /// \brief item2Json
    /// \return
    ///
    QJsonObject item2Json() const;

    ///
    /// \brief fline2Json
    /// \return
    ///
    QByteArray fline2Json() const;

    ///
    /// \brief json2Item
    /// \param json
    ///
    void json2Item(const QByteArray &json);
    void mergeItem(const QByteArray &json);

    ///
    /// \brief mapLines
    /// \param taskline FTaskLine
    /// \return
    ///
    QVector<StageController *> &getTaskLines(int taskline) const;

    ///
    /// \brief allStages
    /// \return
    ///
    GcsListmodel *allStages(void) { return &_subNodes; }

    ///
    /// \brief taskStages
    /// \return
    ///
    GcsListmodel *taskStages() { return &_taskStages; }

    GcsListmodel *gmapStages() { return &_gmapStages; }

    GcsListmodel *stagePath() { return &_stagePaths; }

    int counts(int stage) const;

    void doRemoveWaypoint(QVisualItem *);

    Q_INVOKABLE void removeStageResource();

    Q_INVOKABLE void clearStageResource();

    Q_INVOKABLE int loadFlightLine(QString filename, bool resave);

    Q_INVOKABLE int exportFlightLine(QString file);

    Q_INVOKABLE void showErgencyLine(QGeoCoordinate coor, int distance);

    int newTemplineStage();

    void calcWaypointPath();

  signals:
    void allStagesChanged();
    void stagePathChanged();
    void flineLandChanged(QPoint node);

  protected:
    QMap<int, std::shared_ptr<FlineStageDetail>> stage_lines_map;
    FlineStageDetail *deft_fline_stage = nullptr;
    GcsListmodel _gmapStages;
    GcsListmodel _taskStages;
    GcsListmodel _stagePaths;
    QVariantList _landMapNode;
};
