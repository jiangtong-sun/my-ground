#include "gcsrectangle.h"

#include <QPainterPath>

GcsRectangle::GcsRectangle(QQuickItem *parent) : QQuickPaintedItem(parent) {
    color(QColor(25, 25, 25, 250));
    connect(this, &GcsRectangle::colorChanged, this, [=] { update(); });
}

void GcsRectangle::paint(QPainter *painter) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(_color));
    QRectF rect = boundingRect();
    QPolygon lefttop;
    lefttop << QPoint((rect.topLeft() + QPointF(0, 10)).toPoint());
    lefttop << QPoint((rect.topLeft() + QPointF(0, 0)).toPoint());
    lefttop << QPoint((rect.topLeft() + QPointF(10, 0)).toPoint());
    QPolygon leftbotm;
    leftbotm << QPoint((rect.bottomLeft() + QPointF(0, -10)).toPoint());
    leftbotm << QPoint((rect.bottomLeft() + QPointF(0, 0)).toPoint());
    leftbotm << QPoint((rect.bottomLeft() + QPointF(10, 0)).toPoint());
    QPolygon rightbotm;
    rightbotm << QPoint((rect.bottomRight() + QPointF(-10, 0)).toPoint());
    rightbotm << QPoint((rect.bottomRight() + QPointF(0, 0)).toPoint());
    rightbotm << QPoint((rect.bottomRight() + QPointF(0, 10)).toPoint());
    QPolygon righttop;
    righttop << QPoint((rect.topRight() + QPointF(0, 10)).toPoint());
    righttop << QPoint((rect.topRight() + QPointF(0, 0)).toPoint());
    righttop << QPoint((rect.topRight() + QPointF(-10, 0)).toPoint());
    painter->drawLines(lefttop);
    painter->drawLines(leftbotm);
    painter->drawLines(rightbotm);
    painter->drawLines(righttop);
    painter->restore();
}
