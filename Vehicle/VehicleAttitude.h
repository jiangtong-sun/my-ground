#pragma once

#include "CommTableModel.h"
#include "factdecodermgr.h"
#include "xlsxchart.h"
#include "xlsxworkbook.h"

class VEHICLE_EXPORT DisplayElemNode : public CommElemNode {
    Q_OBJECT
    Q_DISABLE_COPY(DisplayElemNode)

  public:
    DisplayElemNode() = default;
    void setGroupName(const QString &);
    bool attributeMetaData(const FactMetaData *data);
    bool referenceMetaData(const FactMetaData *data);
    QVariant columnValue(int column);

  private slots:
    void doUpdateQuick(QString name, int color);
    void doReferenceSource(const QString &name, int color, int raw);

  private:
    QString groupName = "";
    float cacheDatas[3] = {0};
    const FactMetaData *attributeData = nullptr;
    FactMetaData *referenceData = nullptr;
};

class VEHICLE_EXPORT AttributeTable : public CommTableModel {
    Q_OBJECT
  public:
    AttributeTable() = default;
    ~AttributeTable() = default;
    void initColumnHeader() override;
    void setNavigationMode(int val);
    QVariant elementData(int row, int col) const override;
    void ReadModule(const QXlsx::Worksheet *sheet, QMap<QString, int> &columns,
                    FactDecoderMgr *decoder, int frow, int lrow);
  private slots:
    void doUpdateTable(const QString &name, int color, int chl);

  protected:
    int curr_nav_mode = -1;
    std::shared_ptr<CommElemNode> find(int row);
};
