#include "DataLinkerSummary.h"
#define DataLinkerQuality 25
DataLinkerSummary::DataLinkerSummary() {
    srand((unsigned int) time(nullptr));
    _working = false;
    _upState = false;
    _downState = false;
    _quality = 0;
}

void DataLinkerSummary::notifyUpdate() { emit summaryChanged(); }

int DataLinkerSummary::addRecvFrame(int datas) {
    _recv_frames += 1;
    _recv_framedata += datas;
    return _recv_frames - _last_recv_frames;
}

void DataLinkerSummary::calculateQuality(NetworkNodeConfig *conf) {
    _quality = _recv_frames - _last_recv_frames;
    // int datas = _recv_framedata - _last_recv_framedata;
    // if (conf->quality() > 0) {
    //     _quality = datas / conf->quality();
    // }

    _last_recv_frames = _recv_frames;
    _last_recv_framedata = _recv_framedata;
    _downState = _quality > 0 ? true : false;
}

void DataLinkerSummary::calculateQuality(SerialNodeConfig *conf) {
    _quality = _recv_frames - _last_recv_frames;
    // int datas = _recv_framedata - _last_recv_framedata;
    // if (conf->quality() > 0) {
    //     _quality = datas / conf->quality();
    // }
    _last_recv_frames = _recv_frames;
    _last_recv_framedata = _recv_framedata;
    _downState = _quality > 0 ? true : false;
}
