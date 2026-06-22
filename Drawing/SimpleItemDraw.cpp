#pragma once

#include "SimpleItemDraw.h"
#include "chchar.h"

#include "log_category.h"

#include <QPainterPath>
#include <QQuickPaintedItem>
#include <qpainter.h>

QuickSimpleElemNode::QuickSimpleElemNode(QQuickItem *parent) : QQuickPaintedItem(parent) {
    _color = QColor(0, 255, 0);
    _borderColor = QColor(48, 48, 48, 255);
    connect(this, &QuickSimpleElemNode::colorChanged, this, [=] { update(); });
}

void QuickSimpleElemNode::setData(SimpleElemNode *name) {
    if (name == nullptr) {
        return;
    }

    _dataNode = name;

    connect(&_dataNode->_default, &FlightDisplay::updateColorValue, this, &QuickSimpleElemNode::doUpdateQuick);
}

void QuickSimpleElemNode::doUpdateQuick() { update(); }

void QuickSimpleElemNode::paint(QPainter *painter) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(_color));
    QRectF rect = boundingRect();
    int width = rect.width();
    int height = rect.height();

    QRect txtRect(width / 8, height / 8, width / 2, height * 4 / 5);
    QColor color1 = _dataNode->_default.showColor() ? _warnColor : _color;
    double rate1 = 1 - _dataNode->_default.rateValue();
    int gap1 = rate1 * (height * 4 / 5 - height / 8);
    QRect firstRect(width * 5 / 8, height / 8, width / 3, gap1);
    QRect firstRect2(width * 5 / 8, height / 8 + gap1, width / 3, height * 4 / 5 - gap1);
    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
    painter->drawText(txtRect, Qt::AlignCenter, _dataNode->vGroupName());
    painter->fillRect(firstRect, _borderColor);
    painter->fillRect(firstRect2, color1);
    painter->restore();
}

QuickBusbarElemNode::QuickBusbarElemNode(QQuickItem *parent) : QQuickPaintedItem(parent) {
    _color = QColor(0, 255, 0);
    _fontColor = QColor(0, 255, 0);
    _whiteColor = QColor(255, 255, 255);
    _warnColor = QColor(255, 0, 0);
    connect(this, &QuickBusbarElemNode::colorChanged, this, [=] { update(); });
}

void QuickBusbarElemNode::setData(SimpleElemNode *name) {
    if (name == nullptr) {
        COMMON_WARN("invalid input param");
        return;
    }

    _dataNode = name;
    BusbarElemNode *node = dynamic_cast<BusbarElemNode *>(_dataNode);
    if (node == nullptr) {
        COMMON_WARN("unknown input {}", name->groupName().toStdString());
        return;
    }

    connect(node->valtage(), &FlightDisplay::updateColorValue, this, &QuickBusbarElemNode::doUpdateQuick);

    connect(node->current(), &FlightDisplay::updateColorValue, this, &QuickBusbarElemNode::doUpdateQuick);
}

void QuickBusbarElemNode::printMessage(QString val, int color) {
    COMMON_WARN("value {}, color {}", val.toStdString(), color);
}

void QuickBusbarElemNode::doUpdateQuick(QString val, int color) { update(); }

QColor QuickBusbarElemNode::getColor(int color) {
    switch (color & 0xf) {
    case GreenColor:
        return _color;
    case RedColor:
    case OrangeColor:
    case YellowColor:
        return _warnColor;
    case WhiteColor:
        return _whiteColor;
    }
    return _color;
}

void QuickBusbarElemNode::paint(QPainter *painter) {
    QRectF rect = boundingRect();
    int width = rect.width();
    int height = rect.height();

    BusbarElemNode *node = dynamic_cast<BusbarElemNode *>(_dataNode);
    if (node == nullptr) {
        COMMON_WARN("unknown input {}", node->groupName().toStdString());
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(_fontColor));

    int dwidth = width / 16;
    int dheight = height / 8;
    QColor color1 = getColor(node->valtage()->showColor());
    QColor color2 = getColor(node->current()->showColor());
    QRect txtRect(0, dheight, dwidth * 5, height * 4 / 5);
    float hsets = height * 4 / 5;
    float rate1 = 1 - node->valtage()->rateValue();
    float rate2 = 1 - node->current()->rateValue();
    float gap1 = rate1 * hsets;
    float gap2 = rate2 * hsets;
    float gap11 = hsets - gap1;
    float gap21 = hsets - gap2;
    QRect firstRect(dwidth * 5, dheight, dwidth * 4, height * 4 / 5);
    QRect secondRect(dwidth * 11, dheight, dwidth * 4, height * 4 / 5);

    if (gap11 > 0) {
        QRect firstRect2(dwidth * 5, dheight + gap1, dwidth * 4, gap11);
        painter->fillRect(firstRect2, color1);
    }
    if (gap21 > 0) {
        QRect secondRect2(dwidth * 11, dheight + gap2, dwidth * 4, gap21);
        painter->fillRect(secondRect2, color2);
    }

    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
    painter->drawText(txtRect, Qt::AlignCenter, _dataNode->vGroupName());
    painter->drawText(firstRect, Qt::AlignCenter, node->valtage()->display() + "\n" + node->valtage()->unit());
    painter->drawText(secondRect, Qt::AlignCenter, node->current()->display() + "\n" + node->current()->unit());
    painter->restore();

    painter->save();
    QPainterPath path4;
    painter->setPen(QPen(color1, 1));
    path4.moveTo(dwidth * 5, dheight);
    path4.lineTo(dwidth * 5, dheight + height * 4 / 5);
    path4.lineTo(dwidth * 5 + dwidth * 4, dheight + height * 4 / 5);
    path4.lineTo(dwidth * 5 + dwidth * 4, dheight);
    path4.lineTo(dwidth * 5, dheight);
    path4.moveTo(dwidth * 11, dheight);
    path4.lineTo(dwidth * 11, dheight + height * 4 / 5);
    path4.lineTo(dwidth * 11 + dwidth * 4, dheight + height * 4 / 5);
    path4.lineTo(dwidth * 11 + dwidth * 4, dheight);
    path4.lineTo(dwidth * 11, dheight);
    painter->drawPath(path4);
    painter->restore();
}

