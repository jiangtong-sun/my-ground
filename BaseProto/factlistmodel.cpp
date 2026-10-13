#include "factlistmodel.h"

#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>

const int FactListModel::ObjectRole = Qt::UserRole;
const int FactListModel::TextRole = Qt::UserRole + 1;

FactListModel::FactListModel(QObject *parent) : QAbstractListModel(parent) {}

FactListModel::~FactListModel() {}

/**
 * @brief FactListModel::get 获取指定索引处的对象
 * @param index 要获取的对象的索引位置
 * @return 返回指定索引处的QObject指针，如果索引无效则返回nullptr
 */
QObject *FactListModel::get(int index){
    // 检查索引是否有效，如果小于0或大于等于列表大小则返回nullptr
    if (index < 0 || index >= _objectList.count()) {
        return nullptr;
    }

    // 返回指定索引处的对象
    return _objectList[index];
}

int FactListModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return _objectList.count();
}

QVariant FactListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= _objectList.count()) {
        return QVariant();
    }

    if (role == ObjectRole) {
        return QVariant::fromValue(_objectList[index.row()]);
    } else if (role == TextRole) {
        return QVariant::fromValue(_objectList[index.row()]->objectName());
    } else {
        return QVariant();
    }
}

QHash<int, QByteArray> FactListModel::roleNames(void) const {
    QHash<int, QByteArray> hash;
    hash[ObjectRole] = "object";
    hash[TextRole] = "text";
    return hash;
}

bool FactListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid() && role == ObjectRole) {
        _objectList.replace(index.row(), value.value<QObject *>());
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

bool FactListModel::insertRows(int position, int rows, const QModelIndex &parent) {
    Q_UNUSED(parent);

    if (position < 0 || position > _objectList.count() + 1) {
        return true;
    }

    beginInsertRows(QModelIndex(), position, position + rows - 1);
    endInsertRows();

    emit countChanged(count());

    return true;
}

bool FactListModel::removeRows(int position, int rows, const QModelIndex &parent) {
    Q_UNUSED(parent);

    if (position < 0 || position >= _objectList.count()) {
        return true;
    } else if (position + rows > _objectList.count()) {
        return true;
    }

    beginRemoveRows(QModelIndex(), position, position + rows - 1);
    for (int row = 0; row < rows; row++) {
        _objectList.removeAt(position);
    }
    endRemoveRows();

    emit countChanged(count());

    return true;
}

void FactListModel::move(int from, int to) {
    if (0 <= from && from < count() && 0 <= to && to < count() && from != to) {
        if (from == to - 1) {
            to = from++;
        }
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        _objectList.move(from, to);
        endMoveRows();
    }
}

QObject *FactListModel::operator[](int index) {
    if (index < 0 || index >= _objectList.count()) {
        return nullptr;
    }

    return _objectList[index];
}

const QObject *FactListModel::operator[](int index) const {
    if (index < 0 || index >= _objectList.count()) {
        return nullptr;
    }
    return _objectList[index];
}

void FactListModel::clear() {
    beginResetModel();
    _objectList.clear();
    endResetModel();
    emit countChanged(count());
}

QObject *FactListModel::removeAt(int i) {
    QObject *removedObject = _objectList[i];
    removeRows(i, 1);
    return removedObject;
}

void FactListModel::insert(int i, QObject *object) {
    if (i < 0 || i > _objectList.count()) {
        return;
    }

    if (object) {
        QQmlEngine::setObjectOwnership(object, QQmlEngine::CppOwnership);
    }

    _objectList.insert(i, object);

    insertRows(i, 1);
}

/**
 * @brief 向模型中追加对象列表
 * @param objects 要追加的对象哈希表，键为uint32_t类型的ID，值为QObject指针
 */
void FactListModel::append(const QHash<uint32_t, QObject *> &objects) {
    int j = 0;  // 用于记录插入位置的索引
    // 如果传入的对象哈希表为空，则直接返回
    if (objects.count() <= 0) {
        return;
    }

    // 遍历传入的对象哈希表
    for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
        // 设置对象的所有权为C++端，防止被QML垃圾回收
        QQmlEngine::setObjectOwnership(iter.value(), QQmlEngine::CppOwnership);
        // 将对象插入到内部对象列表中
        _objectList.insert(j, iter.value());
        j++;  // 递增插入位置索引
    }

    // 在模型的开头插入新行，行数等于追加的对象数量
    insertRows(0, objects.count());
}

void FactListModel::insert(int i, const QList<QObject *> &objects) {
    if (objects.length() <= 0) {
        return;
    }

    int j = i;
    for (int a = 0; a < objects.count(); a++) {
        QQmlEngine::setObjectOwnership(objects.at(a), QQmlEngine::CppOwnership);
        _objectList.insert(j, objects.at(a));
        j++;
    }

    insertRows(i, objects.count());
}

/**
 * @brief FactListModel::append 在对象列表末尾添加一个新对象
 * @param object 要添加的QObject指针
 * @return 返回新添加对象在列表中的索引位置
 */
int FactListModel::append(QObject *object) {
    int cur = _objectList.count();  // 获取当前列表中的对象数量
    insert(_objectList.count(), object);  // 在列表末尾插入新对象
    return cur;  // 返回新添加对象的索引位置
}

void FactListModel::append(const QList<QObject *> &objects) {
    insert(_objectList.count(), objects);
}

void FactListModel::replace(const QList<QObject *> &objects) {
    int position = objects.count();
    int rows = count() - position;

    beginResetModel();

    if (rows > 0) {
        for (int row = position; row < position + rows; row++) {
            _objectList.removeAt(position);
        }
    }

    for (int a = 0; a < _objectList.count(); a++) {
        QQmlEngine::setObjectOwnership(objects.at(a), QQmlEngine::CppOwnership);
        _objectList[a] = objects.at(a);
    }

    for (int am = _objectList.count(); am < objects.count(); ++am) {
        QQmlEngine::setObjectOwnership(objects.at(am), QQmlEngine::CppOwnership);
        _objectList.insert(am, objects.at(am));
    }

    endResetModel();

    emit countChanged(_objectList.count());
}

int FactListModel::count() const { return rowCount(); }

void FactListModel::clearAll() {
    beginResetModel();
    _objectList.clear();
    endResetModel();
    emit countChanged(count());
}

void FactListModel::beginReset() { beginResetModel(); }

void FactListModel::endReset() { endResetModel(); }
