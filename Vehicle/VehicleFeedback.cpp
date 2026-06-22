#include "VehilceFeedback.h"
#include "log_category.h"
#include <QDateTime>

bool VehicleFeedback::updateFeedback(const QString &expect, int row, int delay, bool state) {
    if (orginDatas.size() <= row || row < 0) {
        COMMON_WARN("index out of range {}", row);
        return false;
    }
    std::shared_ptr<CommElemNode> updates = orginDatas.at(row);
    if (updates != nullptr) {
        FeedbackDataNode *data = (FeedbackDataNode *) (updates.get());
        data->warn(!state ? true : false);
        data->delay(QString::number(delay));
        data->feedback(expect);
        /// 通知UI进行当前指令状态的更新
        emit currCommandChanged(data->content(), expect, state ? 1 : 2);
    }
    updateRow(row, updates.get());
    return true;
}

int VehicleFeedback::recordCommand(QString comandName) {
    std::shared_ptr<FeedbackDataNode> _curr_command = std::make_shared<FeedbackDataNode>();
    QDateTime utc_time = QDateTime::currentDateTime();
    qint64 veh_cmd_upt = utc_time.offsetFromUtc();
    QString utc_str = utc_time.toString("hh:mm:ss");
    _curr_command->key(veh_cmd_upt);
    _curr_command->time(utc_str);
    _curr_command->content(comandName);
    int rtn = appendRow(_curr_command);
    emit currCommandChanged(comandName, "", 1);
    return rtn;
}

void VehicleFeedback::initColumnHeader() {
    QStringList column;
    column.append("sendtime");
    column.append("content");
    columnSource(column);
}
