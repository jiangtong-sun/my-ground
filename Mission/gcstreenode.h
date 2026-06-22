#pragma once

#include <QtQml/qqml.h>

#include "GcsListmodel.h"
#include "GcsMissionDef.h"

class GcsTreeNode : public QObject {
    Q_OBJECT
    QPROPERTY_AUTO(int, uuid)
    QPROPERTY_AUTO(int, depth)
    QPROPERTY_AUTO(int, refUuid)
    QPROPERTY_AUTO(bool, checked)
    QPROPERTY_AUTO(bool, isExpanded)
    QPROPERTY_AUTO(bool, defNode)
    QPROPERTY_AUTO(bool, unsaved)
    QPROPERTY_AUTO(bool, isLock)
    QPROPERTY_AUTO(bool, newElem)

  public:
    explicit GcsTreeNode(QObject *parent = nullptr);
    ~GcsTreeNode();

    void addSubNode(GcsTreeNode *);
    GcsTreeNode *removeSubNode(GcsTreeNode *);
    GcsListmodel &nodes() { return _subNodes; }
    virtual QVariant columnValue(int column) const { return QVariant(); }
    Q_INVOKABLE QVariant get(int column);
    Q_INVOKABLE void displayTreeNode();
    Q_INVOKABLE bool hasChildren();

  protected:
    GcsListmodel _subNodes;
};
