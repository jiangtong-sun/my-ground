#include "FlightWarnGroup.h"
#include "log_category.h"
#include <QDateTime>

#define EACH_WARNMSG_SHOWFREQ 500
#define EACH_WARNMSG_TIMEOUT 10000

void FlightWarnGroup::calculateWarnGroup() {
    QDateTime utc_time = QDateTime::currentDateTimeUtc();
    qint64 tm = utc_time.toMSecsSinceEpoch();
    if (tm > warn_updatefreq) {
        warn_updatefreq = tm + EACH_WARNMSG_SHOWFREQ;
        auto mx = warnGroupCache.begin();
        while (mx != warnGroupCache.end()) {
            if (tm > mx->get()->timeStr() + EACH_WARNMSG_TIMEOUT) {
                mx->get()->state(false);
            }
            if (mx->get()->state()) {
                if (!mx->get()->manual()) {
                    bool get = mx->get()->show();
                    mx->get()->show(!get);
                } else if (mx->get()->manual()) {
                    mx->get()->show(true);
                }
            }
            ++mx;
        }

        auto bx = warnGroupCache.begin();
        while (bx != warnGroupCache.end()) {
            if (!bx->get()->state()) {
                _warnGroup.removeOne(bx->get());
                bx = warnGroupCache.erase(bx);
            } else {
                ++bx;
            }
        }
    }
}

bool FlightWarnGroup::addWarnGroup(int priority, int level, const QString &value) {
    if (value.isEmpty()) {
        return false;
    }

    QDateTime utc_time = QDateTime::currentDateTimeUtc();
    if (warnGroupCache.size() > 0) {
        auto mx = warnGroupCache.begin();
        while (mx != warnGroupCache.end()) {
            if (mx->get()->state() && value.compare(mx->get()->name()) == 0) {
                mx->get()->timeStr(utc_time.toMSecsSinceEpoch());
                return true;
            }
            ++mx;
        }
    }

    std::shared_ptr<WarnElemNode> newGroup(new WarnElemNode());

    newGroup->state(true);
    newGroup->level(level);
    newGroup->priority(priority);
    newGroup->name(value);
    newGroup->timeStr(utc_time.toMSecsSinceEpoch());
    QByteArray groupArr = value.toLocal8Bit();

    int same = -1;
    for (int ix = 0; ix < warnGroupCache.size(); ix++) {
        if (warnGroupCache.at(ix)->state() && (warnGroupCache.at(ix)->priority() > priority)) {
            same = ix;
            break;
        }
    }

    COMMON_INFO("insert warn node {} {} {}", same, (int)level, groupArr.toStdString());
    
    if (same >= 0) {
        warnGroupCache.insert(same, newGroup);
        _warnGroup.insert(same, newGroup.get());
    }
    else {
        warnGroupCache.append(newGroup);
        _warnGroup.append(newGroup.get());
    }

    return true;
}
