#include "FlightWarning.h"
#include <QDateTime>
#include <fstream>
#include <sstream>

WarnElemNode::WarnElemNode() {
    _state = false;
    _show = true;
    _manual = false;
}

WarnElemNode::WarnElemNode(const WarnElemNode *node) {
    _state = node->_state;
    _level = node->_level;
    _show = node->_show;
    _manual = node->_manual;
    _timeStr = node->_timeStr;
    _priority = node->_priority;
    _vehicle = node->_vehicle;
    _timestamp = node->_timestamp;
    _frame = node->_frame;
    _group = node->_group;
    _name = node->_name;
}

WarnElemNode::WarnElemNode(const WarnElemNode &node) {
    _state = node._state;
    _level = node._level;
    _show = node._show;
    _manual = node._manual;
    _timeStr = node._timeStr;
    _priority = node._priority;
    _vehicle = node._vehicle;
    _timestamp = node._timestamp;
    _frame = node._frame;
    _group = node._group;
    _name = node._name;
}

WarnElemNode &WarnElemNode::operator=(const WarnElemNode &node) {
    if (&node == this) {
        return *this;
    }

    _state = node._state;
    _level = node._level;
    _show = node._show;
    _manual = node._manual;
    _timeStr = node._timeStr;
    _priority = node._priority;
    _vehicle = node._vehicle;
    _timestamp = node._timestamp;
    _frame = node._frame;
    _group = node._group;
    _name = node._name;

    return *this;
}

void WarnElemNode::header(std::ofstream &outstr) {
    outstr << "vehicle\t"
           << "priority\t"
           << "level\t"
           << "frame\t"
           << "group\t"
           << "name\t"
           << "timestamp\t\n";
}

void WarnElemNode::content(std::ofstream &outstr) {
    outstr << _vehicle << "\t" << _priority << "\t" << _level << "\t" << _group.toStdString()
           << "\t" << _frame.toStdString() << "\t" << _name.toStdString() << "\t"
           << _timestamp.toStdString() << "\t\n";
}

std::string WarnElemNode::content() {
    std::stringstream buffer;
    buffer << _vehicle << "\t" << _priority << "\t" << _level << "\t" << _group.toStdString()
        << "\t" << _frame.toStdString() << "\t" << _name.toStdString() << "\t"
        << _timestamp.toStdString() << "\t\n";
    return buffer.str();
}