QuickHBusbarElemNode::QuickHBusbarElemNode(QQuickItem *parent) : QuickBusbarElemNode(parent) {}

void QuickHBusbarElemNode::printMessage(QString val, int color) {
    COMMON_WARN("value {}, color {}", val.toStdString(), color);
}

void QuickHBusbarElemNode::paint(QPainter *painter) {
    QRectF rect = boundingRect();
    int width = rect.width();
    int height = rect.height();
    BusbarElemNode *node = (BusbarElemNode *) (_dataNode);
    if (node == nullptr) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(_fontColor));
    int dwidth = (width - 10) / 6;
    int dheight = height / 5;

    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
    if (node->capacity() != nullptr) {
        QRect rect(dwidth * 4 + 10 + 2, dheight + dheight / 4, dwidth * 2 + 8, height * 4);
        QString tmp = node->capacity()->showColor() > 0xf ? "--" : node->capacity()->display();
        tmp.append(node->capacity()->unit());
        painter->drawText(rect, tmp);
    } else if (node->valtage() != nullptr) {
        QRect rect(dwidth * 4 + 10 + 2, dheight + dheight / 4, dwidth * 2 + 8, height * 4);
        QString tmp = node->valtage()->showColor() > 0xf ? "--" : node->valtage()->display();
        tmp.append(node->valtage()->unit());
        painter->drawText(rect, tmp);
    }

    QRect txtRect(0, dheight, dwidth * 2 + 10, dheight * 4);
    painter->drawText(txtRect, Qt::AlignCenter, node->moduleAbbr());
    painter->restore();
    painter->save();

    if (node->capacity() != nullptr) {
        QColor color1 = getColor(node->capacity()->showColor());
        QPainterPath path4;
        painter->setPen(QPen(color1, 1));
        path4.moveTo(dwidth * 2 + 10, dheight * 2);
        path4.lineTo(dwidth * 4 + 10, dheight * 2);
        path4.lineTo(dwidth * 4 + 10, dheight * 4);
        path4.lineTo(dwidth * 2 + 10, dheight * 4);
        path4.lineTo(dwidth * 2 + 10, dheight * 2);
        painter->drawPath(path4);
        float rate1 = node->capacity()->rateValue() * (dwidth * 2);
        QPainterPath path;
        path.moveTo(dwidth * 2 + 10 + rate1, dheight * 2);
        path.lineTo(dwidth * 2 + 10 + rate1 + 4, dheight);
        path.lineTo(dwidth * 2 + 10 + rate1 - 4, dheight);
        path.lineTo(dwidth * 2 + 10 + rate1, dheight * 2);
        QRect firstRect2(dwidth * 2 + 10, dheight * 2, rate1, dheight * 2);
        painter->fillRect(firstRect2, color1);
        painter->drawPath(path);
    } else if (node->valtage() != nullptr) {
        QColor color1 = getColor(node->valtage()->showColor());
        QPainterPath path4;
        painter->setPen(QPen(color1, 1));
        path4.moveTo(dwidth * 2 + 10, dheight * 2);
        path4.lineTo(dwidth * 4 + 10, dheight * 2);
        path4.lineTo(dwidth * 4 + 10, dheight * 4);
        path4.lineTo(dwidth * 2 + 10, dheight * 4);
        path4.lineTo(dwidth * 2 + 10, dheight * 2);
        painter->drawPath(path4);
        float rate1 = node->valtage()->rateValue() * (dwidth * 2);
        QPainterPath path;
        path.moveTo(dwidth * 2 + 10 + rate1, dheight * 2);
        path.lineTo(dwidth * 2 + 10 + rate1 + 4, dheight);
        path.lineTo(dwidth * 2 + 10 + rate1 - 4, dheight);
        path.lineTo(dwidth * 2 + 10 + rate1, dheight * 2);
        QRect firstRect2(dwidth * 2 + 10, dheight * 2, rate1, dheight * 2);
        painter->fillRect(firstRect2, color1);
        painter->drawPath(path);
    }

    painter->restore();
}

