#pragma once

#include "DrawingDefine.h"
#include "FlightAircraft.h"
#include <QQuickPaintedItem>

class QuickAttitudeElem : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY(FlightVechicle *vehicle READ vehicle WRITE updateModel NOTIFY vehicleChanged)
    DRAW_PROPERTY_AUTO(QColor, strokeColor)
    DRAW_PROPERTY_AUTO(QColor, fillStyle)
    QML_NAMED_ELEMENT(QuickAttitudeElem)

  public:
    FlightVechicle *vehicle() { return curr_veh; }
    Q_INVOKABLE void updateModel(FlightVechicle *);
    QuickAttitudeElem(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;

  protected:
    void drawRollScale(QPainter *painter);
    void drawAirspeedScale(QPainter *painter);
    void drawAltitudeScale(QPainter *painter);
    void drawPitchAngleScale(QPainter *painter);
    void drawSlideOffsetScale(QPainter *painter);
    void pitchAngleDraw(QPainter *painter, int pitch, int lower, int upper, float cpart);

  signals:
    void vehicleChanged();

  private:
    FlightVechicle *curr_veh = nullptr;
    float dWidth = 1;
    float partWidth = 1;
    float partHeight = 1;
    float drawHeight = 1;
    float splitWidth = 1;
    float veh_speed = 0;
    float veh_roll = 10;
    float veh_height = 0;
    float veh_pitch = 0;
    float veh_slide = 0;
    float veh_offset = 0;
};
