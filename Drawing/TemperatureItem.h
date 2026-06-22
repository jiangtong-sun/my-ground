#pragma once

#include "DrawingDefine.h"
#include "VehicleElement.h"

#include <QQuickPaintedItem>

class QuickTemperature : public QQuickPaintedItem {
    Q_OBJECT
    DRAW_PROPERTY_AUTO(QColor, color)
    DRAW_PROPERTY_AUTO(QColor, warnColor)
    DRAW_PROPERTY_AUTO(QColor, setColor)
    DRAW_PROPERTY_AUTO(QColor, fontColor)
    Q_PROPERTY(SimpleElemNode *dataNode READ dataNode WRITE setData NOTIFY nodeChanged)
    QML_NAMED_ELEMENT(QuickEngineElem)

  public:
    QuickTemperature(QQuickItem *parent = nullptr);

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
