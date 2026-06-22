#ifndef GCSRECT_TRAPEZIUM_H
#define GCSRECT_TRAPEZIUM_H

#include <QPainter>
#include <QQuickItem>
#include <QQuickPaintedItem>

#include "gcsstdafx.h"

class CONTROL_EXPORT GcsTrapezium : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY_AUTO(QColor, color)
    Q_PROPERTY_AUTO(QColor, penColor)
    Q_PROPERTY_AUTO(QColor, specailColor)
    QML_NAMED_ELEMENT(GcsTrapezium)

  public:
    explicit GcsTrapezium(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;
};

#endif // GCSRECT_TRAPEZIUM_H
