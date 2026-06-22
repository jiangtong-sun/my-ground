#pragma once

#include "BatteryItemDraw.h"
#include "chchar.h"

#include <QPainterPath>
#include <QQuickPaintedItem>
#include <qpainter.h>

QuickBatteryElem::QuickBatteryElem(QQuickItem *parent)
    : QQuickPaintedItem(parent), _dataNode(nullptr) {
    _state = false;
    _threshold = 20;
    _itemName = QString("#1");
    _warnColor = QColor(255, 155, 0);
    _color = QColor::fromRgbF(255, 255, 255);
    _fontColor = QColor::fromRgbF(255, 255, 255);
    connect(this, &QuickBatteryElem::colorChanged, this, [=] { update(); });
}

void QuickBatteryElem::doUpdateQuick(QString val, int color) { update(); }

void QuickBatteryElem::setData(SimpleElemNode *name) {
    if (name == nullptr) {
        return;
    }

    _dataNode = name;
    connect(&_dataNode->_default, &FlightDisplay::updateColorValue, this,
            &QuickBatteryElem::doUpdateQuick);
}

void QuickBatteryElem::paint(QPainter *painter) {
    QPen pen;
    pen.setWidth(1);
    float eWidth = width() / 8;
    float eHeight = height() / 9;
    float elecHeight = 0;
    float rateValue = 0;

    if (_dataNode != nullptr) {
        rateValue = _dataNode->defRateValue();
        elecHeight = eHeight * 6 * rateValue;
    }
    pen.setColor(rateValue * 100 > _threshold ? _color : _warnColor);
    painter->setPen(pen);
    painter->save();
    painter->drawLine(eWidth, eHeight * 2, eWidth * 4, eHeight * 2);
    painter->drawLine(eWidth * 4, eHeight * 2, eWidth * 4, eHeight * 8 + 2);
    painter->drawLine(eWidth * 4, eHeight * 8 + 2, eWidth, eHeight * 8 + 2);
    painter->drawLine(eWidth, eHeight * 8 + 2, eWidth, eHeight * 2);
    painter->fillRect(eWidth + 2, height() - elecHeight - eHeight, eWidth * 3 - 2, elecHeight,
                      rateValue * 100 > _threshold ? _color : _warnColor);
    painter->fillRect(eWidth * 2, eHeight * 3 / 2, eWidth, eHeight / 2, _color);

    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
    painter->setPen(QPen(rateValue * 100 > _threshold ? _fontColor : _warnColor));
    QRect txtRect(eWidth, eHeight * 4, eWidth * 3, eHeight * 4);
    QString rate = QString::number(rateValue * 100);
    QString rateStr = QString("%1%").arg(rate);
    painter->drawText(txtRect, Qt::AlignCenter, rateStr);
    txtRect = QRect(eWidth * 5, eHeight, eWidth * 2, eHeight * 8);
    QString nameStr = QString("%1").arg(_itemName);
    painter->drawText(txtRect, Qt::AlignCenter, nameStr);
    painter->restore();
}

QuickCircleElem::QuickCircleElem(QQuickItem *parent) : QQuickPaintedItem(parent) {
    _dataNode = nullptr;
    _itemName = QString("#1");
    _warnColor = QColor(255, 155, 0);
    _color = QColor(155, 155, 155);
    _fontColor = QColor(255, 255, 255);
    connect(this, &QuickCircleElem::colorChanged, this, [=] { update(); });
}

void QuickCircleElem::doUpdateQuick(QString val, int color) { update(); }

void QuickCircleElem::setData(FlightDisplay *name) {
    if (name == nullptr) {
        return;
    }

    _dataNode = name;
    connect(_dataNode, &FlightDisplay::updateColorValue, this, &QuickCircleElem::doUpdateQuick);
}

void QuickCircleElem::drawBackground(QPainter *painter) {
    QRectF rect = boundingRect();
    int width = rect.width();
    int height = rect.height();
    int radius = (width > height ? height : width) / 2 * 0.8;

    QPainterPath path2;
    painter->setPen(QPen(_color, 3));
    painter->translate(width / 2, height / 2 - 5);
    path2.moveTo(0, radius);
    path2.arcTo(-radius, -radius, radius * 2, radius * 2, -90, -360);
    painter->drawPath(path2);

    QPainterPath path;
    painter->setPen(QPen(_warnColor, 3));
    path.moveTo(0, radius);
    path.arcTo(-radius, -radius, radius * 2, radius * 2, -90, -270);
    painter->drawPath(path);

    QFont font = painter->font();
    font.setPointSize(12);
    painter->setFont(font);
    painter->setPen(Qt::white);
    painter->drawText(-radius * 3 / 4, -15, radius + radius / 2, 30, Qt::AlignCenter, "100.2\nV");
}

void QuickCircleElem::paint(QPainter *painter) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    drawBackground(painter);
    painter->restore();
}
