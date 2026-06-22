#include "CommTableModel.h"
#include "log_category.h"

CommTableModel::CommTableModel(QObject *parent) : QAbstractTableModel{parent} {}

int CommTableModel::rowCount(const QModelIndex &parent) const {
    int rtn = _dataSource.count();
    return rtn;
}

int CommTableModel::columnCount(const QModelIndex &parent) const {
    return this->_columnSource.size();
}

QVariant CommTableModel::elementData(int row, int col) const {
    if (orginDatas.size() < row || row < 0) {
        return QVariant();
    }

    return _dataSource.at(row)->columnValue(col);
}

QVariant CommTableModel::elementState(int row) const {
    if (orginDatas.size() < row || row < 0) {
        return QVariant();
    }
    return _dataSource.at(row)->elemState();
}

QVariant CommTableModel::data(const QModelIndex &index, int role) const {
    switch (role) {
    case CommTableModel::ElemState:
        return elementState(index.row());
    case CommTableModel::ElemNode:
        return elementData(index.row(), index.column());
    case CommTableModel::ColumnName:
        return _columnSource.at(index.column());
    default:
        break;
    }
    return QVariant();
}

QHash<int, QByteArray> CommTableModel::roleNames() const {
    return {{CommTableModel::ElemNode, "elemNode"},
        {CommTableModel::ColumnName, "columnName"},
        {CommTableModel::ElemState, "elemState"}};
}

void CommTableModel::clear() {
    beginResetModel();
    this->_dataSource.clear();
    endResetModel();
}

int CommTableModel::updateRow(int rowIndex, CommElemNode *row) {
    _dataSource.replace(rowIndex, row);
    Q_EMIT dataChanged(index(rowIndex, 0), index(rowIndex, columnCount() - 1));
    return rowIndex;
}

int CommTableModel::insertRow(int rowIndex, std::shared_ptr<CommElemNode> row) {
    if (orginDatas.size() < rowIndex || rowIndex < 0) {
        COMMON_WARN("index out of range {}", rowIndex);
        return -1;
    }

    orginDatas.insert(rowIndex, row);

    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    _dataSource.insert(rowIndex, row.get());
    endInsertRows();
    return rowIndex;
}

int CommTableModel::removeRow(int rowIndex, int rows) {
    beginRemoveRows(QModelIndex(), rowIndex, rowIndex + rows - 1);
    _dataSource = _dataSource.mid(0, rowIndex) + _dataSource.mid(rowIndex + rows);
    endRemoveRows();
    return rowIndex;
}

int CommTableModel::appendRow(std::shared_ptr<CommElemNode> row) {
    int curr = rowCount();
    insertRow(curr, row);
    return curr;
}