QuickBatteryElemNode::QuickBatteryElemNode(QQuickItem *parent) : QQuickPaintedItem(parent) {
    _color = QColor(0, 255, 0);
    _fontColor = QColor(0, 255, 0);
    connect(this, &QuickBatteryElemNode::colorChanged, this, [=] { update(); });
}

void QuickBatteryElemNode::setData(SimpleElemNode *name) {
    if (name == nullptr) {
        COMMON_WARN("invalid input param");
        return;
    }

    _dataNode = name;
    BatteryElemNode *node = dynamic_cast<BatteryElemNode *>(_dataNode);
    if (node == nullptr) {
        COMMON_WARN("unknown input {}", node->groupName().toStdString());
        return;
    }

    connect(node->valtage(), &FlightDisplay::updateColorValue, this, &QuickBatteryElemNode::doUpdateQuick);

    connect(node->capacity(), &FlightDisplay::updateColorValue, this, &QuickBatteryElemNode::doUpdateQuick);
}

void QuickBatteryElemNode::doUpdateQuick() { update(); }

void QuickBatteryElemNode::paint(QPainter *painter) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(_fontColor));
    QRectF rect = boundingRect();
    int width = rect.width();
    int height = rect.height();
    BatteryElemNode *node = (BatteryElemNode *) (_dataNode);
    if (node == nullptr) {
        painter->restore();
        return;
    }
    int dwidth = width / 16;
    int dheight = height / 8;
    int hsets = height * 4 / 5 - 10;
    QColor color1 = node->valtage()->showColor() ? _warnColor : _color;
    QColor color2 = node->current()->showColor() ? _warnColor : _color;
    QRect txtRect(0, dheight, dwidth * 5, height * 4 / 5);
    float rate1 = 1 - node->valtage()->rateValue();
    float rate2 = 1 - node->current()->rateValue();
    float gap1 = rate1 * hsets;
    float gap2 = rate2 * hsets;
    float gap11 = hsets - gap1;
    float gap21 = hsets - gap2;
    if (gap11 > 0) {
        QRect firstRect2(dwidth * 5, dheight + gap1, dwidth * 4, gap11);
        painter->fillRect(firstRect2, color1);
    }
    if (gap21 > 0) {
        QRect secondRect2(dwidth * 11, dheight + gap2, dwidth * 4, gap21);
        painter->fillRect(secondRect2, color2);
    }
    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
    QRect firstRect(dwidth * 5, dheight, dwidth * 4, hsets);
    QRect secondRect(dwidth * 11, dheight, dwidth * 4, hsets);
    painter->drawText(txtRect, Qt::AlignCenter, _dataNode->vGroupName());
    painter->drawText(firstRect, Qt::AlignCenter, node->valtage()->display() + "\n" + node->valtage()->unit());
    painter->drawText(secondRect, Qt::AlignCenter, node->current()->display() + "\n" + node->current()->unit());
    painter->restore();
    painter->save();
    QPainterPath path4;
    painter->setPen(QPen(color1, 1));
    path4.moveTo(dwidth * 5, dheight);
    path4.lineTo(dwidth * 5, dheight + hsets);
    path4.lineTo(dwidth * 5 + dwidth * 4, dheight + hsets);
    path4.lineTo(dwidth * 5 + dwidth * 4, dheight);
    path4.lineTo(dwidth * 5, dheight);

    path4.moveTo(dwidth * 11, dheight);
    path4.lineTo(dwidth * 11, dheight + hsets);
    path4.lineTo(dwidth * 11 + dwidth * 4, dheight + hsets);
    path4.lineTo(dwidth * 11 + dwidth * 4, dheight);
    path4.lineTo(dwidth * 11, dheight);
    painter->drawPath(path4);
    painter->restore();
}

QuickEngineElemNode::QuickEngineElemNode(QQuickItem *parent) : QQuickPaintedItem(parent) {
    _color = QColor(0, 255, 0);
    _fontColor = QColor(0, 255, 0);
    _setColor = QColor(125, 0, 225);
    _warnColor = QColor(248, 108, 1, 255);
    connect(this, &QuickEngineElemNode::fontColorChanged, this, [=] { update(); });
    connect(this, &QuickEngineElemNode::setColorChanged, this, [=] { update(); });
    connect(this, &QuickEngineElemNode::colorChanged, this, [=] { update(); });
}

