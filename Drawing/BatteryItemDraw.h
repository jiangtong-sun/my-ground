#pragma once

#include "DrawingDefine.h"
#include "VehicleElement.h"

#include <QQuickPaintedItem>

class QuickBatteryElem : public QQuickPaintedItem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(bool, state)
    DRAW_PROPERTY_AUTO(QColor, color)
    DRAW_PROPERTY_AUTO(QColor, fontColor)
    DRAW_PROPERTY_AUTO(QColor, warnColor)
    DRAW_PROPERTY_AUTO(float, threshold)
    DRAW_PROPERTY_AUTO(QString, itemName)
    Q_PROPERTY(SimpleElemNode *dataNode READ dataNode WRITE setData NOTIFY nodeChanged)
    QML_NAMED_ELEMENT(QuickBatteryElem)

  public:
    QuickBatteryElem(QQuickItem *parent = nullptr);
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

class QuickCircleElem : public QQuickPaintedItem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(QColor, color)
    DRAW_PROPERTY_AUTO(QColor, fontColor)
    DRAW_PROPERTY_AUTO(QColor, warnColor)
    DRAW_PROPERTY_AUTO(QString, itemName)
    Q_PROPERTY(FlightDisplay *dataNode READ dataNode WRITE setData NOTIFY nodeChanged)
    QML_NAMED_ELEMENT(QuickBatteryElem)

  public:
    QuickCircleElem(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;
    void setData(FlightDisplay *name);
    void drawBackground(QPainter *painter);

    FlightDisplay *dataNode() { return _dataNode; }

  private slots:
    void doUpdateQuick(QString val, int color);

  signals:
    void nodeChanged();

  protected:
    FlightDisplay *_dataNode;
};
