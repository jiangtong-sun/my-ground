#include "FLineDBController.h"
#include "log_category.h"
#include "stringtranslate.h"

#include <QFile>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>

FLineDBController::FLineDBController(GcsDatabase *db) : _missionDB(db) {
    connect(this, &FLineDBController::notifyLoadFline, this, &FLineDBController::loadFlightLine);

    QStringList column;
    column.append(transToUnicode("Code"));
    column.append(transToUnicode("Name"));
    column.append(transToUnicode("Action"));
    flineTreeModel.columnSource(column);
}

void FLineDBController::stageResetCallback(OnResetStageTree tree) { resetStageTree = tree; }

FLineDBController::~FLineDBController() {}

void FLineDBController::loadFile(FLineController *item) {}

void FLineDBController::saveAllFline() {
    auto mx = fline_datas.begin();
    while (mx != fline_datas.end()) {
        if (mx.key()->unsaved()) {
            saveOrUpdate((FLineController *) mx.key(), true);
        }
        ++mx;
    }
}

void FLineDBController::saveFile(FLineController *item) {

    QJsonDocument docu;
    QJsonObject obj;
    obj["data"] = item->item2Json();
    obj["name"] = item->name();
    obj["update"] = item->update();
    obj["uuid"] = item->uuid();
    docu.setObject(obj);

    QString filename = QTime::currentTime().toString("yyyy-MM-dd");
    filename.append(".line");
    QFile file(filename);

    QString value = docu.toJson(QJsonDocument::Compact);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(docu.toJson(QJsonDocument::Compact));
        file.close();
    } else {
        COMMON_WARN("openfile failed");
    }
}

void FLineDBController::loadFlightLine() {
    /// 从数据库里面添加航向
    if (_missionDB != nullptr && !_load_flag) {
        _load_flag = true;
        _missionDB->loadLine(fline_datas);
        for (auto i = fline_datas.begin(); i != fline_datas.end(); i++) {
            flineTreeModel.append(i.key());
            i.key()->unsaved(false);
        }

        createTempFline();
    }
}

void FLineDBController::createTempFline() {
    int uuid = 0;
    for (auto i = fline_datas.begin(); i != fline_datas.end(); i++) {
        FLineController *ctl = (FLineController *) (i.key());
        uuid = uuid > ctl->uuid() ? uuid : ctl->uuid();
    }

    empty_fline = std::shared_ptr<FLineController>(new FLineController());
    assert(empty_fline != nullptr);
    curr_line = empty_fline.get();
    curr_line->uuid(uuid + 1);
    curr_line->number(uuid + 1);
    curr_line->unsaved(true);

    fline_datas.insert(curr_line, empty_fline);
    flineTreeModel.append(curr_line);
}

void FLineDBController::loadAllFline() { emit notifyLoadFline(); }

void FLineDBController::saveOrUpdate(FLineController *item, bool update) {

    if (_missionDB != nullptr) {
        if (update) {
            _missionDB->updateLine(item, item->fline2Json());
        } else {
            _missionDB->saveLine(item, item->fline2Json());
        }
    } else {
        saveFile(item);
    }

    item->unsaved(false);
}

bool FLineDBController::verifyFlineName(const QString &name) {
    for (auto i = fline_datas.begin(); i != fline_datas.end(); i++) {
        FLineController *ctl = (FLineController *) (i.key());
        if (ctl->name().compare(name) == 0) {
            return true;
        }
    }
    return false;
}

bool FLineDBController::updateFlightLine(GcsTreeNode *node) {
    assert(node != nullptr);
    if (node == nullptr) {
        return true;
    }

    node->unsaved(false);

    FLineController *line = (FLineController *) node;
    line->update(QDate::currentDate().toString("yyyy-MM-dd"));

    saveOrUpdate(line, true);

    line->calcWaypointPath();

    return true;
}

bool FLineDBController::saveFlightLine(GcsTreeNode *node) {
    assert(node != nullptr);
    if (node == nullptr) {
        return true;
    }

    FLineController *line = (FLineController *) node;
    line->update(QDate::currentDate().toString("yyyy-MM-dd"));

    saveOrUpdate(line, false);

    line->calcWaypointPath();

    if (resetStageTree) {
        resetStageTree();
    }

    createTempFline(); ///
    emit notifyCurrFLine();
    return true;
}

void FLineDBController::removeFlightLine(GcsTreeNode *index) {
    auto gx = fline_datas.find(index);
    if (gx != fline_datas.end()) {
        if (_missionDB != nullptr) {
            _missionDB->removeLine(index->uuid());
        }

        /// 当前任务是选中的任务，则设置当前的任务为默认空任务
        FLineController *vv = (FLineController *) index;
        if (vv == curr_line) {
            curr_line = empty_fline.get();
            emit notifyCurrFLine();
        }

        flineTreeModel.remove(index);

        vv->calcWaypointPath();
        vv->clearStageResource();

        fline_datas.erase(gx);
    } else {
        COMMON_WARN("remove stage from line");
    }
}

void FLineDBController::selectFightline(GcsTreeNode *node) {
    auto gx = fline_datas.find(node);
    if (gx != fline_datas.end()) {
        curr_line = (FLineController *) (node);
    } else {
        curr_line = empty_fline.get();
    }

    if (curr_line) {
        curr_line->calcWaypointPath();
    }

    emit notifyCurrFLine();
}
