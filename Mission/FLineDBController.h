#pragma once

#include <QObject>
#include <QVariant>

#include "FLineController.h"
#include "GCSTreemodel.h"
#include "SqliteDatabase.h"

using OnResetStageTree = std::function<void()>;

class FLineDBController : public QObject {
    Q_OBJECT
    Q_PROPERTY(GcsTreeModel *tableModel READ tableModel CONSTANT)
    Q_PROPERTY(FLineController *currFline READ currFline NOTIFY notifyCurrFLine)

  public:
    FLineDBController(GcsDatabase *db);
    ~FLineDBController();

    void loadAllFline();
    void saveAllFline();

    void loadFile(FLineController *item);
    void saveFile(FLineController *item);

    FLineController *currFline() { return curr_line; }
    GcsTreeModel *tableModel() { return &flineTreeModel; }

    bool verifyFlineName(const QString &);

    Q_INVOKABLE bool updateFlightLine(GcsTreeNode *line);
    Q_INVOKABLE bool saveFlightLine(GcsTreeNode *line);
    Q_INVOKABLE void removeFlightLine(GcsTreeNode *line);
    Q_INVOKABLE void selectFightline(GcsTreeNode *line);

    void stageResetCallback(OnResetStageTree);

  protected:
    void createTempFline();
    void saveOrUpdate(FLineController *item, bool update);

  public slots:
    void loadFlightLine();

  signals:
    void notifyCurrFLine();
    void notifyLoadFline();

  private:
    std::atomic_bool _load_flag = false;

    GcsTreeModel flineTreeModel;
    QMap<GcsTreeNode *, std::shared_ptr<GcsTreeNode>> fline_datas;
    std::shared_ptr<FLineController> empty_fline;
    OnResetStageTree resetStageTree = nullptr;
    FLineController *curr_line = nullptr;
    GcsDatabase *_missionDB = nullptr;
};
