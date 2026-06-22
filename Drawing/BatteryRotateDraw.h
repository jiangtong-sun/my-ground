#pragma once

#include "DrawingDefine.h"
#include "VehicleElement.h"

#include <QQuickPaintedItem>

class QuickBatteryRotate : public QQuickPaintedItem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(bool, state)
    DRAW_PROPERTY_AUTO(QColor, color)
    DRAW_PROPERTY_AUTO(QColor, warnColor)
    DRAW_PROPERTY_AUTO(float, electric)
    DRAW_PROPERTY_AUTO(float, threshold)
    DRAW_PROPERTY_AUTO(float, capacity)
    DRAW_PROPERTY_AUTO(QString, itemName)
    QML_NAMED_ELEMENT(QuickBatteryRotate)

  public:
    QuickBatteryRotate(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;

    void setData(SimpleElemNode *name);

    SimpleElemNode *dataNode() { return _dataNode; }

  private slots:
    void doUpdateQuick(QString val, int color);

  signals:
    void nodeChanged();

  protected:
    SimpleElemNode *_dataNode;
};
