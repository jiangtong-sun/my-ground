#include "GcsListmodel.h"

#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>

const int GcsListmodel::ObjectRole = Qt::UserRole;
const int GcsListmodel::TextRole = Qt::UserRole + 1;

GcsListmodel::GcsListmodel(QObject *parent) : QAbstractListModel(parent) {}

GcsListmodel::~GcsListmodel() {}

QObject *GcsListmodel::get(int index) {
    if (index < 0 || index >= _objectList.count()) {
        return nullptr;
    }

    return _objectList[index];
}

int GcsListmodel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return _objectList.count();
}

QVariant GcsListmodel::data(const QModelIndex &index, int role) const {
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

QHash<int, QByteArray> GcsListmodel::roleNames(void) const {
    QHash<int, QByteArray> hash;
    hash[ObjectRole] = "object";
    hash[TextRole] = "text";
    return hash;
}

bool GcsListmodel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid() && role == ObjectRole) {
        _objectList.replace(index.row(), value.value<QObject *>());
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

bool GcsListmodel::insertRows(int position, int rows, const QModelIndex &parent) {
    Q_UNUSED(parent);

    if (position < 0 || position > _objectList.count() + 1) {
        return true;
    }

    beginInsertRows(QModelIndex(), position, position + rows - 1);
    endInsertRows();

    emit countChanged(count());

    return true;
}

bool GcsListmodel::removeRows(int position, int rows, const QModelIndex &parent) {
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

void GcsListmodel::move(int from, int to) {
    if (0 <= from && from < count() && 0 <= to && to < count() && from != to) {
        if (from == to - 1) {
            to = from++;
        }
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        _objectList.move(from, to);
        endMoveRows();
    }
}

QObject *GcsListmodel::operator[](int index) {
    if (index < 0 || index >= _objectList.count()) {
        return nullptr;
    }

    return _objectList[index];
}

const QObject *GcsListmodel::operator[](int index) const {
    if (index < 0 || index >= _objectList.count()) {
        return nullptr;
    }
    return _objectList[index];
}

void GcsListmodel::clear() {
    beginResetModel();
    _objectList.clear();
    endResetModel();
    emit countChanged(count());
}

QObject *GcsListmodel::removeAt(int i) {
    if (i < 0 || i >= _objectList.size()) {
        return nullptr;
    }

    QObject *removedObject = _objectList[i];

    removeRows(i, 1);

    return removedObject;
}

void GcsListmodel::insert(int i, QObject *object) {
    if (i < 0 || i > _objectList.count()) {
        return;
    }

    if (object) {
        QQmlEngine::setObjectOwnership(object, QQmlEngine::CppOwnership);
    }

    _objectList.insert(i, object);
    insertRows(i, 1);
}

void GcsListmodel::append(const std::map<uint32_t, QObject *> &objects) {
    int j = 0;
    if (objects.size() <= 0) {
        return;
    }

    for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
        QQmlEngine::setObjectOwnership(iter->second, QQmlEngine::CppOwnership);
        _objectList.insert(j, iter->second);
        j++;
    }

    insertRows(0, objects.size());
}

void GcsListmodel::insert(int i, const std::list<QObject *> &objects) {
    if (objects.size() <= 0) {
        return;
    }

    int j = i;
    for (int a = 0; a < objects.size(); a++) {
        QQmlEngine::setObjectOwnership(objects.front() + a, QQmlEngine::CppOwnership);
        _objectList.insert(j, objects.front() + a);
        j++;
    }

    insertRows(i, objects.size());
}

int GcsListmodel::append(QObject *object) {
    int cur = _objectList.count();
    insert(_objectList.count(), object);
    return cur;
}

void GcsListmodel::append(const std::list<QObject *> &objects) {
    insert(_objectList.size(), objects);
}

void GcsListmodel::replace(const std::list<QObject *> &objects) {
    int position = objects.size();
    int rows = count() - position;

    beginResetModel();

    if (rows > 0) {
        for (int row = position; row < position + rows; row++) {
            _objectList.removeAt(position);
        }
    }

    for (int a = 0; a < _objectList.count(); a++) {
        QQmlEngine::setObjectOwnership(objects.front() + a, QQmlEngine::CppOwnership);
        _objectList[a] = objects.front() + a;
    }

    for (int am = _objectList.count(); am < objects.size(); ++am) {
        QQmlEngine::setObjectOwnership(objects.front() + am, QQmlEngine::CppOwnership);
        _objectList.insert(am, objects.front() + am);
    }

    endResetModel();

    emit countChanged(_objectList.count());
}

int GcsListmodel::count() const {
    return _objectList.count();
}

void GcsListmodel::clearAll() {
    if (_objectList.size() > 0) {
        beginResetModel();
        _objectList.clear();
        endResetModel();

        emit countChanged(count());
    }
}

void GcsListmodel::beginReset() { beginResetModel(); }

void GcsListmodel::endReset() { endResetModel(); }
