#pragma once

#include "EngineItemDraw.h"
#include "chchar.h"

#include <QPainterPath>
#include <QQuickPaintedItem>
#include <qpainter.h>

QuickEngineElem::QuickEngineElem(QQuickItem *parent) : QQuickPaintedItem(parent), _dataNode(nullptr) {
    _color = QColor(0, 255, 0);
    _fontColor = QColor(125, 0, 225);
    connect(this, &QuickEngineElem::fontColorChanged, this, [=] { update(); });
}

void QuickEngineElem::setData(SimpleElemNode *name) {
    if (name == nullptr) {
        return;
    }

    _dataNode = name;
    EngineElemNode *node = dynamic_cast<EngineElemNode *>(_dataNode);
    if (node != nullptr) {
        connect(node->inletTotalTmp(), &FlightDisplay::updateColorValue, this, &QuickEngineElem::doUpdateQuick);
        connect(node->oilTmp(), &FlightDisplay::updateColorValue, this, &QuickEngineElem::doUpdateQuick);
        connect(node->oilPressure(), &FlightDisplay::updateColorValue, this, &QuickEngineElem::doUpdateQuick);
        connect(node->exhuastTmp(), &FlightDisplay::updateColorValue, this, &QuickEngineElem::doUpdateQuick);
    }
}

void QuickEngineElem::doUpdateQuick(QString val, int color) { update(); }

