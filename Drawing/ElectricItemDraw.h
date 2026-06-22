#pragma once

#include "DrawingDefine.h"
#include "VehicleElement.h"

#include <QQuickPaintedItem>

class QuickElectricElem : public QQuickPaintedItem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(QColor, color)
    DRAW_PROPERTY_AUTO(QColor, fontColor)
    DRAW_PROPERTY_AUTO(QColor, warnColor)
    DRAW_PROPERTY_AUTO(QColor, whiteColor)
    DRAW_PROPERTY_AUTO(QColor, greenColor)
    Q_PROPERTY(SimpleElemList *battery READ battery WRITE setBattery NOTIFY batteryChanged)
    Q_PROPERTY(SimpleElemList *elecNode READ elecNode WRITE setElecNode NOTIFY elecNodeChanged)
    QML_NAMED_ELEMENT(QuickElectricElem)

  public:
    QuickElectricElem(QQuickItem *parent = nullptr);
    virtual void paint(QPainter *painter) override;

    void setElecNode(SimpleElemList *name);
    SimpleElemList *elecNode() { return _emic_node; }

    void setBattery(SimpleElemList *name);
    SimpleElemList *battery() { return _battery; }

    QColor getColor(FlightDisplay *);

  private slots:
    void doUpdateQuick(QString val, int color);

  signals:
    void warnGroupChanged();
    void elecNodeChanged();
    void batteryChanged();

  protected:
    SimpleElemList *_emic_node;
    SimpleElemList *_battery;
};

class QuickElectricElem2 : public QuickElectricElem {
    Q_OBJECT
    Q_PROPERTY(SimpleElemList *generator READ generator WRITE setGenerator NOTIFY generatorChanged)
    QML_NAMED_ELEMENT(QuickElectricElem2)

  public:
    QuickElectricElem2(QQuickItem *parent = nullptr);

    virtual void paint(QPainter *painter) override;

    void drawLine(QPainter *painter, BatteryElemNode *item, QVector<QPoint> &points, int direct,
                  int diode);

    void drawItem(QPainter *painter, BatteryElemNode *item, int left, int top, int width,
                  int height, bool state);

    void setGenerator(SimpleElemList *name);

    SimpleElemList *generator() { return _generator; }

  private slots:
    void doUpdateQuick2(QString val, int color);

  signals:
    void generatorChanged();

  protected:
    SimpleElemList *_generator;
};

class QuickElectricElem3 : public QuickElectricElem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(QColor, lineColor)
    Q_PROPERTY(SimpleElemList *generator READ generator WRITE setGenerator NOTIFY generatorChanged)
    QML_NAMED_ELEMENT(QuickElectricElem3)

  public:
    QuickElectricElem3(QQuickItem *parent = nullptr);

    virtual void paint(QPainter *painter) override;

    void drawItem3(QPainter *painter, BatteryElemNode *item, int left, int top, int width,
                   int height, int ltop, int lbotm, int hstep);

    void drawItem2(QPainter *painter, ElecSpdCtrlElemNode *item, int left, int top, int width,
                   int height, int ltop, int lbotm);

    void drawItem(QPainter *painter, BatteryElemNode *item, int left, int top, int width,
                  int height, int botm);

    void drawItem(QPainter *painter, BatteryElemNode *item, int left, int top, int width,
                  int height);

    void setGenerator(SimpleElemList *name);

    SimpleElemList *generator() { return _generator; }

  private slots:
    void doUpdateQuick3(QString val, int color);

  signals:
    void generatorChanged();

  protected:
    SimpleElemList *_generator;
};
