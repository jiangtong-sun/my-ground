#pragma once

#include "ExecutorItemDraw.h"

#include <QPainterPath>
#include <QQuickPaintedItem>
#include <qpainter.h>

QuickExecutorElem::QuickExecutorElem(QQuickItem *parent) : QQuickPaintedItem(parent) {
    _color = QColor::fromRgbF(255, 255, 255);
    connect(this, &QuickExecutorElem::colorChanged, this, [=] { update(); });
}

void QuickExecutorElem::doUpdateQuick() { update(); }

void QuickExecutorElem::paint(QPainter *painter) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(_color));
    QRectF rect = boundingRect();
    int width = rect.width() / 12;
    int height = rect.height() / 20;
    int cWidth = rect.width() / 2;

    painter->save();
    QPainterPath path;
    path.moveTo(cWidth - width * 2 - height / 2, height * 11);
    path.cubicTo(cWidth - width * 2 - height / 2, height * 11, cWidth - 5 * width, height * 10 + 20,
                 cWidth - 5 * width, height * 10 + 15);
    path.cubicTo(cWidth - 5 * width, height * 10 + 15, cWidth - 5 * width - 30, height * 10 - 10,
                 cWidth - 5 * width, height * 10);
    path.cubicTo(cWidth - 5 * width, height * 10, cWidth - 5 * width, height * 10, cWidth - width,
                 height * 10);
    path.cubicTo(cWidth - width, height * 10, cWidth - width / 4 - 10, height * 6, cWidth,
                 height * 6);
    path.cubicTo(cWidth, height * 6, cWidth + width / 4 + 10, height * 6, cWidth + width,
                 height * 10);
    path.cubicTo(cWidth + width, height * 10, cWidth + 5 * width, height * 10, cWidth + 5 * width,
                 height * 10);
    path.cubicTo(cWidth + 5 * width, height * 10, cWidth + 5 * width + 30, height * 10 - 10,
                 cWidth + 5 * width, height * 10 + 15);
    path.cubicTo(cWidth + 5 * width, height * 10 + 15, cWidth + 5 * width, height * 10 + 20,
                 cWidth + width * 2 + height / 2, height * 11);
    path.cubicTo(cWidth + width * 2 + height / 2, height * 11, cWidth, height * 13,
                 cWidth - width * 2 - height / 2, height * 11);
    path.addRect(cWidth + width * 2 + height / 2, height * 11, 4, height * 2);
    path.addRect(cWidth - width * 2 - height / 2, height * 11, 4, height * 2);
    path.addEllipse(cWidth + width * 2, height * 13, height, height);
    path.addEllipse(cWidth - width * 2 - height, height * 13, height, height);
    path.addEllipse(cWidth - height / 2, height * 13 - height / 2, height, height);
    path.addRect(cWidth - 2, height * 13 - height, 4, height / 2);
    path.addRect(cWidth - 2, height * 10 - height / 2, 4, height);
    path.addRect(cWidth - width / 2, height * 10 + height / 2, width, 2);
    painter->drawPath(path);
    painter->restore();
}
