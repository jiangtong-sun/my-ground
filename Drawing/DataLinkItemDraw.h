#pragma once

#include "DataLinkerSummary.h"
#include "DrawingDefine.h"

#include <QQuickPaintedItem>

class QuickDataLinkElem : public QQuickPaintedItem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(QColor, color)
    DRAW_PROPERTY_AUTO(QColor, redColor)
    DRAW_PROPERTY_AUTO(QColor, orangeColor)
    DRAW_PROPERTY_AUTO(QString, linkName)
    Q_PROPERTY(DataLinkerSummary *dataNode READ dataNode WRITE setData NOTIFY nodeChanged)
    QML_NAMED_ELEMENT(QuickDataLinkElem)

  public:
    QuickDataLinkElem(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;

    void setData(DataLinkerSummary *name);
    DataLinkerSummary *dataNode() { return _summary; }

  private slots:
    void doUpdateQuick();

  signals:
    void nodeChanged();

  protected:
    DataLinkerSummary *_summary;
};
