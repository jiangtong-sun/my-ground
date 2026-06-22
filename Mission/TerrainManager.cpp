#include "TerrainManager.h"
#include "log_category.h"
#include <QCoreApplication>
#include <QDirIterator>

const int tSIZE = 3601;

inline int16_t Big2Little(int16_t value) {
    int16_t val = (value & 0xff) << 8;
    val |= (value>>8)&0xff;
    return val;
}

TerrainThread& TerrainThread::instance(){
    static TerrainThread terrain;
    return terrain;
}

TerrainThread::~TerrainThread() {}

TerrainThread::TerrainThread(): terrainBuffer(nullptr), datasize(0){

}

std::shared_ptr<TerrainThread::DemNodeFile> TerrainThread::terrainFile(double lng, double lat){
    for (auto mx : terrainFileSet){
        double lngright = mx.get()->lng + mx.get()->step * mx.get()->col;
        double latbotm = mx.get()->lat - mx.get()->step * mx.get()->row;
        if (lng>= mx.get()->lng && lng <= lngright && lat >= latbotm && lat <= mx.get()->lat){
            if (mx.get()->col*mx.get()->row != datasize){
                datasize = mx.get()->col*mx.get()->row;
                terrainBuffer = std::shared_ptr<int16_t>(new int16_t[datasize]);
            }
            return mx;
        }
    }

    return nullptr;
}

void TerrainThread::loadTerrain(const QString & folder){
    static QRegularExpression splitExpress("[-_]");
    QStringList fileList;
    QDir dir(folder);

    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    foreach (auto fileInfo, fileInfoList) {
        if(!fileInfo.isFile()) {
            continue;
        }

        /// 文件后缀格式
        QString filename = fileInfo.fileName();
        int fileext = filename.indexOf(".hgt");
        if (fileext <= 0) {
            continue;
        }
        std::shared_ptr<DemNodeFile> node(new DemNodeFile());
        node->file = fileInfo.absoluteFilePath();
        QStringList sets = filename.mid(0, fileext).split(splitExpress);
        if (sets.size() >= 5) {
            node->lng = sets[0].toDouble();
            node->lat = sets[1].toDouble();
            node->step = sets[2].toDouble();
            node->row = sets[3].toDouble();
            node->col = sets[4].toDouble();
            terrainFileSet.append(node);
        }
    }
}

void TerrainThread::terrain(QVisualItem* item){
    double lng = item->getLng();
    double lat = item->getLat();
    if (item->terrainState()) {
        return;
    }

    bool inrange = false;
    if (currentNode != nullptr){
        double lngright = currentNode->lng + currentNode->step * currentNode->col;
        double latbotm = currentNode->lat - currentNode->step * currentNode->row;
        if (lng>= currentNode->lng && lng <= lngright && lat >= latbotm && lat <= currentNode->lat){
            inrange = true;
        }
    }

    if ((currentNode == nullptr) || !inrange) {
        currentNode = terrainFile(lng, lat);
        if (currentNode == nullptr) {
            COMMON_WARN("not found file {} {}", lng, lat);
            return;
        }

        std::ifstream file(currentNode->file.toStdString(), std::ios::binary);
        if (!file) {
            COMMON_WARN("not found file {}", currentNode->file.toStdString());
            return;
        }

        file.seekg(0, std::ios::end);
        int sz = file.tellg();
        if (sz != currentNode->row*currentNode->col*sizeof(short)) {
            COMMON_WARN("checking the contents of file");
            file.close();
            return;
        }

        file.seekg(0, std::ios::beg);
        short * elevationData = terrainBuffer.get();
        memset(elevationData, 0, currentNode->row*currentNode->col*sizeof(short));
        file.read(reinterpret_cast<char*>(elevationData), currentNode->col*currentNode->row*sizeof(short));
        file.close();
    }

    double lat_frac = (currentNode->lat - lat)/currentNode->step;
    double lon_frac = (lng - currentNode->lng)/currentNode->step;
    int frow = std::floor(lat_frac);
    int crow = std::ceil(lat_frac);
    int fcol = std::floor(lon_frac);
    int ccol = std::ceil(lon_frac);
    frow = qMin(qMax(frow, 0), currentNode->row);
    crow = qMin(qMax(crow, 0), currentNode->row);
    fcol = qMin(qMax(fcol, 0), currentNode->col);
    ccol = qMin(qMax(ccol, 0), currentNode->col);
    int16_t val1 = (terrainBuffer.get()[frow * currentNode->row + fcol]);
    int16_t val2 = (terrainBuffer.get()[frow * currentNode->row + ccol]);
    int16_t val3 = (terrainBuffer.get()[crow * currentNode->row + fcol]);
    int16_t val4 = (terrainBuffer.get()[crow * currentNode->row + ccol]);
    // val1 = Big2Little(val1);
    // val2 = Big2Little(val2);
    // val3 = Big2Little(val3);
    // val4 = Big2Little(val4);
    item->setTerrain((val1+val2+val3+val4)/4);
    item->setTerrainState(true);
}


void TerrainThread::loadElevatorFromFile(StageController* stage) {
    assert(stage != nullptr);
    GcsListmodel *visuals = stage->visualPoints();
    for (int i = 0;  visuals && i < visuals->count(); i++) {
        QVisualItem *item = visuals->value<QVisualItem *>(i);
        if (!item->terrainState()) {
            terrain(item);
        }
    }
}

void TerrainThread::close() {
    if (runflag) {
        runflag.store(false);
        COMMON_INFO("try cloes terrain");
        if (terrainRes.joinable()){
            terrainRes.join();
        }
    }
}

int TerrainThread::calTerrain(){

    while(runflag.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        StageController* stageData = nullptr;

        if (!stageQueue.try_dequeue(stageData)) {
            continue;
        }

        if (stageData == nullptr) {
            continue;
        }

        if (!runflag.load()) {
            return 1;
        }

        loadElevatorFromFile(stageData);

        if (stageData->canRemove()) {
            // COMMON_INFO("stage {}, uuid {}, stage {}, count {}",  (int)(stageData),
            //         stageData->uuid(), stageData->stage(), stageData->referCount());
            delete stageData;
            continue;
        }

        if ((terrainDraw != nullptr)){
            emit terrainDraw->drawStageTerrain(stageData);
        }
    }

    return 1;
}

void TerrainThread::setTerrainDraw(TerrainDrawSignal * draw){
    if (runflag) {
        return;
    }

    runflag = true; ///
    terrainDraw = draw;
    terrainRes = std::thread([=](){return calTerrain();});
}

void TerrainThread::elevation(StageController* line) {
    if (line == nullptr) {
        COMMON_INFO("invalid param");
        return;
    }

    int cont = line->addRefer();
    // COMMON_INFO("refer stage {}, uuid {}, stage {}, count {}",
    //              (int)(line), line->uuid(), line->stage(), cont);
    stageQueue.enqueue(line);
}
