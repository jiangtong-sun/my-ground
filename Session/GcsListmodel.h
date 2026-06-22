#pragma once

#include <QtCore/QAbstractListModel>

class GcsListmodel : public QAbstractListModel {
    Q_OBJECT
    Q_DISABLE_COPY(GcsListmodel)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

  public:
    GcsListmodel(QObject *parent = nullptr);
    ~GcsListmodel() override;

    int count() const;
    int append(QObject *object);
    void append(const std::list<QObject *> &objects);
    void replace(const std::list<QObject *> &objects);
    void append(const std::map<uint32_t, QObject *> &objects);

    Q_INVOKABLE void clear();
    Q_INVOKABLE void clearAll();
    Q_INVOKABLE void move(int from, int to);
    Q_INVOKABLE QObject *get(int index);

    QObject *removeAt(int i);
    QObject *removeOne(QObject *object) { return removeAt(indexOf(object)); }

    void insert(int i, QObject *object);
    void insert(int i, const std::list<QObject *> &objects);

    bool contains(QObject *object) { return _objectList.indexOf(object) != -1; }
    int indexOf(QObject *object) { return _objectList.indexOf(object); }

    QObject *operator[](int i);
    const QObject *operator[](int i) const;

    template <class T>
    T value(int index) {
        return qobject_cast<T>(_objectList[index]);
    }

    const QList<QObject *> &objectList() const { return _objectList; }

    void beginReset();
    void endReset();

  signals:
    void countChanged(int count);

  private:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames(void) const override;

  private:
    QList<QObject *> _objectList;
    static const int ObjectRole;
    static const int TextRole;
};
