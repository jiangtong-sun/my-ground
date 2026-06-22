#pragma once

#include "TemperatureItem.h"
#include "chchar.h"
#include "log_category.h"

#include <QPainterPath>
#include <QQuickPaintedItem>
#include <qpainter.h>

QuickTemperature::QuickTemperature(QQuickItem *parent) : QQuickPaintedItem(parent), _dataNode(nullptr) {
    _color = QColor(0, 255, 0);
    _setColor = QColor(125, 0, 225);
    connect(this, &QuickTemperature::colorChanged, this, [=] { update(); });
    connect(this, &QuickTemperature::setColorChanged, this, [=] { update(); });
}

void QuickTemperature::setData(SimpleElemNode *name) {
    if (name == nullptr) {
        return;
    }

    _dataNode = name;
    const FlightDisplay *node = _dataNode->defNode();
    if (node != nullptr) {
        connect(node, &FlightDisplay::updateColorValue, this, &QuickTemperature::doUpdateQuick);
    } else {
        COMMON_WARN("invalid input param");
    }
}

void QuickTemperature::doUpdateQuick(QString val, int color) { update(); }

void QuickTemperature::paint(QPainter *painter) {
    const FlightDisplay *node = _dataNode->defNode();
    QColor colorSet = _color;
    QString temperature = 0;
    QString tempValue = 0;
    float rateValue = 0;

    if (node != nullptr) {
        colorSet = node->showColor() & 0xf ? _warnColor : _color;
        tempValue = node->showColor() > 0xf ? "--" : node->display() + node->unit();
        temperature = node->itemName();
        rateValue = node->rateValue();
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    QRectF rect = boundingRect();
    int width = rect.width();
    int height = rect.height();
    int radius = (width > height ? height : width) / 2 - 10;
    QPainterPath path2;
    QColor color = QColor(249, 108, 17);
    painter->setPen(QPen(color, 3));
    painter->translate(width / 2, height / 2);
    path2.moveTo(radius * std::cos(3.1415 / 6), radius * std::sin(3.1415 / 6));
    path2.arcTo(-radius, -radius, radius * 2, radius * 2, -30, 45);
    painter->drawPath(path2);
    QPainterPath path3;
    QColor color2 = QColor(169, 118, 1);
    painter->setPen(QPen(color2, 3));
    path3.moveTo(radius, 0);
    path3.arcTo(-radius, -radius, radius * 2, radius * 2, 0, 45);
    painter->drawPath(path3);
    QPainterPath path;
    painter->setPen(QPen(_color, 3));
    int xyoff = radius * std::sin(3.1415 / 4);
    path.moveTo(xyoff, -xyoff);
    path.arcTo(-radius, -radius, radius * 2, radius * 2, 45, 165);
    painter->drawPath(path);
    painter->restore();

    painter->save();
    QPainterPath path4;
    painter->setPen(QPen(colorSet, 2));
    float rateAngle = 3.1415 * 4 / 3;
    float curSpeed = rateValue * rateAngle;
    int xyoffs = radius * std::sin(rateAngle + 3.1415 / 3 - curSpeed);
    int xyoffc = radius * std::cos(rateAngle + 3.1415 / 3 - curSpeed);
    path4.moveTo(width / 2 - 1, height / 2 - 1);
    path4.lineTo(width / 2 + xyoffs, height / 2 + xyoffc);
    path4.lineTo(width / 2 + 1, height / 2 - 1);
    path4.lineTo(width / 2 - 1, height / 2 - 1);
    path4.addEllipse(width / 2 - 3, height / 2 - 3, 6, 6);
    painter->fillPath(path4, QBrush(colorSet));
    painter->drawPath(path4);
    painter->restore();

    painter->save();
    painter->setPen(QPen(_fontColor));
    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
    QRect txtRect(5, height - 30, width - 10, 30);
    painter->drawText(txtRect, Qt::AlignCenter, temperature);
    painter->restore();

    painter->save();
    painter->setPen(QPen(_fontColor));
    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
    QRect txtRect2(5, height - 50, width - 10, 30);
    painter->drawText(txtRect2, Qt::AlignCenter, tempValue);
    painter->restore();
}
