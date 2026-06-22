#pragma once

#include "MissionPath.h"
#include "factlistmodel.h"
#include <QVariant>

class QMissionLine : public QObject {

    Q_OBJECT
    Q_DISABLE_COPY(QMissionLine)

  public:
    QMissionLine(QObject *parent = nullptr);

    Q_PROPERTY(FactListModel *waypath READ waypath NOTIFY waypathChanged)
    Q_PROPERTY(FactListModel *waypoints READ waypoints NOTIFY waypointsChanged)

    FactListModel *waypath(void) { return &_waypath; }
    FactListModel *waypoints() { return &_waypoints; }

    QGeoCoordinate loadMissionData(const QString &missionFile);

    void addPoint(std::shared_ptr<QMissionItem> item);

    void addPointPath(std::shared_ptr<QMissionItem> st, std::shared_ptr<QMissionItem> et);

    Q_INVOKABLE void createMissionPath();

    void addPath(std::shared_ptr<QMissionPath> item);

    void removePath(QMissionPath *item);

  public slots:
    void recalLocation();

  signals:
    void notifyLineColor();
    void waypointsChanged();
    void waypathChanged();

  protected:
    QMap<int, std::shared_ptr<QMissionItem>> _itemSet;
    FactListModel _waypoints;

    QList<std::shared_ptr<QMissionPath>> _pathSet;
    FactListModel _waypath;
};
