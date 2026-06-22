#pragma once

#include "DrawingDefine.h"
#include "VehicleElement.h"
#include <QQuickPaintedItem>

class QuickTemperatureElem : public QQuickPaintedItem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(QColor, color)
    DRAW_PROPERTY_AUTO(QColor, setColor)
    Q_PROPERTY(SimpleElemNode *dataNode READ dataNode WRITE setData NOTIFY nodeChanged)
    QML_NAMED_ELEMENT(QuickEngineElem)

  public:
    QuickTemperatureElem(QQuickItem *parent = nullptr);

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
