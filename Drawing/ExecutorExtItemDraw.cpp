#pragma once

#include "ExecutorExtItemDraw.h"
#include "chchar.h"

#include <QPainterPath>
#include <QQuickPaintedItem>
#include <math.h>
#include <qpainter.h>

QuickExecutorExtElem::QuickExecutorExtElem(QQuickItem *parent) : QQuickPaintedItem(parent) {
    _fillStyle = QColor::fromRgbF(255, 255, 255);
    _strokeColor = QColor::fromRgbF(255, 255, 255);
    connect(this, &QuickExecutorExtElem::fillStyleChanged, this, [=] { update(); });
    connect(this, &QuickExecutorExtElem::strokeColorChanged, this, [=] { update(); });
}

void QuickExecutorExtElem::updateModel(FlightVechicle *veh) { curr_veh = veh; }

void QuickExecutorExtElem::paint(QPainter *painter) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
    painter->setPen(QPen(_strokeColor, 2));
    QRectF rect = boundingRect();
    partWidth = rect.width();
    partHeight = rect.height() / 20;
    drawHeight = partHeight * 12;
    dWidth = partWidth / 21;
    splitWidth = dWidth * 3;
    painter->save();
    painter->restore();
}
