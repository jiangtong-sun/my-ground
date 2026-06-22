#pragma once

#include <QAbstractItemModel>
#include <QObject>
#include <memory>
#include "VehicleMetadef.h"

class VEHICLE_EXPORT CommElemNode : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(CommElemNode)
  public:
    CommElemNode() = default;
    virtual ~CommElemNode() = default;
    virtual QVariant elemState() { return QVariant(); }
    virtual QVariant columnValue(int column) { return QVariant(); }
};

class VEHICLE_EXPORT CommTableModel : public QAbstractTableModel {
    Q_OBJECT
    VEH_PROPERTY_RW(QStringList, columnSource)
    Q_PROPERTY(int rowCount READ rowCount CONSTANT)
    Q_DISABLE_COPY(CommTableModel)

  public:
    enum TableModelRoles { ElemNode = 0x0101, ColumnName = 0x0102, ElemState = 0x0103};

    explicit CommTableModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    virtual void initColumnHeader() = 0;

    Q_INVOKABLE void clear();

    int removeRow(int rowIndex, int rows = 1);
    int insertRow(int rowIndex, std::shared_ptr<CommElemNode> row);
    int appendRow(std::shared_ptr<CommElemNode> row);

  protected:
    int updateRow(int rowIndex, CommElemNode *row);
    virtual QVariant elementData(int row, int col) const;

    QVariant elementState(int row) const;

    QList<std::shared_ptr<CommElemNode>> orginDatas;
    QList<CommElemNode *> _dataSource;
};
