#pragma once

#include "FLineController.h"
#include "StageController.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtSql/QSqlDatabase>

class GcsDatabase : public QObject {
    Q_OBJECT

  public:
    GcsDatabase();
    ~GcsDatabase();

    QString stageTable(int code);
    static QMap<int, QString> &stageMapName();

    void loadLine(QMap<GcsTreeNode *, std::shared_ptr<GcsTreeNode>> &line);
    bool saveLine(const FLineController *, const QByteArray &data);
    bool updateLine(const FLineController *, const QByteArray &data);
    bool removeLine(const int uid);

    void loadStage(GcsListmodel &stageList, int stage);
    bool saveStage(const StageController *, const QByteArray &data);
    bool removeStage(const QString &name, int stage, int id);
    void setDatabasePath(const QString &path);

    QString findStage(const QString &name, const QString &table);
    QString checkStage(int uid, const QString &table);

    bool connectDB();
    bool createDB();

    void disconnectDB();

    bool dbFlag();

  protected:
    QScopedPointer<QSqlDatabase> _db;
    QString _databaseFile;
    bool _dbflag = false;
};