void QuickEngineElem::paint(QPainter *painter) {
    EngineElemNode *node = dynamic_cast<EngineElemNode *>(_dataNode);
    if (node == nullptr) {
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE - 2, 1));
    QRectF rect = boundingRect();
    int width = rect.width();
    int height = rect.height() / 2;
    int stepWidth = width / 4 - 4;
    int stepWidth2 = stepWidth + 2;
    int stepWidth3 = stepWidth + 6;
    int leftWidth = 2;
    int rightPos = leftWidth + 2 * stepWidth + stepWidth2;

    painter->save();
    painter->setPen(QPen(_color));
    QRect txtRect1(leftWidth, height / 8, stepWidth, height * 4 / 5);
    painter->drawText(txtRect1, Qt::AlignCenter, node->inletTotalTmp()->vert2Name());
    txtRect1 = QRect(leftWidth + stepWidth + stepWidth2, height / 8, stepWidth, height * 4 / 5);
    painter->drawText(txtRect1, Qt::AlignCenter, node->exhuastTmp()->vert2Name());
    txtRect1 = QRect(leftWidth, height / 8 + height, stepWidth, height * 4 / 5);
    painter->drawText(txtRect1, Qt::AlignCenter, node->oilTmp()->vert2Name());
    txtRect1 = QRect(leftWidth + stepWidth + stepWidth2, height / 8 + height, stepWidth, height * 4 / 5);
    painter->drawText(txtRect1, Qt::AlignCenter, node->oilPressure()->vert2Name());
    painter->restore();

    if (node) {
        painter->save();
        painter->setPen(QPen(_fontColor));
        QColor color1 = node->inletTotalTmp()->showColor() ? _warnColor : _color;
        double rate1 = 1 - node->inletTotalTmp()->rateValue();
        int gap1 = rate1 * (height * 4 / 5);

        QRect firstRect2(leftWidth + stepWidth, height / 8 + gap1, stepWidth, height * 4 / 5 - gap1);
        painter->fillRect(firstRect2, color1);

        QRect firstRect(leftWidth + stepWidth, height / 8, stepWidth, height * 4 / 5);

        QString dval = node->inletTotalTmp()->display();
        QString note = QString("%1\n%2").arg(dval.toInt()).arg(node->inletTotalTmp()->unit());

        painter->drawText(firstRect, Qt::AlignCenter, note);

        QPainterPath path;
        painter->setPen(QPen(color1, 1));
        path.moveTo(leftWidth + stepWidth, height / 8);
        path.lineTo(leftWidth + stepWidth, height / 8 + height * 4 / 5);
        path.lineTo(leftWidth + stepWidth + stepWidth, height / 8 + height * 4 / 5);
        path.lineTo(leftWidth + stepWidth + stepWidth, height / 8);
        path.lineTo(leftWidth + stepWidth, height / 8);
        painter->drawPath(path);
        painter->restore();
    }

    if (node) {
        painter->save();
        painter->setPen(QPen(_fontColor));

        QColor color1 = node->exhuastTmp()->showColor() ? _warnColor : _color;
        double rate1 = 1 - node->exhuastTmp()->rateValue();
        int gap1 = rate1 * (height * 4 / 5);

        QRect firstRect2(rightPos, height / 8 + gap1, stepWidth3, height * 4 / 5 - gap1);
        painter->fillRect(firstRect2, color1);

        QRect firstRect(rightPos, height / 8, stepWidth3, height * 4 / 5);

        QString dval = node->exhuastTmp()->display();
        QString note = QString("%1\n%2").arg((int) dval.toDouble()).arg(node->exhuastTmp()->unit());

        painter->drawText(firstRect, Qt::AlignCenter, note);

        QPainterPath path;
        painter->setPen(QPen(color1, 1));
        path.moveTo(rightPos, height / 8);
        path.lineTo(rightPos, height / 8 + height * 4 / 5);
        path.lineTo(rightPos + stepWidth3, height / 8 + height * 4 / 5);
        path.lineTo(rightPos + stepWidth3, height / 8);
        path.lineTo(rightPos, height / 8);
        painter->drawPath(path);
        painter->restore();
    }

    if (node) {
        painter->save();
        painter->setPen(QPen(_fontColor));

        QColor color1 = node->oilTmp()->showColor() ? _warnColor : _color;
        double rate1 = 1 - node->oilTmp()->rateValue();
        int gap1 = rate1 * (height * 4 / 5);

        QRect firstRect2(leftWidth + stepWidth, height / 8 + height + gap1, stepWidth, height * 4 / 5 - gap1);
        painter->fillRect(firstRect2, color1);

        QRect firstRect(leftWidth + stepWidth, height / 8 + height, stepWidth, height * 4 / 5);

        QString dval = node->oilTmp()->display();
        QString note = QString("%1\n%2").arg(dval.toInt()).arg(node->oilTmp()->unit());

        painter->drawText(firstRect, Qt::AlignCenter, note);

        QPainterPath path;
        painter->setPen(QPen(color1, 1));
        path.moveTo(leftWidth + stepWidth, height / 8 + height);
        path.lineTo(leftWidth + stepWidth, height / 8 + height + height * 4 / 5);
        path.lineTo(leftWidth + stepWidth + stepWidth, height / 8 + height + height * 4 / 5);
        path.lineTo(leftWidth + stepWidth + stepWidth, height / 8 + height);
        path.lineTo(leftWidth + stepWidth, height / 8 + height);
        painter->drawPath(path);
        painter->restore();
    }

    if (node) {
        painter->save();
        painter->setPen(QPen(_fontColor));
        QColor color1 = node->oilPressure()->showColor() ? _warnColor : _color;
        double rate1 = 1 - node->oilPressure()->rateValue();
        int gap1 = rate1 * (height * 4 / 5);

        QRect firstRect2(rightPos, height / 8 + height + gap1, stepWidth3, height * 4 / 5 - gap1);
        painter->fillRect(firstRect2, color1);

        QRect firstRect(rightPos, height / 8 + height, stepWidth3, height * 4 / 5);

        QString dval = node->oilPressure()->display();
        QString note = QString("%1\n%2").arg(dval.toInt()).arg(node->oilPressure()->unit());

        painter->drawText(firstRect, Qt::AlignCenter, note);

        QPainterPath path;
        painter->setPen(QPen(color1, 1));
        path.moveTo(rightPos, height / 8 + height);
        path.lineTo(rightPos, height / 8 + height + height * 4 / 5);
        path.lineTo(rightPos + stepWidth3, height / 8 + height + height * 4 / 5);
        path.lineTo(rightPos + stepWidth3, height / 8 + height);
        path.lineTo(rightPos, height / 8 + height);
        painter->drawPath(path);
        painter->restore();
    }
}
