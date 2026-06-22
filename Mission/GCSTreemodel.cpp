#include "gcstreemodel.h"
#include "log_category.h"
#include <QMetaEnum>

GcsTreeModel::GcsTreeModel(QObject *parent) : QAbstractTableModel{parent} {}

int GcsTreeModel::rowCount(const QModelIndex &parent) const { return _dataSource.count(); }

int GcsTreeModel::columnCount(const QModelIndex &parent) const {
    return this->_columnSource.size();
}

QVariant GcsTreeModel::data(const QModelIndex &index, int role) const {
    switch (role) {
    case TreeModelRoles::RowModel:
        return QVariant::fromValue(_dataSource.at(index.row()));
    case TreeModelRoles::ColumnModel:
        return QVariant::fromValue(_columnSource.at(index.column()));
    default:
        break;
    }
    return {};
}

QHash<int, QByteArray> GcsTreeModel::roleNames() const {
    return {{TreeModelRoles::RowModel, "rowModel"}, {TreeModelRoles::ColumnModel, "columnModel"}};
}

void GcsTreeModel::removeRows(int row, int count) {
    if (row < 0 || row + count > _dataSource.size() || count == 0) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row + count - 1);
    QList<QObject *> firstPart = _dataSource.mid(0, row);
    QList<QObject *> secondPart = _dataSource.mid(row + count);
    _dataSource.clear();
    _dataSource.append(firstPart);
    _dataSource.append(secondPart);
    endRemoveRows();
}

GcsTreeNode *GcsTreeModel::select(int id) {
    if (id >= _dataSource.count() || id < 0) {
        return nullptr;
    }

    return (GcsTreeNode *) _dataSource.at(id);
}

void GcsTreeModel::remove(GcsTreeNode *data) {
    if (data != nullptr) {
        int parentRow = 0;
        auto mx = _dataSource.begin();
        for (; mx != _dataSource.end(); mx++, parentRow++) {
            GcsTreeNode *node = (GcsTreeNode *) (*mx);
            if ((data == node) && (data->uuid() >= 0)) {
                emit dataChanged(index(parentRow, 0), index(parentRow, 0));
                removeRows(parentRow, 1);
                break;
            }
        }
    }
}

void GcsTreeModel::append(GcsTreeNode *data) {
    int refUuid = data->refUuid();
    if (refUuid != 0) {
        int parentRow = 0;
        auto mx = _dataSource.begin();
        for (; mx != _dataSource.end(); mx++, parentRow++) {
            GcsTreeNode *node = (GcsTreeNode *) (*mx);
            if (node->uuid() == refUuid && node->isExpanded()) {
                parentRow += node->nodes().count();
                data->depth(node->depth() + 1);
                beginInsertRows(QModelIndex(), parentRow, parentRow);
                _dataSource.insert(parentRow, data);
                endInsertRows();
            }
        }
    } else {
        int rowIndex = _dataSource.count();
        beginInsertRows(QModelIndex(), rowIndex, rowIndex);
        _dataSource.insert(rowIndex, data);
        endInsertRows();
    }
}

void GcsTreeModel::insertRows(int row, const QList<QObject *> &data) {
    if (row < 0 || row > _dataSource.size() || data.empty()) {
        return;
    }

    beginInsertRows(QModelIndex(), row, row + data.size() - 1);
    QList<QObject *> firstPart = _dataSource.mid(0, row);
    QList<QObject *> secondPart = _dataSource.mid(row);
    _dataSource.clear();
    _dataSource.append(firstPart);
    _dataSource.append(data);
    _dataSource.append(secondPart);
    endInsertRows();
}

QObject *GcsTreeModel::getRow(int row) { return _dataSource.at(row); }

void GcsTreeModel::updateData(int row) {
    if (row >= 0 && row < rowCount()) {
        Q_EMIT dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
}

void GcsTreeModel::collapse(int row) {
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
    GcsTreeNode *modelData = (GcsTreeNode *) _dataSource.at(row);
    removeRows(row + 1, modelData->nodes().count());
    modelData->isExpanded(false);
}

void GcsTreeModel::expand(int row) {
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
    GcsTreeNode *modelData = (GcsTreeNode *) _dataSource.at(row);
    GcsListmodel &children = modelData->nodes();
    foreach (auto c, children.objectList()) {
        GcsTreeNode *node = (GcsTreeNode *) (c);
        node->depth(modelData->depth() + 1);
    }
    insertRows(row + 1, children.objectList());
    modelData->isExpanded(true);
}

GcsTreeNode *GcsTreeModel::getNode(int row) { return (GcsTreeNode *) _dataSource.at(row); }

void GcsTreeModel::resetChecked() {
    auto mx = _dataSource.begin();
    for (; mx != _dataSource.end(); mx++) {
        GcsTreeNode *node = (GcsTreeNode *) (*mx);
        if (node && node->checked()) {
            node->checked(false);
            GcsListmodel &children = node->nodes();
            foreach (auto c, children.objectList()) {
                GcsTreeNode * cnode = (GcsTreeNode *)c;
                cnode->checked(false);
            }
        }
    }
}

void GcsTreeModel::allExpand() {
    beginResetModel();
    QList<QObject *> stack;
    while (_dataSource.count() > 0) {
        GcsTreeNode *item = (GcsTreeNode *) _dataSource.back();
        _dataSource.pop_back();
        stack.append(item);
        if (item->refUuid() > 0) {
            item->isExpanded(true);
        }
        GcsListmodel &children = item->nodes();
        if (children.count() > 0) {
            foreach (auto c, children.objectList()) { stack.append((GcsTreeNode *) c); }
        }
    }
    _dataSource = stack;
    endResetModel();
}

void GcsTreeModel::allCollapse() {
    beginResetModel();
    QList<QObject *> stack;
    while (_dataSource.count() > 0) {
        GcsTreeNode *item = (GcsTreeNode *) _dataSource.back();
        _dataSource.pop_back();
        if (item->refUuid() > 0) {
            item->isExpanded(false);
            continue;
        }
        stack.push_front(item);
    }
    _dataSource = stack;
    endResetModel();
}
