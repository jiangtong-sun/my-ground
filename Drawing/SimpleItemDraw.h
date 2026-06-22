#pragma once

#include "DrawingDefine.h"
#include "VehicleElement.h"

#include <QQuickPaintedItem>

class QuickSimpleElemNode : public QQuickPaintedItem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(QColor, color)
    DRAW_PROPERTY_AUTO(QColor, warnColor)
    DRAW_PROPERTY_AUTO(QColor, borderColor)
    Q_PROPERTY(SimpleElemNode *dataNode READ dataNode WRITE setData NOTIFY nodeChanged)
    QML_NAMED_ELEMENT(QuickSimpleElemNode)

  public:
    QuickSimpleElemNode(QQuickItem *parent = nullptr);

    void setData(SimpleElemNode *name);

    SimpleElemNode *dataNode() { return _dataNode; }

    void paint(QPainter *painter) override;

  private slots:
    void doUpdateQuick();

  signals:
    void nodeChanged();

  protected:
    SimpleElemNode *_dataNode;
};

class QuickBusbarElemNode : public QQuickPaintedItem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(QColor, color)
    DRAW_PROPERTY_AUTO(QColor, warnColor)
    DRAW_PROPERTY_AUTO(QColor, fontColor)
    DRAW_PROPERTY_AUTO(QColor, whiteColor)
    Q_PROPERTY(SimpleElemNode *dataNode READ dataNode WRITE setData NOTIFY nodeChanged)
    QML_NAMED_ELEMENT(QuickBusbarElemNode)

  public:
    QuickBusbarElemNode(QQuickItem *parent = nullptr);

    void setData(SimpleElemNode *name);

    SimpleElemNode *dataNode() { return _dataNode; }

    void paint(QPainter *painter) override;

    virtual void printMessage(QString val, int color);

    QColor getColor(int color);

  private slots:
    void doUpdateQuick(QString val, int color);

  signals:
    void nodeChanged();

  protected:
    SimpleElemNode *_dataNode;
};

class QuickHBusbarElemNode : public QuickBusbarElemNode {
    Q_OBJECT
    QML_NAMED_ELEMENT(QuickHBusbarElemNode)

  public:
    QuickHBusbarElemNode(QQuickItem *parent = nullptr);
    void printMessage(QString val, int color) override;
    void paint(QPainter *painter) override;
};

class QuickBatteryElemNode : public QQuickPaintedItem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(QColor, color)
    DRAW_PROPERTY_AUTO(QColor, warnColor)
    DRAW_PROPERTY_AUTO(QColor, fontColor)
    Q_PROPERTY(SimpleElemNode *dataNode READ dataNode WRITE setData NOTIFY nodeChanged)
    QML_NAMED_ELEMENT(QuickBatteryElemNode)

  public:
    QuickBatteryElemNode(QQuickItem *parent = nullptr);

    void setData(SimpleElemNode *name);

    SimpleElemNode *dataNode() { return _dataNode; }

    void paint(QPainter *painter) override;

  private slots:
    void doUpdateQuick();

  signals:
    void nodeChanged();

  protected:
    SimpleElemNode *_dataNode;
};

class QuickEngineElemNode : public QQuickPaintedItem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(QColor, color)
    DRAW_PROPERTY_AUTO(QColor, setColor)
    DRAW_PROPERTY_AUTO(QColor, redColor)
    DRAW_PROPERTY_AUTO(QColor, warnColor)
    DRAW_PROPERTY_AUTO(QColor, greenColor)
    DRAW_PROPERTY_AUTO(QColor, fontColor)

    Q_PROPERTY(SimpleElemNode *dataNode READ dataNode WRITE setData NOTIFY nodeChanged)
    QML_NAMED_ELEMENT(QuickEngineElemNode)

  public:
    QuickEngineElemNode(QQuickItem *parent = nullptr);

    void setData(SimpleElemNode *name);

    SimpleElemNode *dataNode() { return _dataNode; }

    virtual void paint(QPainter *painter) override;

    void drawEnine(QPainter *painter);

  private slots:
    void doUpdateQuick(QString val, int color);

  signals:
    void nodeChanged();

  protected:
    SimpleElemNode *_dataNode = nullptr;
};

class QuickEngineElemNode2 : public QuickEngineElemNode {
    Q_OBJECT
    QML_NAMED_ELEMENT(QuickEngineElemNode2)
  public:
    void drawElem(QPainter *painter, FlightDisplay *diplay, int left, int top, int width,
                  int height);
    virtual void paint(QPainter *painter) override;
};

class QuickEngineElemNode3 : public QuickEngineElemNode {
    Q_OBJECT
    QML_NAMED_ELEMENT(QuickEngineElemNode2)
public:
    void drawElem(QPainter *painter, FlightDisplay *diplay, int left, int top, int width,
                  int height);
    virtual void paint(QPainter *painter) override;
};
