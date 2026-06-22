#include "QmlObjectListModel.h"

#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>

const int QmlObjectListModel::ObjectRole = Qt::UserRole;
const int QmlObjectListModel::TextRole = Qt::UserRole + 1;

QmlObjectListModel::QmlObjectListModel(QObject* parent)
    : QAbstractListModel(parent)
{

}

QmlObjectListModel::~QmlObjectListModel()
{
    
}

QObject* QmlObjectListModel::get(int index)
{
    if (index < 0 || index >= _objectList.count()) {
        return nullptr;
    }

    return _objectList[index];
}

int QmlObjectListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    
    return _objectList.count();
}

QVariant QmlObjectListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    
    if (index.row() < 0 || index.row() >= _objectList.count()) {
        return QVariant();
    }
    
    if (role == ObjectRole) {
        return QVariant::fromValue(_objectList[index.row()]);
    }
    else if (role == TextRole) {
        return QVariant::fromValue(_objectList[index.row()]->objectName());
    }
    else {
        return QVariant();
    }
}

QHash<int, QByteArray> QmlObjectListModel::roleNames(void) const
{
    QHash<int, QByteArray> hash;  
    hash[ObjectRole] = "object";
    hash[TextRole] = "text";
    return hash;
}

bool QmlObjectListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() && role == ObjectRole) {
        _objectList.replace(index.row(), value.value<QObject*>());
        emit dataChanged(index, index);
        return true;
    }
    
    return false;
}

bool QmlObjectListModel::insertRows(int position, int rows, const QModelIndex& parent)
{
    Q_UNUSED(parent);
    
    if (position < 0 || position > _objectList.count() + 1) {
        return true;
    }
    
    beginInsertRows(QModelIndex(), position, position + rows - 1);
    endInsertRows();

    emit countChanged(count());

    return true;
}

bool QmlObjectListModel::removeRows(int position, int rows, const QModelIndex& parent)
{
    Q_UNUSED(parent);
    
    if (position < 0 || position >= _objectList.count()) {
        return true;
    } else if (position + rows > _objectList.count()) {
        return true;
    }
    
    beginRemoveRows(QModelIndex(), position, position + rows - 1);
    for (int row=0; row<rows; row++) {
        _objectList.removeAt(position);
    }
    endRemoveRows();
    
    emit countChanged(count());
    
    return true;
}

void QmlObjectListModel::domove(int from, int to)
{
    if (from != to && from >= 0 && from < count() && to >= 0 && to < count()){
        if(from == to - 1) {
            to = from++;
        }

        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        _objectList.move(from, to);
        endMoveRows();
    }
}

void QmlObjectListModel::move(int from, int to)
{
    if(0 <= from && from < count() && 0 <= to && to < count() && from != to) {
        if(from == to - 1) {
            to = from++;
        }
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        _objectList.move(from, to);
        endMoveRows();
    }
}

QObject* QmlObjectListModel::operator[](int index)
{
    if (index < 0 || index >= _objectList.count()) {
        return nullptr;
    }
    return _objectList[index];
}

const QObject* QmlObjectListModel::operator[](int index) const
{
    if (index < 0 || index >= _objectList.count()) {
        return nullptr;
    }
    return _objectList[index];
}

void QmlObjectListModel::clear()
{
    beginResetModel();
    _objectList.clear();
    endResetModel();
    emit countChanged(count());
}

QObject* QmlObjectListModel::removeAt(int i)
{
    QObject* removedObject = _objectList[i];
    removeRows(i, 1);
    return removedObject;
}

void QmlObjectListModel::insert(int i, QObject* object)
{
    if (i < 0 || i > _objectList.count()) {
        return;
    }

    if(object) {
        QQmlEngine::setObjectOwnership(object, QQmlEngine::CppOwnership);
    }

    _objectList.insert(i, object);
    insertRows(i, 1);
}

void QmlObjectListModel::append(QHash<uint32_t, QObject*>& objects){
    int j = 0;
    if (objects.count() <= 0) {
        return;
    }

    QHash<uint32_t, QObject * >::Iterator iter;
    for (iter = objects.begin(); iter != objects.end(); ++iter) {
        QQmlEngine::setObjectOwnership(iter.value(), QQmlEngine::CppOwnership);
        _objectList.insert(j, iter.value());
        j++;
    }
    insertRows(0, objects.count());
}

void QmlObjectListModel::insert(int i, QList<QObject*>& objects)
{
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

void QmlObjectListModel::append(QObject* object)
{
    insert(_objectList.count(), object);
}

void QmlObjectListModel::append(QList<QObject*>& objects)
{
    insert(_objectList.count(), objects);
}

void QmlObjectListModel::replace(QList<QObject*>& objects){

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

QObjectList QmlObjectListModel::swapObjectList(const QObjectList& newlist)
{
    QObjectList oldlist(_objectList);
    beginResetModel();
    _objectList = newlist;
    endResetModel();
    emit countChanged(count());
    return oldlist;
}

int QmlObjectListModel::count() const
{
    return rowCount();
}

void QmlObjectListModel::clearAll()
{
    beginResetModel();
    _objectList.clear();
    endResetModel();
    emit countChanged(count());
}

void QmlObjectListModel::beginReset()
{
    beginResetModel();
}

void QmlObjectListModel::endReset()
{
    endResetModel();
}