void QuickEngineElemNode::setData(SimpleElemNode *name) {
    if (name == nullptr) {
        COMMON_WARN("invalid input param");
        return;
    }

    _dataNode = name;
    EngineElemNode *node = dynamic_cast<EngineElemNode *>(_dataNode);
    if (node == nullptr) {
        COMMON_WARN("unknown input {}", node->groupName().toStdString());
        return;
    }

    connect(node->inletTotalTmp(), &FlightDisplay::updateColorValue, this, &QuickEngineElemNode::doUpdateQuick);
    connect(node->oilTmp(), &FlightDisplay::updateColorValue, this, &QuickEngineElemNode::doUpdateQuick);
    connect(node->oilPressure(), &FlightDisplay::updateColorValue, this, &QuickEngineElemNode::doUpdateQuick);
    connect(node->throttle(), &FlightDisplay::updateColorValue, this, &QuickEngineElemNode::doUpdateQuick);
    connect(node->physicalRSpeed(), &FlightDisplay::updateColorValue, this, &QuickEngineElemNode::doUpdateQuick);
    connect(node->givenSpeed(), &FlightDisplay::updateColorValue, this, &QuickEngineElemNode::doUpdateQuick);
    connect(node->engineLevel(), &FlightDisplay::updateColorValue, this, &QuickEngineElemNode::doUpdateQuick);
    connect(node->exhuastTmp(), &FlightDisplay::updateColorValue, this, &QuickEngineElemNode::doUpdateQuick);
}

void QuickEngineElemNode::doUpdateQuick(QString val, int color) { update(); }

static void drawIndicator(QPainter *painter, int radius, int angle) {
    painter->save();
    QPolygon pts;
    int r = radius * 0.6;
    pts.setPoints(3, -2, 0, 2, 0, 0, r);
    double degRotate = angle * 40 + 60;
    painter->rotate(degRotate);
    QRadialGradient haloGradient(0, 0, 60, 0, 0);
    haloGradient.setColorAt(0, QColor(100, 100, 100));
    haloGradient.setColorAt(1, QColor(250, 50, 50));
    painter->setPen(QColor(250, 150, 150));
    painter->setBrush(haloGradient);
    painter->drawConvexPolygon(pts);
    QRadialGradient radial(0, 0, 14);
    radial.setColorAt(0.0, QColor(100, 100, 100));
    radial.setColorAt(1.0, QColor(250, 250, 50));
    painter->setPen(Qt::NoPen);
    painter->setBrush(radial);
    painter->drawEllipse(-7, -7, 14, 14);
    painter->restore();
}

static void drawDigital(QPainter &painter, int radius) {
    painter.save();
    int r = (int) (radius * 0.6);
    painter.setFont(QFont("Arial", 10));
    painter.setPen(QPen(QColor(255, 255, 255)));
    QFontMetricsF fm = QFontMetricsF(painter.font());
    for (int i = 0; i <= 6; i += 1) {
        int angle = 150 + 40 * i;
        float angleArc = (angle % 360) * 3.14 / 180;
        int x = (r) *cos(angleArc);
        int y = (r) *sin(angleArc);
        QString value = QString::number(i);
        int w = (int) fm.width(value);
        int h = (int) fm.height();
        x = x - w / 2;
        y = y + h / 4;
        painter.drawText(QPointF(x, y), value);
    }
    painter.restore();
}

static void drawSmallScale(QPainter &painter, int radius, QColor color) {
    int r = radius;
    painter.save();
    painter.rotate(60);
    for (int i = 0; i <= 6; i++) {
        painter.setPen(QPen(color, 2));
        painter.drawLine(0, radius - 2, 0, radius - 10);
        painter.rotate(40);
    }
    painter.restore();
}

static void drawCircle(QPainter &painter, int radius, QColor color) {
    painter.save();
    int diff = 5;
    QPainterPath outRing;
    QPainterPath inRing;
    outRing.moveTo(0, 0);
    inRing.moveTo(0, 0);
    outRing.arcTo(-radius, -radius, 2 * radius, 2 * radius, -45, 270);
    inRing.addEllipse(-radius + diff, -radius + diff, 2 * (radius - diff), 2 * (radius - diff));
    outRing.closeSubpath();
    QRadialGradient radialGradient(0, 0, radius, 0, 0);
    radialGradient.setColorAt(1, color);
    radialGradient.setColorAt(0.8, QColor(0, 82, 199));
    painter.setBrush(radialGradient);
    painter.drawPath(outRing.subtracted(inRing));
    painter.restore();
}

