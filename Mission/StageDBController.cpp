#include "StageDBController.h"
#include "log_category.h"
#include "stringtranslate.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QMutexLocker>
#include <QTextCodec>

StageDBController::StageDBController(GcsDatabase *db, int stage) : stage_code(stage), select_index(-1), _missionDB(db) {
    QStringList names = GcsStageNames();
    stage_name = names[stage];
    _code_name.code(stage);
    _code_name.name(stage_name);
    _unsaved = false;
}

int StageDBController::stageCode() { return stage_code; }

void StageDBController::stageInitialize() {
    curr_stage = nullptr;
    _uuid = -stage_code;

    QStringList column;
    column.append(transToUnicode("Uuid"));
    column.append(transToUnicode("Name"));
    column.append(transToUnicode("Action"));
    stageTreeModel.columnSource(column);
}

QVariant StageDBController::columnValue(int column) const {
    QStringList names = GcsStageNames();
    if (column == 0) {
        return QVariant("");
    } else if (column == 1) {
        return names[stage_code];
    } else {
        return QVariant("");
    }
}

StageDBController::~StageDBController() {
    for (int i = 0; i < _subNodes.count(); i++) {
        StageController *vm = (StageController *) _subNodes[i];
        if ((vm != nullptr) && vm->canRemove()) {
            COMMON_TRACE("remove stage {}, uuid {}", (int) vm, vm->uuid());
            delete vm;
        }
    }
    _subNodes.clearAll();
}

QString StageDBController::assign(StageController *ctrl, int stage) {
    if (ctrl == nullptr) {
        return QString();
    }
    QString tm = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    ctrl->setName(QString("%1-%2").arg(stage).arg(tm));
    ctrl->setStage(stage);
    return ctrl->name();
}

bool StageDBController::insertWaypoint(QGeoCoordinate coordinate) {
    assert(curr_stage != nullptr);
    if (curr_stage == nullptr) {
        return false;
    }
    curr_stage->setStage(stage_code);
    curr_stage->insertItem(coordinate);
    return true;
}

void StageDBController::groupColor(const QString &color) {
    if (curr_stage) {
        curr_stage->setColor(color);
    }

    for (int i = 0; i < _subNodes.count(); i++) {
        StageController *ctrl = (StageController *) _subNodes.get(i);
        if (ctrl != nullptr) {
            ctrl->setColor(color);
        }
    }
}

void StageDBController::loadStageLine() {
    if (_missionDB != nullptr && !_load_flag.load()) {
        _load_flag = true;
        _missionDB->loadStage(_subNodes, stage_code);
        for (int i = 0; i < _subNodes.count(); i++) {
            stageTreeModel.append((StageController *) _subNodes[i]);
        }
    }
}

void StageDBController::loadAll() { setSelectId(-1); }

void StageDBController::saveStage(StageController *currStage) {
    if (currStage == nullptr) {
        COMMON_WARN("invalid input parameter");
        return;
    }

    if (_missionDB != nullptr) {
        QByteArray data = currStage->waypoint2Json();
        _missionDB->saveStage(currStage, data);
    }

    currStage->unsaved(false);
}

void StageDBController::removeStage(bool iscurr, int rowidx) {
    if (iscurr) {
        curr_stage->removeDisplayResource();
        setSelectId(-1);
    }
    if (rowidx >= 0) {
        removeStageData(_subNodes.value<StageController *>(rowidx));
        stageTreeModel.removeRows(rowidx, 1);
    }
}

void StageDBController::removeStageData(StageController *node) {
    StageController *stage = (StageController *) removeSubNode(node);
    if (stage != nullptr) {
        stage->releasePathResource();
        stage->releaseWaypointResource();
        if (_missionDB != nullptr) {
            _missionDB->removeStage(stage->name(), stage->stage(), stage->uuid());
        }
        setSelectId(-1);
    }
}

void StageDBController::removeItemByPath(const QMissionPath *elem) {
    if (elem != nullptr) {
        const QMissionItem *sposAddr = elem->sposAddr();
        removeWaypoint((QMissionItem *) sposAddr);
    }
}

void StageDBController::removeWaypoint(QObject *elem) {
    if (elem != nullptr && curr_stage != nullptr) {
        curr_stage->removeWaypoint(elem);
    }
}

void StageDBController::recalWaypointPath(StageController *node) {
    if (node != nullptr) {
        node->recalWaypointPath();
    }
}

void StageDBController::setSelectId(int id) {
    select_index = id;
    if (select_index >= 0) {
        curr_stage = (StageController *) _subNodes.get(select_index);
    }
    recalWaypointPath(curr_stage);
    emit notifySelected();
}

void StageDBController::updateStageLine() {
    if (curr_stage != nullptr) {
        QString tm = QDate::currentDate().toString("yyyy-MM-dd");
        curr_stage->setUpdate(tm);
        curr_stage->unsaved(false);
        saveStage(curr_stage);
    }
    setSelectId(-1);
}

bool StageDBController::addStage() {
    for (int i = 0; i < _subNodes.count(); i++) {
        StageController *vm = (StageController *) _subNodes[i];
        if (vm && vm->name().isEmpty()) {
            return false;
        }
    }
    QString tm = QDate::currentDate().toString("yyyy-MM-dd");
    curr_stage = new StageController(stage_code);
    curr_stage->setUpdate(tm);
    curr_stage->unsaved(false);
    curr_stage->uuid(1);

    curr_stage->addRefer();

    if (_subNodes.count() > 0) {
        StageController *tmp = nullptr;
        if ((tmp = (StageController *) _subNodes[_subNodes.count() - 1])) {
            curr_stage->uuid(tmp->uuid() + 1);
        } else {
            curr_stage->uuid(1);
        }
    }

    stageTreeModel.append(curr_stage);
    select_index = stageTreeModel.rowCount();

    addSubNode(curr_stage); // todo check

    return true;
}

bool StageDBController::saveStageLine(const QString &name) {
    for (int i = 0; i < _subNodes.count(); i++) {
        StageController *vm = (StageController *) _subNodes[i];
        if (vm && vm->name().compare(name) == 0) {
            return false;
        }
    }
    QString tm = QDate::currentDate().toString("yyyy-MM-dd");
    curr_stage->setName(name);
    curr_stage->setUpdate(tm);
    curr_stage->unsaved(false);
    saveStage(curr_stage);
    stageTreeModel.updateData(select_index);
    return true;
}

void StageDBController::resetCheck() {
    int size = _subNodes.count();
    StageController *tmp = nullptr;
    for (int ix = 0; ix < size; ix++) {
        tmp = (StageController *) _subNodes.get(ix);
        if (tmp != nullptr) {
            tmp->checked(false);
        }
    }
}

void StageDBController::addVisualItem(QGeoCoordinate coor) {
    if (curr_stage != nullptr) {
        curr_stage->insertItem(coor);
    }
}

void StageDBController::updateWaypointColor(QString color) {
    if (curr_stage != nullptr) {
        curr_stage->updateWaypointColor(color);
    }
}
