#include "gcstreenode.h"
#include "log_category.h"

GcsTreeNode::GcsTreeNode(QObject *parent)
    : QObject{parent}, _uuid(0), _depth(1), _refUuid(0), _checked(false), _isExpanded(false),
      _unsaved(false), _defNode(false), _isLock(false), _newElem(false) {}

GcsTreeNode::~GcsTreeNode() {}

void GcsTreeNode::addSubNode(GcsTreeNode *node) {
    if (node == nullptr) {
        COMMON_WARN("invalid input param");
        return;
    }

    int mx = 0;
    const QList<QObject *> &objectList = _subNodes.objectList();
    for (auto x = objectList.begin(); x != objectList.end(); x++) {
        GcsTreeNode *node = reinterpret_cast<GcsTreeNode *>(*x);
        mx = mx < node->uuid() ? node->uuid() : mx;
    }

    node->uuid(mx + 1);
    node->refUuid(uuid());
    _subNodes.append(node);
}

GcsTreeNode *GcsTreeNode::removeSubNode(GcsTreeNode *node) {
    if (node != nullptr) {
        return reinterpret_cast<GcsTreeNode *>(_subNodes.removeOne(node));
    }

    return nullptr;
}

void GcsTreeNode::displayTreeNode() { ; }

bool GcsTreeNode::hasChildren() { return _subNodes.count() > 0 ? true : false; }

QVariant GcsTreeNode::get(int column) { return columnValue(column); }
