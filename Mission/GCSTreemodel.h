#pragma once

#include <QAbstractTableModel>
#include <QtQml/qqml.h>

#include "GcsMissionDef.h"
#include "gcstreenode.h"

class GcsTreeModel : public QAbstractTableModel {
    Q_OBJECT
    QPROPERTY_AUTO(QStringList, columnSource)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

  public:
    enum TreeModelRoles { RowModel = 0x0101, ColumnModel = 0x0102 };
    explicit GcsTreeModel(QObject *parent = nullptr);
    void append(GcsTreeNode *data);
    void remove(GcsTreeNode *data);
    GcsTreeNode *select(int id);

    int count() { return _dataSource.count(); }
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void removeRows(int row, int count);
    Q_INVOKABLE void insertRows(int row, const QList<QObject *> &data);
    Q_INVOKABLE QObject *getRow(int row);
    Q_INVOKABLE GcsTreeNode *getNode(int row);
    Q_INVOKABLE void updateData(int row);
    Q_INVOKABLE void collapse(int row);
    Q_INVOKABLE void expand(int row);
    Q_INVOKABLE void resetChecked();
    Q_INVOKABLE void allExpand();
    Q_INVOKABLE void allCollapse();

  signals:
    void countChanged();

  private:
    QList<QObject *> _dataSource;
};
