#include "GcsMissionCache.h"
#include "log_category.h"

MissionUpDownCache::~MissionUpDownCache() {
    MissionLineDataProto *currProto = nullptr;
    while (lineTrans.try_dequeue(currProto)) {
        delete currProto;
        currProto = nullptr;
    }
}

std::shared_ptr<MissionLineDataProto> MissionUpDownCache::downCache(int index) {
    if (index < 0 || index >= down_cache.size()) {
        return nullptr;
    }

    return down_cache[index];
}

std::shared_ptr<VehicleMissionProto> MissionUpDownCache::getUploadData(int index) {
    if (index < 0 || index >= cache_datas.size()) {
        return nullptr;
    }

    return cache_datas[index];
}

std::shared_ptr<VehicleMissionProto> MissionUpDownCache::nextUploadData() {
    MissionLineDataProto *rtnproto = nullptr;
    if (lineTrans.size_approx() > 0 && lineTrans.try_dequeue(rtnproto)) {
        std::shared_ptr<MissionLineDataProto> rtn(rtnproto);
        cache_datas.append(rtn);
        return rtn;
    }

    return nullptr;
}

void MissionUpDownCache::produceUploadData(FLineController *ftask) {
    assert(ftask != nullptr);
    int group = 0;

    clearMissionCache();

    QVector<MissionLineDataProto *> cacheProto;
    if (ftask->fillUpDataProto(cacheProto, GcsTaskLine::FTaskLineA)) {
        while (!cacheProto.isEmpty()) {
            MissionLineDataProto *dataproto = nullptr;
            dataproto = cacheProto.front();
            cacheProto.pop_front();
            lineTrans.enqueue(dataproto);
        }
    }
    cacheProto.clear();
    if (ftask->fillUpDataProto(cacheProto, GcsTaskLine::FTaskLineB)) {
        while (!cacheProto.isEmpty()) {
            MissionLineDataProto *dataproto = nullptr;
            dataproto = cacheProto.front();
            cacheProto.pop_front();
            lineTrans.enqueue(dataproto);
        }
    }

    cacheProto.clear();
    if (ftask->fillUpDataProtoExt(cacheProto, GcsTaskLine::FTaskErgency)) {
        while (!cacheProto.isEmpty()) {
            MissionLineDataProto *dataproto = nullptr;
            dataproto = cacheProto.front();
            cacheProto.pop_front();
            lineTrans.enqueue(dataproto);
        }
    }

    cacheProto.clear();
    if (ftask->fillUpDataProtoExt(cacheProto, GcsTaskLine::FTaskDepart)) {
        while (!cacheProto.isEmpty()) {
            MissionLineDataProto *dataproto = nullptr;
            dataproto = cacheProto.front();
            cacheProto.pop_front();
            lineTrans.enqueue(dataproto);
        }
    }
}

void MissionUpDownCache::produceUploadData(StageController *fline, int type) {
    int currindex = 0;
    int idxwaypoint = 0;
    int groupindex = 1;
    int counts = fline->counts();
    MissionLineDataProto *pproto = new MissionLineDataProto();
    memset(pproto, 0, sizeof(MissionLineDataProto));
    pproto->stage = TaskLineCode(type);
    pproto->number = fline->uuid();
    pproto->totalPoint = counts;
    pproto->groupNo = groupindex;

    clearMissionCache();

    while (currindex < counts) {
        const QVisualItem *item = fline->at(currindex);
        FlineStageDetail::assignWaypoint(pproto->point[idxwaypoint % 4], item, currindex + 1);
        ProtoCRC8((uint8_t *) &pproto->point[idxwaypoint % 4], sizeof(ProtoNormWaypoint), true);
        pproto->pointCount = ++idxwaypoint;
        if (idxwaypoint % 4 == 0) {
            lineTrans.enqueue(pproto);
            pproto = new MissionLineDataProto();
            memset(pproto, 0, sizeof(MissionLineDataProto));
            groupindex += 1;
            idxwaypoint = 0;
            pproto->stage = TaskLineCode(type);
            pproto->number = fline->uuid();
            pproto->totalPoint = counts;
            pproto->groupNo = groupindex;
        }

        currindex += 1;
    }

    if (idxwaypoint > 0) {
        lineTrans.enqueue(pproto);
    } else {
        delete pproto;
    }
}

void MissionUpDownCache::produceUploadData(FLineController *ftask, int type, int number) {
    assert(ftask != nullptr);
    QVector<MissionLineDataProto *> cacheProto;
    if (ftask->fillUpDataProto(cacheProto, type, number)) {
        while (!cacheProto.isEmpty()) {
            MissionLineDataProto *dataproto = nullptr;
            dataproto = cacheProto.front();
            cacheProto.pop_front();
            FillProtoTime(dataproto);
            lineTrans.enqueue(dataproto);
        }
    }
}

void MissionUpDownCache::setDownloadData(std::shared_ptr<MissionLineDataProto> data) {

    assert(data != nullptr);
    for (int ix = 0; ix < down_cache.size(); ix++){
        MissionLineDataProto * history = nullptr;
        history = down_cache.at(ix).get();
        if ((history != nullptr) && (history->number == data->number)
            && (history->stage == data->stage) && (history->groupNo == data->groupNo)) {
            COMMON_WARN("exist line data {} {} {}" , (int)data->stage, (int)data->number, data->groupNo);
            return;
        }
    }

    down_cache.append(data);
}

void MissionUpDownCache::clearMissionCache() {
    MissionLineDataProto *rtnproto = nullptr;
    while (lineTrans.size_approx() > 0 && lineTrans.try_dequeue(rtnproto)) {
        std::shared_ptr<MissionLineDataProto> rtn(rtnproto);
        cache_datas.append(rtn);
    }
    cache_datas.clear();
    down_cache.clear();
}
