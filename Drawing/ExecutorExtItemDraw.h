#pragma once

#include "DrawingDefine.h"
#include "FlightAircraft.h"
#include <QQuickPaintedItem>

class QuickExecutorExtElem : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY(FlightVechicle *vehicle READ vehicle WRITE updateModel NOTIFY vehicleChanged)
    DRAW_PROPERTY_AUTO(QColor, strokeColor)
    DRAW_PROPERTY_AUTO(QColor, fillStyle)
    QML_NAMED_ELEMENT(QuickExecutorExtElem)

  public:
    FlightVechicle *vehicle() { return curr_veh; }
    Q_INVOKABLE void updateModel(FlightVechicle *);
    QuickExecutorExtElem(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;

  signals:
    void vehicleChanged();

  private:
    FlightVechicle *curr_veh = nullptr;

    float dWidth = 1;
    float partWidth = 1;
    float partHeight = 1;
    float drawHeight = 1;
    float splitWidth = 1;

    float lwheelRate = 0;
    float rwheelRate = 10;
    QString leftRudderVal = 0;
    QString rightRudderVal = 0;
    QString lwheelPressure = 0;
    QString rwheelPressure = 0;
    QString fwheelAngle = 0;
    QString lFlapPosition = 0;
    QString rFlapPosition = 0;
    QString lAileronPosition = 0;
    QString rAileronPosition = 0;
};
