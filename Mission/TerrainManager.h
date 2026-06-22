#pragma once

#include "concurrentqueue.h"
#include "TerrainDrawSignal.h"
#include <future>

class TerrainThread : public QObject {
    typedef struct {
        int row;
        int col;
        double lng;
        double lat;
        double step;
        QString file;
    } DemNodeFile;

public:
    TerrainThread();
    ~TerrainThread();
    static TerrainThread& instance();
    void loadTerrain(const QString & folder);
    void terrain(QVisualItem* item);

    void setTerrainDraw(TerrainDrawSignal * );
    void elevation(StageController * );
    int calTerrain();
    void close();

signals:
    void notifyDrawTerrain(StageController*);

protected:
    void loadElevatorFromFile(StageController*);
    std::shared_ptr<DemNodeFile> terrainFile(double lng, double lat);

private:
    int datasize = 0;
    std::thread terrainRes;
    std::atomic_bool runflag = false;
    TerrainDrawSignal * terrainDraw = nullptr;
    std::shared_ptr<int16_t> terrainBuffer = nullptr;
    std::shared_ptr<TerrainThread::DemNodeFile> currentNode = nullptr;
    moodycamel::ConcurrentQueue<StageController *> stageQueue;
    QVector<std::shared_ptr<DemNodeFile>> terrainFileSet;
    bool dataReady = false;
};
