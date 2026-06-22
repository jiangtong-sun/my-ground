#include "FlightStatus.h"
#include "log_category.h"

FlightDisplay *FlightStatus::first(const QString &name) const {
    if (_mapGroup.compare(name) == 0 && nodeList.size() > 0) {
        return nodeList.at(0).get();
    }

    return nullptr;
}

FactListModel *FlightStatus::firstPart() { return &_dataModel[0]; }

FactListModel *FlightStatus::secondPart() { return &_dataModel[1]; }

FactListModel *FlightStatus::thridPart() { return &_dataModel[2]; }

FactListModel *FlightStatus::fourPart() { return &_dataModel[3]; }

FactListModel *FlightStatus::fivePart() { return &_dataModel[4]; }

FactListModel *FlightStatus::sixPart() { return &_dataModel[5]; }

void FlightStatus::addGroup(std::shared_ptr<FlightDisplay> node, const QString &group) {
    nodeList.append(node);
    FactListModel *submodel = subModal(group);
    if (submodel == nullptr) {
        QByteArray groupArr = group.toLocal8Bit();
        COMMON_INFO("new group {}", groupArr.toStdString().c_str());
        _subGroupStr.append(group);
        submodel = subModal(group);
        allDataModel.append(submodel);
        _countGroupElem.insert(group, 0);
    }
    if (submodel != nullptr) {
        submodel->append(node.get());
        _countGroupElem[group] = submodel->count();
    } else {
        _dataModel[0].append(node.get());
    }

    if (_mapGroup.isEmpty()) {
        setMapGroup(group);
    }
}

FactListModel *FlightStatus::subModal(const QString &sub) {
    int ix = 0;
    for (int ix = 0; ix < _subGroupStr.size() && ix < 10; ++ix) {
        if (!sub.isEmpty() && _subGroupStr[ix].compare(sub) == 0) {
            return &_dataModel[ix];
        }
    }

    return nullptr;
}

void FlightStatus::addDefault(std::shared_ptr<FlightDisplay> node) {
    nodeList.append(node);
    _dataModel[0].append(node.get());
}

void FlightStatus::setMapGroup(const QString &displayFrame) {
    QString vertName = FactMetaData::vert2String(displayFrame);
    _vertGroupName = vertName;
    _mapGroup = displayFrame;
}

void FlightStatus::setMadelFile(const QString &val) { _model_file = val; }
