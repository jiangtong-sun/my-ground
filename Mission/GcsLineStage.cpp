#include "GcsLineStage.h"
#include "stringtranslate.h"
#include <QVariant>

QStringList GcsStageNames() {
    static QStringList names = {transToUnicode("【航线】"), transToUnicode("迫降"),     transToUnicode("围栏"),
                                transToUnicode("临时"),     transToUnicode("驶入驶出"), transToUnicode("起飞"),
                                transToUnicode("着陆"),     transToUnicode("巡航")};
    return names;
}

QString LineStage2Str(int stage) {
    static QString code2Str[] = {transToUnicode("综合航线"), transToUnicode("驶入驶出"), transToUnicode("迫降航线"),
                                 transToUnicode("临时航线"), transToUnicode("临时航线"), transToUnicode("未知航线")};
    return code2Str[stage];
}

int TaskLineCode(int taskType) {
    if (taskType == GcsTaskLine::FTaskLineA || taskType == GcsTaskLine::FTaskLineB) {
        return 0;
    } else if (taskType == GcsTaskLine::FTaskDepart) {
        return 1;
    } else if (taskType == GcsTaskLine::FTaskErgency) {
        return 2;
    } else if (taskType == GcsTaskLine::FTaskTempline) {
        return 3;
    } else if (taskType == GcsTaskLine::FTaskFence) {
        return 4;
    }
    return 5; // unknown...
}

QString FTaskLineName(int value) {
    if (value == GcsTaskLine::FTaskLineA) {
        return transToUnicode("综合航线A");
    }
    if (value == GcsTaskLine::FTaskLineB) {
        return transToUnicode("综合航线B");
    }
    if (value == GcsTaskLine::FTaskDepart) {
        return transToUnicode("驶入驶出");
    }
    if (value == GcsTaskLine::FTaskErgency) {
        return transToUnicode("迫降航线");
    }
    if (value == GcsTaskLine::FTaskTempline) {
        return transToUnicode("临时航线");
    }
    if (value == GcsTaskLine::FTaskFence) {
        return transToUnicode("围栏航线");
    }
    return QString();
}

const StageLimit &FlightStageLimit::GcsStageConfig(int stage) {
    if (stage == GcsLineStage::Depart) {
        return _enter;
    } else if (stage == GcsLineStage::Landing) {
        return _landing;
    } else if (stage == GcsLineStage::Ergency) {
        return _ergency;
    } else if (stage == GcsLineStage::Curise) {
        return _curise;
    } else if (stage == GcsLineStage::Templine) {
        return _templine;
    } else {
        return _nullLimit;
    }
}

QString FlightStageLimit::stageTypeName(int stage) {
    if (stage == GcsLineStage::Depart) {
        return transToUnicode("驶入驶出");
    } else if (stage == GcsLineStage::Landing) {
        return transToUnicode("着陆");
    } else if (stage == GcsLineStage::Ergency) {
        return transToUnicode("应急");
    } else if (stage == GcsLineStage::Curise) {
        return transToUnicode("任务");
    } else if (stage == GcsLineStage::Templine) {
        return transToUnicode("临时");
    } else {
        return transToUnicode("N/A");
    }
}

int FlightStageLimit::sameStageLimit(int code, int stage, int count) {

    switch (code) {
    case GcsTaskLine::FTaskLineA: {
        if (stage == GcsLineStage::Curise) {
            if (count >= _ftaskNormalA._Curise) {
                return GcsLineOperate::TaskStageOverLimit;
            }
        } else if (stage == GcsLineStage::Landing) {
            if (count >= _ftaskNormalA._Landing) {
                return GcsLineOperate::TaskStageOverLimit;
            }
        } else {
            return GcsLineOperate::UnableUpdateStage;
        }
        break;
    }
    case GcsTaskLine::FTaskLineB:
        if (stage == GcsLineStage::Curise) {
            if (count >= _ftaskNormalB._Curise) {
                return GcsLineOperate::TaskStageOverLimit;
            }
        } else if (stage == GcsLineStage::Landing) {
            if (count >= _ftaskNormalB._Landing) {
                return GcsLineOperate::TaskStageOverLimit;
            }
        } else {
            return GcsLineOperate::UnableUpdateStage;
        }
        break;
    case GcsTaskLine::FTaskDepart:
        if (stage == GcsLineStage::Depart) {
            if (count >= _ftaskDepart._Depart) {
                return GcsLineOperate::TaskStageOverLimit;
            }
        } else {
            return GcsLineOperate::UnableUpdateStage;
        }
        break;
    case GcsTaskLine::FTaskErgency:
        if (stage == GcsLineStage::Ergency) {
            if (count >= _ftaskErgency._Ergency) {
                return GcsLineOperate::TaskStageOverLimit;
            }
        } else {
            return GcsLineOperate::UnableUpdateStage;
        }
        break;
    case GcsTaskLine::FTaskTempline:
        if (stage == GcsLineStage::Templine) {
            if (count >= _ftaskTempline._Templine) {
                return GcsLineOperate::TaskStageOverLimit;
            }
        } else {
            return GcsLineOperate::UnableUpdateStage;
        }
        break;
    case GcsTaskLine::FTaskFence:
        if (stage == GcsLineStage::GisFence) {
            if (count >= _ftaskFence._GisFence) {
                return GcsLineOperate::TaskStageOverLimit;
            }
        } else {
            return GcsLineOperate::UnableUpdateStage;
        }
        break;
    }

    return 0;
}

FlightStageLimit::FlightStageLimit() {
    _ftaskNormalA._Curise = 1;
    _ftaskNormalA._Landing = 2;
    _ftaskNormalB._Curise = 1;
    _ftaskNormalB._Landing = 2;
    _ftaskErgency._Ergency = 16;
    _ftaskDepart._Depart = 4;
    _ftaskFence._GisFence = 4;
    _ftaskTempline._Templine = 2;
}

Q_GLOBAL_STATIC(FlightStageLimit, fstageMgr)

FlightStageLimit *FlightStageLimit::instance() { return fstageMgr(); }
