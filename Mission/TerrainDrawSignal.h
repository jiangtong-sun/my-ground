#pragma once

#include <QObject>
#include "StageController.h"
#include "FLineController.h"

class TerrainDrawSignal : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(TerrainDrawSignal)

  public:
    TerrainDrawSignal(QObject *parent = nullptr) : QObject(parent) {}
    Q_INVOKABLE void resetTerrainCurve() {
        emit resetDrawTerrain();
    }

  signals:
    void drawStageTerrain(StageController *);
    void drawFlineTerrain(FlineStageDetail *);
    void resetDrawTerrain();
};
