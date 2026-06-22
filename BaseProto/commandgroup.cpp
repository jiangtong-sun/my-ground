#include "CommandGroup.h"
#include "log_category.h"
#include <QDateTime>

RCCommandGroup::RCCommandGroup() {
    _name = "";
    _checked = false;
}

RCCommandGroup::~RCCommandGroup() {}

bool RCCommandGroup::resetWorkState(int code) {
    auto stage = cmd_cache.begin();
    while (stage != cmd_cache.end()) {
        if (code == stage->get()->feed()) {
            stage->get()->setWorking(false);
            return true;
        }

        ++stage;
    }
    return false;
}

void RCCommandGroup::resetChecking(){
    auto stage = cmd_cache.begin();
    while (stage != cmd_cache.end()) {
        stage->get()->setWorking(false);
        ++stage;
    }
}

void RCCommandGroup::insert(std::shared_ptr<RCCommandItem> item) {
    if (item == nullptr) {
        COMMON_WARN("input param is null pointer");
    }

    cmd_cache.append(item); /// cache
    _subcmds.append(item.get());
}

void RCCommandGroup::setGroupName(const QString &val) {
    QString vehType = val;
    for (int ia = vehType.length() - 2; ia > 0; ia -= 2) {
        vehType.insert(ia, '\n');
    }

    _name = val;
    _vertName = vehType;
}

RCCmdSubGroup::RCCmdSubGroup() { _name = ""; }

RCCommandItem *RCCmdSubGroup::firstNode() {
    if (cmd_cache.size() > 0) {
        return cmd_cache[0].get();
    }

    return nullptr;
}

RCCommandItem *RCCmdSubGroup::secondNode() {
    if (cmd_cache.size() > 1) {
        return cmd_cache[1].get();
    }

    return nullptr;
}

RCCommandItem *RCCmdSubGroup::thirdNode() {
    if (cmd_cache.size() > 2) {
        return cmd_cache[2].get();
    }

    return nullptr;
}

FactListModel *RCCmdSubGroup::subcommands() { return &_sub_cmds; }

void RCCmdSubGroup::insert(std::shared_ptr<RCCommandItem> item) {
    if (item == nullptr) {
        COMMON_WARN("input param is null pointer");
    }
    cmd_cache.append(item);
    _sub_cmds.append(item.get());
}

void RCCmdSubGroup::setGroupName(const QString &val) {
    QString vehType = val;
    for (int ia = vehType.length() - 2; ia > 0; ia -= 2) {
        vehType.insert(ia, '\n');
    }

    _name = val;
    _vertName = vehType;
}

void RCCommandGroupExt::insert(std::shared_ptr<RCCommandItem> item) {
    if (item == nullptr) {
        COMMON_WARN("input param is null pointer");
    }

    auto mx = command_group.find(item->group());
    if (mx != command_group.end()) {
        mx.value()->insert(item);
        int gsz = mx.value()->counts();
        _row_elem = gsz > _row_elem ? gsz : _row_elem;
    } else {
        std::shared_ptr<RCCmdSubGroup> newGroup(new RCCmdSubGroup());
        command_group.insert(item->group(), newGroup);
        _subcmds.append(newGroup.get());
        int gsz = newGroup->counts();
        _row_elem = gsz > _row_elem ? gsz : _row_elem;
        newGroup->setGroupName(item->group());
        newGroup->insert(item);
    }
}

void RCAdjustGroup::setGroupName(const QString &val) {
    QString vehType = val;
    for (int ia = vehType.length() - 2; ia > 0; ia -= 2) {
        vehType.insert(ia, '\n');
    }

    _groupName = val;
    _vertName = vehType;
}

RCAdjustItem *RCAdjustGroup::secondItem() const {
    if (_adjustcmd_set.count() > 1) {
        return _adjustcmd_set.at(1).get();
    }

    return nullptr;
}

RCAdjustItem *RCAdjustGroup::firstItem() const {
    if (_adjustcmd_set.count() > 0) {
        return _adjustcmd_set.at(0).get();
    }

    return nullptr;
}

QStringList RCAdjustGroup::groupList() {
    QStringList dataList;
    auto vv = _adjustcmd_set.begin();
    while (vv != _adjustcmd_set.end()) {
        dataList.append(vv->get()->name());
        ++vv;
    }
    return dataList;
}

RCAdjustItem *RCAdjustGroup::findSimpleItem(int itemNum) const {
    auto vv = _adjustcmd_set.begin();
    while (vv != _adjustcmd_set.end()) {
        if (vv->get() && vv->get()->itemNum() == itemNum) {
            return vv->get();
        }
        ++vv;
    }
    return nullptr;
}

RCAdjustItem *RCAdjustGroup::findItemByCmdType(int cmdType) {
    auto vv = _adjustcmd_set.begin();
    while (vv != _adjustcmd_set.end()) {
        if (vv->get()->commandType() == cmdType) {
            return vv->get();
        }
        ++vv;
    }

    return nullptr;
}

RCAdjustItem *RCAdjustGroup::findAdjustByFeed(int feedback) const{
    RCAdjustItem *feedItem = nullptr;
    auto vv = _adjustcmd_set.begin();
    while (vv != _adjustcmd_set.end()) {
        assert(vv->get() != nullptr);
        if (vv->get()->feedback() == feedback) {
            return vv->get();
        }
        ++vv;
    }
    return feedItem;
}

RCAdjustItem *RCAdjustGroup::findItem(int feedback, int cmdtype, int subtype) const {
    QList<RCAdjustItem *> sameNumCmds;
    //RCAdjustItem *feedItem = nullptr;
    auto vv = _adjustcmd_set.begin();
    while (vv != _adjustcmd_set.end()) {
        assert(vv->get() != nullptr); //
        if (vv->get()->feedback() == feedback &&
            vv->get()->itemNum() == cmdtype) {
            sameNumCmds.append(vv->get());
        }
        ++vv;
    }
    if(sameNumCmds.count() == 1) {
        return sameNumCmds.at(0);
    }
    else if((subtype != 0xffff) && sameNumCmds.count() > 0){
        for(int x = 0; x < sameNumCmds.count(); x++){
            if((sameNumCmds[x]->countSubMeta() > 0) &&
                sameNumCmds[x]->getElem(0)->isEnum()) {
                if (sameNumCmds[x]->getElem(0)->enumValueCheck(subtype) == subtype){
                    return sameNumCmds[x];
                }
            }
        }
    }

    return nullptr;
}

void RCAdjustGroup::add(std::shared_ptr<RCAdjustItem> elem) {
    _adjustcmd_set.append(elem);
    _adjustCmds.append(elem.get());
}
