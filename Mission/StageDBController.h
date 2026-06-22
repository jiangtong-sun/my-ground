#pragma once

#include "GCSTreemodel.h"
#include "MissionPath.h"
#include "SqliteDatabase.h"
#include "StageController.h"

#include <QObject>
#include <QVariant>

class StageDBController : public GcsTreeNode {
    Q_OBJECT
    Q_PROPERTY(int stageCode READ stageCode CONSTANT)
    Q_PROPERTY(QString stageName READ stageName CONSTANT)
    Q_PROPERTY(GcsTreeModel *dataModel READ dataModel CONSTANT)
    Q_PROPERTY(QStringList opearteBtn MEMBER _opearteBtn NOTIFY opearteBtnChanged)
    Q_PROPERTY(int selectId READ selectId WRITE setSelectId NOTIFY notifySelected)
    QPROPERTY_AUTO(bool, unsaved)

  public:
    StageDBController(GcsDatabase *db, int stage);
    ~StageDBController();

    QVariant columnValue(int column) const;
    QString stageName() { return stage_name; }

    int stageCode();
    void resetCheck();
    void stageInitialize();
    void saveStage(StageController *ctl);

    Q_INVOKABLE bool addStage();
    Q_INVOKABLE void updateStageLine();
    Q_INVOKABLE void addVisualItem(QGeoCoordinate);
    Q_INVOKABLE bool saveStageLine(const QString &);
    Q_INVOKABLE void updateWaypointColor(QString color);
    Q_INVOKABLE bool insertWaypoint(QGeoCoordinate coordinate);
    Q_INVOKABLE void loadAll();

    Q_INVOKABLE void removeWaypoint(QObject *elem);
    Q_INVOKABLE void removeItemByPath(const QMissionPath *elem);
    Q_INVOKABLE void removeStage(bool iscurr, int rowid);
    Q_INVOKABLE void recalWaypointPath(StageController *);

    int selectId() const { return select_index; }
    void setSelectId(int id);
    void removeStageData(StageController *);

    StageCodeName *codeName() { return &_code_name; }
    GcsTreeModel *dataModel() { return &stageTreeModel; }

    Q_INVOKABLE void groupColor(const QString &color);

  protected:
    QString assign(StageController *ctrl, int stage);

  public slots:
    void loadStageLine();

  signals:
    void notifySelected();
    void notifyUnsaved();
    void opearteBtnChanged();

  private:
    int stage_code = 0;
    int select_index = 0;
    QString stage_name = "";
    StageCodeName _code_name;
    QStringList _opearteBtn;
    GcsTreeModel stageTreeModel;
    std::atomic_bool _load_flag{false};
    StageController *curr_stage = nullptr;
    GcsDatabase *_missionDB = nullptr;
};
