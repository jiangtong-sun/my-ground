#pragma once

#include <QtCore/QAbstractListModel>

class QmlObjectListModel : public QAbstractListModel
{
    Q_OBJECT
    
public:
    QmlObjectListModel(QObject* parent = nullptr);
    ~QmlObjectListModel() override;
    
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_INVOKABLE QObject* get(int index);

    int         count               () const;
    void        append              (QObject* object);
    void        append              (QList<QObject*>& objects);
    void        replace             (QList<QObject*>& objects);
    void        append              (QHash<uint32_t, QObject*>& objects);
    QObjectList swapObjectList    (const QObjectList& newlist);

    Q_INVOKABLE void  clear         ();
    Q_INVOKABLE void  clearAll      ();

    QObject*    removeAt            (int i);
    QObject*    removeOne           (QObject* object) { return removeAt(indexOf(object)); }
    void        insert              (int i, QObject* object);
    void        insert              (int i, QList<QObject*>& objects);
    bool        contains            (QObject* object) { return _objectList.indexOf(object) != -1; }
    int         indexOf             (QObject* object) { return _objectList.indexOf(object); }

    Q_INVOKABLE void move(int from, int to);
    Q_INVOKABLE void domove(int from, int to);

    QObject*    operator[]          (int i);
    const QObject* operator[]       (int i) const;
    template<class T> T value       (int index) { return qobject_cast<T>(_objectList[index]); }
    QList<QObject*>* objectList     () { return &_objectList; }

    void beginReset                 ();
    void endReset                   ();

signals:
    void countChanged               (int count);

private:
    int         rowCount    (const QModelIndex & parent = QModelIndex()) const override;
    QVariant    data        (const QModelIndex & index, int role = Qt::DisplayRole) const override;
    bool        insertRows  (int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool        removeRows  (int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool        setData     (const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames(void) const override;

private:
    QList<QObject*> _objectList;
    static const int ObjectRole;
    static const int TextRole;
};
