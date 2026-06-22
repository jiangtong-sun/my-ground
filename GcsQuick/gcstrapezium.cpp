#include "gcstrapezium.h"

#include <QPainterPath>

GcsTrapezium::GcsTrapezium(QQuickItem* parent) : QQuickPaintedItem(parent)
{
    color(QColor(255, 255, 255, 255));
    penColor(QColor(0x35, 0x8E, 0x90, 255));
    specailColor(QColor(0x35, 0x8E, 0x90, 255));

    connect(this, &GcsTrapezium::specailColorChanged, this, [=] { update(); });
    connect(this, &GcsTrapezium::penColorChanged, this, [=] { update(); });
    connect(this, &GcsTrapezium::colorChanged, this, [=] { update(); });
}

void GcsTrapezium::paint(QPainter* painter)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(_penColor));

    QPainterPath path;
    QRectF rect = boundingRect();

    QPolygon elepolygon0;
    QPointF brPoint = rect.bottomRight();
    QPointF blPoint = rect.bottomLeft();

    blPoint += QPointF(10, 0);
    brPoint -= QPointF(10, 0);

    elepolygon0 << QPoint(rect.topLeft().toPoint());
    elepolygon0 << QPoint(blPoint.toPoint());
    elepolygon0 << QPoint(brPoint.toPoint());
    elepolygon0 << QPoint(rect.topRight().toPoint());
    elepolygon0 << QPoint(rect.topLeft().toPoint());

    path.moveTo(rect.topLeft().toPoint());
    path.lineTo(blPoint.toPoint());
    path.lineTo(brPoint.toPoint());
    path.lineTo(rect.topRight().toPoint());
    path.lineTo(rect.topLeft().toPoint());

    painter->drawPolygon(elepolygon0);
    painter->fillPath(path, _specailColor);
    painter->restore();
}
