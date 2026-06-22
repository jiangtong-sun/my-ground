#pragma once

#include <QObject>

class RedrawSignal : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(RedrawSignal)

  public:
    RedrawSignal(QObject *parent = nullptr) : QObject(parent) {}
    void notifyAddNotAvail(const QString &msg) { emit addressNotAvail(msg); }
    void linkTelemtryState(int veh, float datarate, int freq);
    void doStageReset() { emit stageResetSelect(); }

  signals:
    void redrawCurve();
    void stageResetSelect();
    void addressNotAvail(QString msg);
};
