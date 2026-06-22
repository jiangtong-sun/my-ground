#pragma once

#include "BatteryRotateDraw.h"
#include <QPainterPath>
#include <QQuickPaintedItem>
#include <qpainter.h>

QuickBatteryRotate::QuickBatteryRotate(QQuickItem *parent)
    : QQuickPaintedItem(parent), _dataNode(nullptr) {
    _state = false;
    _threshold = 20;
    _electric = 80;
    _capacity = 100;
    _itemName = QString("#1");
    _warnColor = QColor(255, 155, 0);
    _color = QColor::fromRgbF(255, 255, 255);
    connect(this, &QuickBatteryRotate::colorChanged, this, [=] { update(); });
}

void QuickBatteryRotate::doUpdateQuick(QString val, int color) { update(); }

void QuickBatteryRotate::setData(SimpleElemNode *name) {
    if (name == nullptr) {
        return;
    }

    _dataNode = name;
    connect(&_dataNode->_default, &FlightDisplay::updateColorValue, this,
            &QuickBatteryRotate::doUpdateQuick);
}

void QuickBatteryRotate::paint(QPainter *painter) {
    painter->save();

    QPen pen;
    pen.setWidth(1);
    pen.setColor(_color);
    painter->setPen(pen);

    float eWidth = width() / 5;
    float eHeight = height() / 9;
    painter->drawLine(eWidth, eHeight * 2, eWidth * 4, eHeight * 2);
    painter->drawLine(eWidth * 4, eHeight * 2, eWidth * 4, eHeight * 8 + 2);
    painter->drawLine(eWidth * 4, eHeight * 8 + 2, eWidth, eHeight * 8 + 2);
    painter->drawLine(eWidth, eHeight * 8 + 2, eWidth, eHeight * 2);
    if (_capacity <= 1) {
        _capacity = 1;
    }

    float elecHeight = eHeight * 6 * _electric / _capacity;
    painter->fillRect(eWidth + 2, height() - elecHeight - eHeight, eWidth * 3 - 2, elecHeight,
                      _electric > _threshold ? _color : _warnColor);
    painter->fillRect(eWidth * 2, eHeight * 3 / 2, eWidth, eHeight / 2, _color);
    painter->restore();
}