void QuickEngineElemNode::drawEnine(QPainter *painter) {
    EngineElemNode *node = dynamic_cast<EngineElemNode *>(_dataNode);
    float engineThrottle = 0;
    QString physicalStr = "";
    QString givenStr = "";
    QString throttleStr = "";
    QString gearStr = "";
    QColor givenColor;

    if (node != nullptr) {
        engineThrottle = node->throttle()->display().toFloat();
        double val = node->physicalRSpeed()->display().toFloat();
        physicalStr = node->physicalRSpeed()->itemName() + ": " + QString::number(val);
        val = node->givenSpeed()->display().toFloat();
        givenStr = node->givenSpeed()->itemName() + ": " + QString::number(val);
        throttleStr = node->engineLevel()->itemName() + ": " + node->engineLevel()->display();
        val = node->engineGear()->display().toFloat();
        gearStr = node->engineGear()->itemName() + ": " + QString::number(val);
        givenColor = node->givenSpeed()->showColor() ? _warnColor : _color;
    }

    QRectF rect = boundingRect();
    int width = rect.width() / 2;
    int height = rect.height() / 2;
    int radius = (width > height ? height : width) / 2 - 8;
    int leftOffset = 10;

    painter->save();
    QPainterPath path2;
    QColor color = QColor(249, 108, 17);
    painter->setPen(QPen(color, 3));
    painter->translate(width / 2 + width + leftOffset, radius + 5);
    int xyoff2 = radius * std::sin(3.1415 / 4);
    path2.moveTo(xyoff2, xyoff2);
    path2.arcTo(-radius, -radius, radius * 2, radius * 2, -45, 45);
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
    path.arcTo(-radius, -radius, radius * 2, radius * 2, 45, 180 - 45);
    painter->drawPath(path);
    QPainterPath pathL;
    painter->setPen(QPen(color2, 3));
    pathL.moveTo(-radius, 0);
    pathL.arcTo(-radius, -radius, radius * 2, radius * 2, -180, 45);
    painter->drawPath(pathL);
    painter->restore();

    painter->save();
    QPainterPath path4;
    painter->setPen(QPen(_color, 2));
    float rateAngle = 3.1415 * 3 / 2;
    float curSpeed = engineThrottle * rateAngle;
    int xyoffs = radius * std::sin(rateAngle + 3.1415 / 4 - curSpeed);
    int xyoffc = radius * std::cos(rateAngle + 3.1415 / 4 - curSpeed);
    path4.moveTo(width / 2 + width + leftOffset - 1, radius + 5 - 1);
    path4.lineTo(width / 2 + width + leftOffset + xyoffs, radius + 5 + xyoffc);
    path4.lineTo(width / 2 + width + leftOffset + 1, radius + 5 - 1);
    path4.lineTo(width / 2 + width + leftOffset - 1, radius + 5 - 1);
    path4.addEllipse(width / 2 + width + leftOffset - 3, radius + 5 - 3, 6, 6);
    painter->fillPath(path4, QBrush(_color));
    painter->drawPath(path4);
    painter->restore();

    painter->save();
    painter->setPen(QPen(_fontColor));

    QRect txtRect(width + leftOffset, radius * 2 + 20, width, 25);
    painter->drawText(txtRect, Qt::AlignLeft | Qt::AlignVCenter, physicalStr);

    QRect gearRect(width + leftOffset, radius * 2 + 25 + 20, width, 25);
    painter->drawText(gearRect, Qt::AlignLeft | Qt::AlignVCenter, gearStr);

    QRect throttleRect(width + leftOffset, radius * 2 + 50 + 20, width, 25);
    painter->drawText(throttleRect, Qt::AlignLeft | Qt::AlignVCenter, throttleStr);
    painter->setPen(QPen(_setColor));

    QRect txtSetRect(width + leftOffset, radius * 2 - 5, width, 25);
    painter->drawText(txtSetRect, Qt::AlignLeft | Qt::AlignVCenter, givenStr);
    painter->restore();
}

