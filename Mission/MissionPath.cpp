#include "MissionPath.h"

QMissionPath::QMissionPath(const QMissionItem *spos, const QMissionItem *epos, const QString &color)
    : QObject(), _lcolor(color), _spos(spos), _epos(epos), _index(0) {
    connect(spos, &QMissionItem::positionChanged, this, &QMissionPath::waypointPosChanged);
    connect(epos, &QMissionItem::positionChanged, this, &QMissionPath::waypointPosChanged);
}

void QMissionPath::waypointPosChanged() {
    emit notifySPos();
    emit notifyEPos();
}
