#include "MissionLine.h"
#include "log_category.h"

#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>

QMissionLine::QMissionLine(QObject *parent) : QObject(parent) {}

void QMissionLine::recalLocation() {

    COMMON_WARN("count waypoint {0} ", _itemSet.count());
    COMMON_WARN("count waypath {0} ", _waypath.count());
}

void QMissionLine::addPoint(std::shared_ptr<QMissionItem> item) {
    int get = _waypoints.append(item.get());
    _itemSet.insert(get, item);

    emit waypointsChanged();
}

void QMissionLine::addPointPath(std::shared_ptr<QMissionItem> st,
                                std::shared_ptr<QMissionItem> et) {
    std::shared_ptr<QMissionPath> cur(new QMissionPath(st.get(), et.get()));
    addPath(cur);
}

void QMissionLine::addPath(std::shared_ptr<QMissionPath> item) {
    item->setIndex(_pathSet.count());
    _waypath.append(item.get());
    _pathSet.append(item);
    emit waypathChanged();
}

void QMissionLine::removePath(QMissionPath *item) {
    _waypath.removeOne(item);
    _pathSet.removeAt(item->index());
    emit waypathChanged();
}

void QMissionLine::createMissionPath() {
    for (int i = 1; i < _waypoints.count(); i++) {
        QMissionItem *itemleft = (QMissionItem *) _waypoints.get(i - 1);
        QMissionItem *itemright = (QMissionItem *) _waypoints.get(i);
        std::shared_ptr<QMissionPath> cur(new QMissionPath(itemleft, itemright));
        addPath(cur);
    }
}

QGeoCoordinate QMissionLine::loadMissionData(const QString &missionFile) {
    QString fileName = missionFile;
    if (!QFileInfo::exists(missionFile)) {
        QUrl url(missionFile);
        fileName = url.toLocalFile();
        if (!QFileInfo::exists(fileName)) {
            COMMON_WARN("check {0} is exist or not", missionFile.toStdString().c_str());
            return QGeoCoordinate(22.461, 113.8919, 213.2);
        }
    }

    _itemSet.clear();
    _waypoints.clear();
    _pathSet.clear();
    _waypath.clear();

    QFile settings(fileName);

    std::shared_ptr<QMissionItem> first = nullptr;
    static QRegularExpression mLineSplitExpress("[\\n ,;；]");
    if (settings.open(QIODevice::Text | QIODevice::ReadOnly)) {

        while (!settings.atEnd()) {
            QByteArray line = settings.readLine();
            QString lineData = QString::fromStdString(line.data());
            QStringList sets = lineData.split(mLineSplitExpress, Qt::SkipEmptyParts);
            std::shared_ptr<QMissionItem> cur(new QMissionItem());
            if (first == nullptr) {
                first = cur;
            }

            if (sets.size() >= 4) {
                double lat = sets[2].toDouble();
                double lng = sets[1].toDouble();
                double alt = sets[3].toDouble();
                cur->setPos(QGeoCoordinate{lat, lng, alt});
                cur->setSeq(sets[0].toUInt());

                addPoint(cur);
            }
        }

        createMissionPath();
    }

    if (first == nullptr) {
        return QGeoCoordinate(22.461, 113.8919, 213.2);
    }

    return first->getPos();
}
