#pragma once

#include "DrawingDefine.h"
#include "FlightAircraft.h"

#include <QQuickPaintedItem>

class QuickActuatorElem : public QQuickPaintedItem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(QColor, color)
    Q_PROPERTY(FlightVechicle *dataNode READ dataNode WRITE setData NOTIFY nodeChanged)
    QML_NAMED_ELEMENT(QuickActuatorElem)

  public:
    QuickActuatorElem(QQuickItem *parent = nullptr);

    void paint(QPainter *painter) override;

    void setData(FlightVechicle *name);

    FlightVechicle *dataNode() { return _dataNode; }

  private slots:
    void doUpdateQuick();

  signals:
    void nodeChanged();

  protected:
    FlightVechicle *_dataNode;
};
