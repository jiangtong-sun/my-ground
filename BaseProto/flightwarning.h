#pragma once

#include "protometadef.h"
#include <QObject>

class BASEPROTO_EXPORT WarnElemNode : public QObject {
    Q_OBJECT
    PROTO_PROPERTY_RW(bool, state)
    PROTO_PROPERTY_RW(bool, show)
    PROTO_PROPERTY_RW(bool, manual)
    PROTO_PROPERTY_RW(int, level)
    PROTO_PROPERTY_RW(int, priority)
    PROTO_PROPERTY_RW(int, vehicle)
    PROTO_PROPERTY_RW(qint64, timeStr)
    PROTO_PROPERTY_RW(QString, name)
    PROTO_PROPERTY_RW(QString, frame)
    PROTO_PROPERTY_RW(QString, timestamp)
    PROTO_PROPERTY_RW(QString, group)
  public:
    WarnElemNode();
    WarnElemNode(const WarnElemNode *node);
    WarnElemNode(const WarnElemNode &node);
    WarnElemNode &operator=(const WarnElemNode &node);
    void header(std::ofstream &);
    void content(std::ofstream &);
    std::string content();
};

Q_DECLARE_METATYPE(WarnElemNode)
typedef std::shared_ptr<WarnElemNode> PWarnElemNode;
