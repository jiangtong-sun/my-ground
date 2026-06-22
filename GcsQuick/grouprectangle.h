#pragma once

#include <QPainter>
#include <QQuickItem>
#include <QQuickPaintedItem>

#include "gcsstdafx.h"

class CONTROL_EXPORT GroupRectangle : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY_AUTO(QColor, textColor)
    Q_PROPERTY_AUTO(QColor, penColor)
    Q_PROPERTY_AUTO(int, leftRightOff)
    Q_PROPERTY_AUTO(int, leftMargin)
    Q_PROPERTY_AUTO(int, rightMargin)
    Q_PROPERTY_AUTO(int, bottomMargin)
    Q_PROPERTY_AUTO(int, topMargin)
    Q_PROPERTY_AUTO(QString, text)
    QML_NAMED_ELEMENT(GroupRectangle)
  public:
    explicit GroupRectangle(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;
};
