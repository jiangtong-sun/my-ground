#include "FlightDevice.h"
#include "log_category.h"

void FlightDevice::addStatus(std::shared_ptr<FlightStatus> node) {
    nodeList.append(node);
    _dataModel.append(node.get());
    _elemCount = _dataModel.count();
}

FlightStatus *FlightDevice::find(const QString &name) { return nodes(name); }

FlightStatus *FlightDevice::firstNode() {
    if (nodeList.size() > 0) {
        return nodeList.at(0).get();
    }

    return nullptr;
}

void FlightDevice::setSelectIndex(int x) {
    _select_node = x;
    emit selectIndexChanged();
    emit selectNodeChanged();
}

FlightStatus *FlightDevice::selectNode() {
    if (nodeList.size() > 0 && _select_node < nodeList.size()) {
        return nodeList.at(_select_node).get();
    }
    return nullptr;
}

FlightStatus * FlightDevice::deviceNode(int idx){
    if (nodeList.size() > 0 && idx < nodeList.size()) {
        return nodeList.at(idx).get();
    }

    return nullptr;
}

QString FlightDevice::deviceNodeName(int idx) {
    if (nodeList.size() > 0 && idx < nodeList.size()) {
        return nodeList.at(idx).get()->mapGroup();
    }
    return QString();
}

FlightStatus *FlightDevice::nodes(const QString &name) {
    for (auto gx = nodeList.begin(); gx != nodeList.end(); gx++) {
        if ((*gx)->mapGroup().compare(name) == 0) {
            return (*gx).get();
        }
    }

    COMMON_INFO("not found {} model", name.toLocal8Bit().toStdString());
    return nullptr;
}

void FlightDevice::setMapGroup(const QString &displayFrame) {

    QString vertName = FactMetaData::vertString(displayFrame);
    COMMON_TRACE("group name {}", vertName.toStdString().c_str());
    _mapGroup = vertName;
    _rawGroupName = displayFrame;
}
