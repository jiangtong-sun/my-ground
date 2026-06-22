#pragma once

#include "DataLinkItemDraw.h"
#include "chchar.h"

#include <QPainterPath>
#include <QQuickPaintedItem>
#include <qpainter.h>

QuickDataLinkElem::QuickDataLinkElem(QQuickItem *parent)
    : QQuickPaintedItem(parent), _summary(nullptr) {
    _linkName = "U";
    _color = QColor(255, 255, 255);
    _redColor = QColor(255, 10, 10);
    _orangeColor = QColor(255, 120, 1);
    connect(this, &QuickDataLinkElem::colorChanged, this, [=] { update(); });
}

void QuickDataLinkElem::setData(DataLinkerSummary *name) {
    if (name == nullptr) {
        return;
    }

    _summary = name;

    connect(_summary, &DataLinkerSummary::workingChanged, this, [=] { update(); });
    connect(_summary, &DataLinkerSummary::summaryChanged, this, [=] { update(); });
}

void QuickDataLinkElem::doUpdateQuick() { update(); }

void QuickDataLinkElem::paint(QPainter *painter) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    QColor colr = _color;
    bool upState = false;
    bool downState = false;
    bool working = false;
    int quality = 1;

    if (_summary) {
        working = _summary->working();
        quality = _summary->quality();
        upState = _summary->upState();
        downState = _summary->downState();
        quality = qBound(1, quality, 10);
        colr = quality < 2 ? _redColor : quality < 8 ? _orangeColor : _color;
    }

    painter->setPen(QPen(colr));
    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
    QRectF rect = boundingRect();
    int width = rect.width();
    int height = rect.height();
    int gapRW = width / 17;
    int gapRH = height / 11;
    QRect txtRect(gapRW * 12, gapRH * 3, gapRW * 4, gapRH * 4);
    painter->drawText(txtRect, Qt::AlignCenter, _linkName);
    for (int i = 0; i < quality; i += 2) {
        QRect firstRect(gapRW * (i + 1), height - gapRH * (i + 2), gapRW, gapRH * (i + 1));
        painter->fillRect(firstRect, colr);
    }
    painter->restore();

    /// makes default data linker
    if (working) {
        painter->save();
        QPainterPath path1;
        path1.addEllipse(gapRW * 2, gapRW * 2, 10, 10);
        painter->setPen(QPen(_color, 1));
        painter->fillPath(path1, QBrush(_color));
        painter->restore();
    }
    painter->save();
    painter->setPen(QPen(upState ? _color : _redColor, 1));
    QPainterPath path2;
    path2.moveTo(gapRW * 12, gapRH * 1);
    path2.lineTo(gapRW * 11, gapRH * 3);
    path2.lineTo(gapRW * 13, gapRH * 3);
    path2.lineTo(gapRW * 12, gapRH * 1);
    painter->fillPath(path2, QBrush(upState && downState ? _color : _redColor));
    painter->drawPath(path2);
    painter->restore();

    painter->save();
    QPainterPath path3;
    painter->setPen(QPen(downState ? _color : _redColor, 1));
    path3.moveTo(gapRW * 12, gapRH * 10);
    path3.lineTo(gapRW * 11, gapRH * 8);
    path3.lineTo(gapRW * 13, gapRH * 8);
    path3.lineTo(gapRW * 12, gapRH * 10);
    painter->fillPath(path3, QBrush(downState ? _color : _redColor));
    painter->drawPath(path3);
    painter->restore();
}
