#include "CommandItem.h"
#include "log_category.h"

RCCommandItem::RCCommandItem() : _working(false) {}

void RCCommandItem::toString() {
    COMMON_INFO("{}, {}, {}, {}", _index, _feed, _type.toStdString().c_str(), _name.toStdString().c_str());
}

RCAdjustItem::RCAdjustItem() : _metaData(), _working(false), _defaultValue() {
    _feedback = 0;
    _defSelect = false;
}

RCAdjustItem::~RCAdjustItem() {
    while (_moreMetaData.count() > 0) {
        QObject *elem = _moreMetaData.removeAt(0);
        FactMetaData *data = (FactMetaData *) (elem);
        if (data != nullptr) {
            delete data;
        }
    }
}

void RCAdjustItem::addMetaData(FactMetaData *metaData) {
    assert(metaData != nullptr);
    _moreMetaData.append(metaData);
}

FactMetaData *RCAdjustItem::getElem(int idx) {
    if (idx < 0 || idx > _moreMetaData.count()) {
        return nullptr;
    }

    return (FactMetaData *) (_moreMetaData.get(idx));
}

const FactMetaData *RCAdjustItem::fectElem(int idx) {
    if (idx < 0 || idx > _moreMetaData.count()) {
        return nullptr;
    }

    return (FactMetaData *) (_moreMetaData.get(idx));
}

QString RCAdjustItem::getCmdNameByCode(int code) {
    QString name = "";
    for (int ix = 0; ix < _moreMetaData.count(); ix++) {
        FactMetaData *data = _moreMetaData.value<FactMetaData *>(ix);
        if (data == nullptr) {
            continue;
        }

        QString tm = data->enumStrByKey(code);
        if (!tm.isEmpty()) {
            name = data->name() + tm;
            break;
        }
    }

    return name;
}

QString RCAdjustItem::vertSplitStr(const QString &value) {
    QString vertName = value;
    int ia = 0;
    int split = -1;
    for (ia = 1; ia < vertName.length(); ia++) {
        if ((vertName.at(ia - 1) > 256 && vertName.at(ia) < 256) ||
            (vertName.at(ia - 1) < 256 && vertName.at(ia) > 256)) {
            vertName.insert(ia, "\n");
            split = ia;
            break;
        }
    }
    for (ia = vertName.length() - 2; ia > split + 1; ia -= 2) {
        if (vertName.at(ia) > 256) {
            vertName.insert(ia, "\n");
        }
    }
    return vertName;
}

QString RCAdjustItem::vertName() const { return vertSplitStr(name()); }