void QuickEngineElemNode::paint(QPainter *painter) {

    QRectF rect = boundingRect();
    int width = rect.width() / 2;
    int height = rect.height() / 2;
    int stepWidth = width / 5;
    int stepWidth2 = width / 6 + 4;
    int stepWidth3 = width / 4;
    int leftWidth = 6;

    EngineElemNode *node = dynamic_cast<EngineElemNode *>(_dataNode);
    if (node == nullptr) {
        return;
    }

    QFont font(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setFont(font);

    drawEnine(painter);

    painter->save();
    painter->setPen(QPen(_color));
    QRect txtRect1(leftWidth, height / 8, stepWidth, height * 4 / 5);
    painter->drawText(txtRect1, Qt::AlignCenter, node->inletTotalTmp()->vertName());
    txtRect1 = QRect(leftWidth + 2 * stepWidth, height / 8, stepWidth, height * 4 / 5);
    painter->drawText(txtRect1, Qt::AlignCenter, node->exhuastTmp()->vertName());
    txtRect1 = QRect(leftWidth, height / 8 + height, stepWidth, height * 4 / 5);
    painter->drawText(txtRect1, Qt::AlignCenter, node->oilTmp()->vertName());
    txtRect1 = QRect(leftWidth + 2 * stepWidth, height / 8 + height, stepWidth3, height * 4 / 5);
    painter->drawText(txtRect1, Qt::AlignCenter, node->oilPressure()->vertName());
    painter->restore();

    if (node) {
        painter->save();
        painter->setPen(QPen(_fontColor));
        int color1 = node->inletTotalTmp()->showColor();
        double rate1 = 1 - node->inletTotalTmp()->rateValue();
        int gap1 = rate1 * (height * 4 / 5);
        QRect firstRect2(leftWidth + stepWidth, height / 8 + gap1, stepWidth2, height * 4 / 5 - gap1);
        painter->fillRect(firstRect2, color1 ? _warnColor : _color);
        QRect firstRect(leftWidth + stepWidth, height / 8, stepWidth2, height * 4 / 5);
        QString dval = node->inletTotalTmp()->display();
        QString note = QString("%1\n%2").arg(dval.toInt()).arg(node->inletTotalTmp()->unit());
        painter->drawText(firstRect, Qt::AlignCenter, note);
        QPainterPath path;
        painter->setPen(QPen(color1 ? _warnColor : _color, 1));
        path.moveTo(leftWidth + stepWidth, height / 8);
        path.lineTo(leftWidth + stepWidth, height / 8 + height * 4 / 5);
        path.lineTo(leftWidth + stepWidth + stepWidth2, height / 8 + height * 4 / 5);
        path.lineTo(leftWidth + stepWidth + stepWidth2, height / 8);
        path.lineTo(leftWidth + stepWidth, height / 8);
        painter->drawPath(path);
        painter->restore();
    }

    if (node) {
        painter->save();
        painter->setPen(QPen(_fontColor));
        int color1 = node->exhuastTmp()->showColor();
        double rate1 = 1 - node->exhuastTmp()->rateValue();
        int gap1 = rate1 * (height * 4 / 5);
        QRect firstRect2(leftWidth + 3 * stepWidth, height / 8 + gap1, stepWidth3, height * 4 / 5 - gap1);
        painter->fillRect(firstRect2, color1 ? _warnColor : _color);
        QRect firstRect(leftWidth + 3 * stepWidth, height / 8, stepWidth3, height * 4 / 5);
        QString dval = node->exhuastTmp()->display();
        QString note = QString("%1\n%2").arg((int) dval.toDouble()).arg(node->exhuastTmp()->unit());
        painter->drawText(firstRect, Qt::AlignCenter, note);
        QPainterPath path;
        painter->setPen(QPen(color1 ? _warnColor : _color, 1));
        path.moveTo(leftWidth + 3 * stepWidth, height / 8);
        path.lineTo(leftWidth + 3 * stepWidth, height / 8 + height * 4 / 5);
        path.lineTo(leftWidth + 3 * stepWidth + stepWidth3, height / 8 + height * 4 / 5);
        path.lineTo(leftWidth + 3 * stepWidth + stepWidth3, height / 8);
        path.lineTo(leftWidth + 3 * stepWidth, height / 8);
        painter->drawPath(path);
        painter->restore();
    }

    if (node) {
        painter->save();
        painter->setPen(QPen(_fontColor));
        int color1 = node->oilTmp()->showColor();
        double rate1 = 1 - node->oilTmp()->rateValue();
        int gap1 = rate1 * (height * 4 / 5);
        QRect firstRect2(leftWidth + stepWidth, height / 8 + height + gap1, stepWidth2, height * 4 / 5 - gap1);
        painter->fillRect(firstRect2, color1 ? _warnColor : _color);
        QRect firstRect(leftWidth + stepWidth, height / 8 + height, stepWidth2, height * 4 / 5);
        QString dval = node->oilTmp()->display();
        QString note = QString("%1\n%2").arg(dval.toInt()).arg(node->oilTmp()->unit());
        painter->drawText(firstRect, Qt::AlignCenter, note);
        QPainterPath path;
        painter->setPen(QPen(color1 ? _warnColor : _color, 1));
        path.moveTo(leftWidth + stepWidth, height / 8 + height);
        path.lineTo(leftWidth + stepWidth, height / 8 + height + height * 4 / 5);
        path.lineTo(leftWidth + stepWidth + stepWidth2, height / 8 + height + height * 4 / 5);
        path.lineTo(leftWidth + stepWidth + stepWidth2, height / 8 + height);
        path.lineTo(leftWidth + stepWidth, height / 8 + height);
        painter->drawPath(path);
        painter->restore();
    }

    if (node) {
        painter->save();
        painter->setPen(QPen(_fontColor));
        int color1 = node->oilPressure()->showColor();
        double rate1 = 1 - node->oilPressure()->rateValue();
        int gap1 = rate1 * (height * 4 / 5);
        QRect firstRect2(leftWidth + 3 * stepWidth, height / 8 + height + gap1, stepWidth3, height * 4 / 5 - gap1);
        painter->fillRect(firstRect2, color1 ? _warnColor : _color);
        QRect firstRect(leftWidth + 3 * stepWidth, height / 8 + height, stepWidth3, height * 4 / 5);
        QString dval = node->oilPressure()->display();
        QString note = QString("%1\n%2").arg(dval.toInt()).arg(node->oilPressure()->unit());
        painter->drawText(firstRect, Qt::AlignCenter, note);
        QPainterPath path;
        painter->setPen(QPen(color1 ? _warnColor : _color, 1));
        path.moveTo(leftWidth + 3 * stepWidth, height / 8 + height);
        path.lineTo(leftWidth + 3 * stepWidth, height / 8 + height + height * 4 / 5);
        path.lineTo(leftWidth + 3 * stepWidth + stepWidth3, height / 8 + height + height * 4 / 5);
        path.lineTo(leftWidth + 3 * stepWidth + stepWidth3, height / 8 + height);
        path.lineTo(leftWidth + 3 * stepWidth, height / 8 + height);
        painter->drawPath(path);
        painter->restore();
    }
}

void QuickEngineElemNode2::drawElem(QPainter *painter, FlightDisplay *display, int left, int top, int width,
                                    int height) {
    if (display == nullptr) {
        return;
    }

    painter->save();
    painter->setPen(QPen(_fontColor));
    int color1 = display->showColor();
    double rate1 = display->rateValue();
    int drawHigh = height * 3 / 4;
    int gap1 = rate1 * drawHigh;
    int dwidth = width / 8;
    int leftpos = left + width / 2 + dwidth;
    int toppos = top + height * 3 / 4 - gap1;
    QRect firstRect2(leftpos, toppos, width / 2 - dwidth, gap1);
    painter->fillRect(firstRect2, color1 & 0xf ? _warnColor : _color);
    QRect firstRect(left, top, width / 2 + dwidth, height * 3 / 4);
    painter->drawText(firstRect, Qt::AlignCenter, display->vertName());
    QPainterPath path;
    painter->setPen(QPen(color1 & 0xf ? _warnColor : _color, 1));
    path.moveTo(left + width / 2 + dwidth, top);
    path.lineTo(left + width / 2 + dwidth, top + height * 3 / 4);
    path.lineTo(left + width, top + height * 3 / 4);
    path.lineTo(left + width, top);
    path.lineTo(left + width / 2 + dwidth, top);
    painter->drawPath(path);
    painter->restore();

    painter->save();
    painter->setPen(QPen(_setColor));
    QString dval = color1 > 0xf ? "--" : display->display();
    QString note = QString("%1 %2").arg(dval).arg(display->unit());
    QRect firstRect3(left, top + height * 3 / 4, width, height / 4);
    painter->drawText(firstRect3, Qt::AlignCenter, note);
    painter->restore();
}

void QuickEngineElemNode2::paint(QPainter *painter) {
    QRectF rect = boundingRect();
    int stepWidth = rect.width() / 4 - 2;
    int height = rect.height() / 2 - 10;
    EngineElemNode *node = dynamic_cast<EngineElemNode *>(_dataNode);
    if (node == nullptr) {
        return;
    }

    QFont font(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setFont(font);

    drawElem(painter, node->inletTotalTmp(), 6, 26, stepWidth, height);
    drawElem(painter, node->oilTmp(), 6 + stepWidth, 26, stepWidth, height);
    drawElem(painter, node->exhuastTmp(), 6 + 2 * stepWidth, 26, stepWidth, height);
    drawElem(painter, node->oilPressure(), 6 + 3 * stepWidth, 26, stepWidth, height);
    // drawElem(painter, &node->physicalRSpeed(), 6+4*stepWidth, 26, stepWidth, height);
    drawElem(painter, node->engineLevel(), 6, 26 + height, stepWidth, height);
    drawElem(painter, node->throttle(), 6 + stepWidth, 26 + height, stepWidth, height);
    drawElem(painter, node->lowFuelWarn(), 6 + 2 * stepWidth, 26 + height, stepWidth, height);
    drawElem(painter, node->engineGear(), 6 + 3 * stepWidth, 26 + height, stepWidth, height);
    // drawElem(painter, &node->givenSpeed()  , 6+4*stepWidth, 26+height, stepWidth, height);
}

void QuickEngineElemNode3::drawElem(QPainter *painter, FlightDisplay *display, int left, int top, int width,
                                    int height) {
    if (display == nullptr) {
        return;
    }

    painter->save();
    painter->setPen(QPen(_fontColor));
    int color1 = display->showColor();
    double rate1 = display->rateValue();
    int leftpos = left;
    int toppos = top + 10;

    QRect firstRect1(leftpos, toppos, width * 2 / 7, height - 20);
    QRect firstRect2(leftpos + width * 2 / 7, toppos, width * 2 / 7, height / 3);

    if (display->itemName().length() >= 4) {
        painter->drawText(firstRect1, Qt::AlignCenter, display->vert3Name());
    } else {
        painter->drawText(firstRect1, Qt::AlignCenter, display->itemName());
    }

    painter->drawText(firstRect2, Qt::AlignCenter, display->display());
    QPainterPath path;
    // painter->setPen(QPen(color1&0xf ? _warnColor : _color, 1));
    path.moveTo(leftpos + width * 2 / 7, toppos);
    path.lineTo(leftpos + width * 2 / 7, toppos + height / 3);
    path.lineTo(leftpos + width * 4 / 7, toppos + height / 3);
    path.lineTo(leftpos + width * 4 / 7, toppos);
    path.lineTo(leftpos + width * 2 / 7, toppos);
    painter->drawPath(path);

    QPainterPath path3;
    path3.moveTo(leftpos + width * 2 / 7, toppos + height / 2);
    path3.lineTo(leftpos + width, toppos + height / 2);
    path3.lineTo(leftpos + width, toppos + height / 2 + height / 4);
    path3.lineTo(leftpos + width * 2 / 7, toppos + height / 2 + height / 4);
    path3.lineTo(leftpos + width * 2 / 7, toppos + height / 2);
    painter->drawPath(path3);

    QPainterPath path2;
    rate1 *= (width - width * 2 / 7);
    path2.moveTo(leftpos + width * 2 / 7, toppos + height / 2);
    path2.lineTo(leftpos + width * 2 / 7 + rate1, toppos + height / 2);
    path2.lineTo(leftpos + width * 2 / 7 + rate1, toppos + height / 2 + height / 4);
    path2.lineTo(leftpos + width * 2 / 7, toppos + height / 2 + height / 4);
    path2.lineTo(leftpos + width * 2 / 7, toppos + height / 2);

    QPainterPath path4;
    path4.moveTo(leftpos + width * 2 / 7 + rate1, toppos + height / 2);
    path4.lineTo(leftpos + width * 2 / 7 + rate1 + 5, toppos + height / 2 - 8);
    path4.lineTo(leftpos + width * 2 / 7 + rate1 - 5, toppos + height / 2 - 8);
    path4.lineTo(leftpos + width * 2 / 7 + rate1, toppos + height / 2);

    painter->fillPath(path4, color1 & 0xf ? _warnColor : _color);
    painter->fillPath(path2, color1 & 0xf ? _warnColor : _color);
    painter->restore();

    // QVector<ColorSet>& datrange = display->ranges();
    // float minvalue = 0;
    // float maxvalue = 0;
    // float ratv = 0;
    // if (datrange.size() > 2) {
    //     minvalue = datrange[0].ivalue;
    //     maxvalue = datrange[datrange.size()-1].value;
    //     ratv = (width - 20) / (maxvalue - minvalue);
    //     int startleft = left + 10;
    //     for (int ix = 0; (maxvalue > minvalue) && (ix < datrange.count()); ix++){
    //         int dwidth = ratv * (datrange[ix].value - datrange[ix].ivalue);
    //         QRect firstRect(startleft, toppos + height/2, dwidth, height/4);
    //         painter->save();
    //         if (datrange[ix].color == RedColor){
    //             painter->fillRect(firstRect, _redColor);
    //         } else if (datrange[ix].color == OrangeColor){
    //             painter->fillRect(firstRect, _warnColor);
    //         } else if (datrange[ix].color == GreenColor){
    //             painter->fillRect(firstRect, _greenColor);
    //         } else {
    //             painter->fillRect(firstRect, _color);
    //         }
    //         startleft += dwidth;
    //         painter->restore();
    //     }
    // }
}

void QuickEngineElemNode3::paint(QPainter *painter) {
    QRectF rect = boundingRect();
    int stepWidth = rect.width() - 40;
    int height = (rect.height() - 40) / 8;
    EngineElemNode *node = dynamic_cast<EngineElemNode *>(_dataNode);
    if (node == nullptr) {
        return;
    }

    QFont font(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setFont(font);

    drawElem(painter, node->engineLevel(), 20, 20 + 0 * height, stepWidth, height);
    drawElem(painter, node->physicalRSpeed(), 20, 20 + 1 * height, stepWidth, height);
    drawElem(painter, node->oilTmp(), 20, 20 + 2 * height, stepWidth, height);
    drawElem(painter, node->inletTotalTmp(), 20, 20 + 3 * height, stepWidth, height);
    drawElem(painter, node->exhuastTmp(), 20, 20 + 4 * height, stepWidth, height);
    drawElem(painter, node->oilPressure(), 20, 20 + 5 * height, stepWidth, height);
    drawElem(painter, node->lowFuelWarn(), 20, 20 + 6 * height, stepWidth, height);
    drawElem(painter, node->engineGear(), 20, 20 + 7 * height, stepWidth, height);
}
