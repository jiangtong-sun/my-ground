#pragma once

#include "MissionItem.h"

#include <QGeoCoordinate>
#include <QObject>
#include <QVariant>

class QMissionPath : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(QMissionPath)

  public:
    Q_PROPERTY(int index READ index WRITE setIndex CONSTANT)
    Q_PROPERTY(QGeoCoordinate spos READ spos NOTIFY notifySPos)
    Q_PROPERTY(QGeoCoordinate epos READ epos NOTIFY notifyEPos)
    Q_PROPERTY(QString lineColor READ lineColor WRITE setLineColor NOTIFY colorChanged)

    QMissionPath(const QMissionItem *spos, const QMissionItem *epos, const QString &color = "blue");

    QGeoCoordinate spos(void) { return _spos->getPos(); }
    QGeoCoordinate epos(void) { return _epos->getPos(); }
    QString lineColor(void) { return _lcolor; }

    const QMissionItem *sposAddr() const { return _spos; }
    const QMissionItem *eposAddr() const { return _epos; }

    int index() { return _index; }
    void setIndex(int v) { _index = v; }
    void setLineColor(const QString &d) {
        _lcolor = d;
        emit colorChanged();
    }

  private slots:
    void waypointPosChanged();

  signals:
    void colorChanged();
    void notifySPos();
    void notifyEPos();

  protected:
    int _index = 0;
    QString _lcolor = "";
    const QMissionItem *_spos = nullptr;
    const QMissionItem *_epos = nullptr;
};
