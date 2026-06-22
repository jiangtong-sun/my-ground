#include "grouprectangle.h"
#include <QPainterPath>
#include "textstyle.h"

GroupRectangle::GroupRectangle(QQuickItem* parent) : QQuickPaintedItem(parent)
{
    leftMargin(10);
    rightMargin(10);
    bottomMargin(10);
    topMargin(10);
    leftRightOff(0);
    textColor(QColor(255, 255, 255, 255));
    penColor(QColor(0x35, 0x8E, 0x90, 255));
    connect(this, &GroupRectangle::textColorChanged, this, [=] { update(); });
    connect(this, &GroupRectangle::penColorChanged, this, [=] { update(); });
}

void GroupRectangle::paint(QPainter* painter)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(_penColor));
    QPolygon elepolygon0;
    QRectF rect = boundingRect();
    int width = rect.width()/3;
    if (leftRightOff() != 0) {
        width = leftRightOff();
    }

    QRect firstRect2(width, -2, width, 26);
    painter->setFont(QFont(TextStyle::instance()->family(), 12, 1));
    painter->drawText(firstRect2, Qt::AlignCenter, _text);

    elepolygon0 << QPoint((rect.topLeft() + QPointF(width, _topMargin)).toPoint());
    elepolygon0 << QPoint((rect.topLeft() + QPointF(_leftMargin, _topMargin)).toPoint());
    elepolygon0 << QPoint((rect.topLeft() + QPointF(_leftMargin, _topMargin)).toPoint());
    elepolygon0 << QPoint((rect.bottomLeft() + QPointF(_leftMargin, -_bottomMargin)).toPoint());
    elepolygon0 << QPoint((rect.bottomLeft() + QPointF(_leftMargin, -_bottomMargin)).toPoint());
    elepolygon0 << QPoint((rect.bottomRight() + QPointF(-_rightMargin, -_bottomMargin)).toPoint());
    elepolygon0 << QPoint((rect.bottomRight() + QPointF(-_rightMargin, -_bottomMargin)).toPoint());
    elepolygon0 << QPoint((rect.topRight() + QPointF(-_rightMargin, _topMargin)).toPoint());
    elepolygon0 << QPoint((rect.topRight() + QPointF(-_rightMargin, _topMargin)).toPoint());
    elepolygon0 << QPoint((rect.topRight() + QPointF(-width, _topMargin)).toPoint());
    painter->drawLines(elepolygon0);
    painter->restore();
}
