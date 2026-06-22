#pragma once

#include <QPainter>
#include <QQuickItem>
#include <QQuickPaintedItem>

#include "gcsstdafx.h"

class CONTROL_EXPORT GcsRectangle : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY_AUTO(QColor, color)
    QML_NAMED_ELEMENT(GcsRectangle)
  public:
    explicit GcsRectangle(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;
};
