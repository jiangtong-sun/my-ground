#pragma once

#include "ElectricItemDraw.h"
#include "chchar.h"

#include <QPainterPath>
#include <QQuickPaintedItem>
#include <qpainter.h>

QuickElectricElem3::QuickElectricElem3(QQuickItem *parent)
    : QuickElectricElem(parent), _generator(nullptr) {}

void QuickElectricElem3::setGenerator(SimpleElemList *name) {
    if (name == nullptr) {
        return;
    }

    _generator = name;
    FactListModel *dataModel = _generator->dataModel();

    if (dataModel != nullptr) {
        for (int ix = 0; ix < dataModel->count(); ix++) {
            BatteryElemNode *node = dataModel->value<BatteryElemNode *>(ix);
            if (node != nullptr && node->valtage() != nullptr) {
                connect(node->valtage(), &FlightDisplay::updateColorValue, this,
                        &QuickElectricElem3::doUpdateQuick3);
            }
        }

        doUpdateQuick3("", 0);
    }
}

void QuickElectricElem3::doUpdateQuick3(QString val, int color) { update();}

void QuickElectricElem3::drawItem3(QPainter *painter, BatteryElemNode *item, int left, int top,
                                   int width, int height, int ltop, int lbotm, int hstep) {

    assert(item != nullptr);
    if (item == nullptr || painter == nullptr) {
        return;
    }

    QColor curColor = getColor(item->charge());
    painter->save();
    painter->setPen(QPen(curColor, 1));
    QPainterPath path2;
    path2.moveTo(left, top);
    path2.lineTo(left + width, top);
    path2.lineTo(left + width, top + height);
    path2.lineTo(left, top + height);
    path2.lineTo(left, top);
    painter->drawPath(path2);
    painter->restore();
    painter->save();
    painter->setPen(QPen(_fontColor, 1));
    QRect txtRect(left, top, width, height);
    painter->drawText(txtRect, Qt::AlignCenter, item->v2GroupName());
    painter->restore();

    painter->save();
    painter->setPen(QPen(curColor, 1));
    QRect connpoint(left + width / 2 - 1, ltop, 2, 2);
    painter->drawEllipse(connpoint);
    QPainterPath path;
    path.moveTo(left + width / 2, ltop);
    path.lineTo(left + width / 2, top);
    painter->drawPath(path);
    painter->restore();

    painter->save();
    painter->setPen(QPen(curColor, 1));
    QPainterPath path3;
    QRect connpoint2(left + width / 2 - 1, lbotm - hstep * 2, 2, 2);
    QRect connpoint3(left - 1, lbotm - hstep, 2, 2);
    painter->drawEllipse(connpoint2);
    painter->drawEllipse(connpoint3);
    path3.moveTo(left + width / 2, top + height);
    path3.lineTo(left + width / 2, lbotm - hstep / 2);
    path3.moveTo(left, lbotm - hstep);
    path3.lineTo(left, lbotm);
    painter->drawPath(path3);
    painter->restore();
}

void QuickElectricElem3::drawItem2(QPainter *painter, ElecSpdCtrlElemNode *item, int left, int top,
                                   int width, int height, int ltop, int lbotm) {

    assert(item != nullptr);
    if (item == nullptr || painter == nullptr) {
        return;
    }

    QColor curColor = getColor(item->charge());
    painter->save();
    painter->setPen(QPen(curColor, 1));
    QPainterPath path2;
    path2.moveTo(left, top);
    path2.lineTo(left + width, top);
    path2.lineTo(left + width, top + height);
    path2.lineTo(left, top + height);
    path2.lineTo(left, top);
    painter->drawPath(path2);
    painter->restore();
    painter->save();
    painter->setPen(QPen(_fontColor, 1));
    QRect txtRect(left, top, width, height);
    painter->drawText(txtRect, Qt::AlignCenter, item->v2GroupName());

    // QRect txtRect2 = QRect(left, top + height, width / 2, lbotm - height - top);
    // QString currStr = item->current()->display() + item->current()->unit();
    // painter->drawText(txtRect2, Qt::AlignCenter, currStr);
    // QRect txtRect3 = QRect(left, top + height * 2 / 3, width, height / 3);
    // QString valtageStr = item->valtage()->display() + item->valtage()->unit();
    // painter->drawText(txtRect3, Qt::AlignCenter, valtageStr);
    painter->restore();

    painter->save();
    painter->setPen(QPen(curColor, 1));
    QPainterPath path;
    path.moveTo(left + width / 2, ltop);
    path.lineTo(left + width / 2, top - height - 30);
    path.moveTo(left + width / 2 - 6, top - height);
    path.lineTo(left + width / 2 + 6, top - height);
    path.lineTo(left + width / 2 + 6, top - height - 30);
    path.lineTo(left + width / 2 - 6, top - height - 30);
    path.lineTo(left + width / 2 - 6, top - height);
    path.moveTo(left + width / 2, top - height);
    path.lineTo(left + width / 2, top);
    painter->drawPath(path);
    painter->restore();

    painter->save();
    painter->setPen(QPen(curColor, 1));
    QPainterPath path3;
    path3.moveTo(left + width / 2, top + height);
    path3.lineTo(left + width / 2, lbotm);
    painter->drawPath(path3);
    painter->restore();
}

void QuickElectricElem3::drawItem(QPainter *painter, BatteryElemNode *item, int left, int top,
                                  int width, int height, int botm) {
    assert(item != nullptr);
    if (item == nullptr || painter == nullptr) {
        return;
    }

    QColor curColor = getColor(item->valtage());
    painter->save();
    painter->setPen(QPen(curColor, 1));
    QPainterPath path2;
    path2.moveTo(left, top);
    path2.lineTo(left + width, top);
    path2.lineTo(left + width, top + height);
    path2.lineTo(left, top + height);
    path2.lineTo(left, top);
    painter->drawPath(path2);
    painter->restore();
    painter->save();
    painter->setPen(QPen(_fontColor, 1));
    QRect txtRect(left, top, width, height);
    painter->drawText(txtRect, Qt::AlignCenter, item->v2GroupName());
    QRect txtRect2 = QRect(left, top + height, width / 2, botm - height - top);
    QString currStr = item->current()->display() + item->current()->unit();
    painter->drawText(txtRect2, Qt::AlignCenter, currStr);
    // QRect txtRect3 = QRect(left, top + height * 2 / 3, width, height / 3);
    // QString valtageStr = item->valtage()->display() + item->valtage()->unit();
    // painter->drawText(txtRect3, Qt::AlignCenter, valtageStr);
    painter->restore();

    painter->save();
    painter->setPen(QPen(curColor, 1));
    QPainterPath path;
    path.moveTo(left + width / 2, top + height);
    path.lineTo(left + width / 2, botm);
    painter->drawPath(path);
    painter->restore();
}

void QuickElectricElem3::drawItem(QPainter *painter, BatteryElemNode *item, int left, int top,
                                  int width, int height) {
    assert(item != nullptr);
    if (item == nullptr || painter == nullptr) {
        return;
    }

    QColor curColor = getColor(item->valtage());
    painter->save();
    painter->setPen(QPen(curColor, 1));
    QPainterPath path2;
    path2.moveTo(left, top);
    path2.lineTo(left + width, top);
    path2.lineTo(left + width, top + height);
    path2.lineTo(left, top + height);
    path2.lineTo(left, top);
    painter->drawPath(path2);
    painter->restore();

    painter->save();
    painter->setPen(QPen(_fontColor, 1));
    QRect txtRect(left, top, width, height);
    painter->drawText(txtRect, Qt::AlignCenter, item->groupName());
    painter->restore();
}

void QuickElectricElem3::paint(QPainter *painter) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
    painter->setPen(QPen(_color, 1));
    QRectF rect = boundingRect();
    int width = rect.width();
    int height = rect.height();
    int gapRH = height / 24;
    int gapRW = width / 20;

    if (_generator != nullptr) {
        FactListModel *dataModel = _generator->dataModel();
        assert(dataModel != nullptr);
        BatteryElemNode *node = dataModel->value<BatteryElemNode *>(0);
        if (node != nullptr) {
            drawItem(painter, node, gapRW, gapRH, gapRW * 14, gapRH);
        }
        node = dataModel->value<BatteryElemNode *>(1);
        if (node != nullptr) {
            drawItem(painter, node, gapRW * 17, gapRH, gapRW * 2, gapRH);
        }
        node = dataModel->value<BatteryElemNode *>(2);
        if (node != nullptr) {
            drawItem(painter, node, gapRW, gapRH * 12, gapRW * 7 + gapRW / 2, gapRH * 2);
        }
        node = dataModel->value<BatteryElemNode *>(3);
        if (node != nullptr) {
            drawItem(painter, node, gapRW * 9, gapRH * 12, gapRW * 8, gapRH * 2);
        }
    }

    if (_emic_node != nullptr) {
        FactListModel *dataModel = _emic_node->dataModel();
        assert(dataModel != nullptr);
        for (int i = 0; i < dataModel->count(); i++) {
            ElecSpdCtrlElemNode *node = dataModel->value<ElecSpdCtrlElemNode *>(i);
            int left = gapRW * (i * 2 + 1 + i / 4);
            int widt = gapRW + gapRW / 2;
            drawItem2(painter, node, left, gapRH * 19, widt, gapRH * 2, gapRH * 14, gapRH * 23);
        }
    }

    if (_battery != nullptr) {
        FactListModel *dataModel = _battery->dataModel();
        assert(dataModel != nullptr);
        for (int i = 0; i < dataModel->count(); i++) {
            BatteryElemNode *node = dataModel->value<BatteryElemNode *>(i);
            int left = gapRW * (1 + i / 3) + i * gapRW / 2 + gapRW * (i * 2);
            int top = gapRH * 5 + gapRH / 2;
            drawItem3(painter, node, left, top, gapRW + gapRW / 2, gapRH * 3, gapRH * 4, gapRH * 12,
                      gapRH);
            painter->save();
            painter->setPen(QPen(_color, 1));
            QPainterPath path3;
            path3.moveTo(left + gapRW * 2, gapRH * 2);
            path3.lineTo(left + gapRW * 2, gapRH * 11);
            path3.lineTo(left + gapRW - gapRW / 4, gapRH * 11);
            painter->drawPath(path3);
            QRect connpoint(left + gapRW - gapRW / 4 - 1, gapRH * 11, 2, 2);
            painter->drawEllipse(connpoint);
            painter->restore();
        }
    }

    painter->save();
    painter->setPen(QPen(_color, 1));
    QPainterPath path;
    path.moveTo(gapRW + gapRW / 2 + gapRW / 4, gapRH * 23);
    path.lineTo(gapRW * 18 + gapRW / 2, gapRH * 23);
    path.lineTo(gapRW * 18 + gapRW / 2, gapRH * 2);
    path.moveTo(gapRW + gapRW / 2 + gapRW / 4, gapRH * 4);
    path.lineTo(gapRW * 18 + gapRW / 2, gapRH * 4);
    painter->drawPath(path);
    painter->restore();
}

QuickElectricElem2::QuickElectricElem2(QQuickItem *parent)
    : QuickElectricElem(parent), _generator(nullptr) {}

void QuickElectricElem2::setGenerator(SimpleElemList *name) {
    if (name == nullptr) {
        return;
    }

    _generator = name;
    FactListModel *dataModel = _generator->dataModel();

    if (dataModel != nullptr) {
        for (int ix = 0; ix < dataModel->count(); ix++) {
            BatteryElemNode *node = dataModel->value<BatteryElemNode *>(ix);
            if (node != nullptr && node->valtage() != nullptr) {
                connect(node->valtage(), &FlightDisplay::updateColorValue, this,
                        &QuickElectricElem2::doUpdateQuick2);
            }
        }

        doUpdateQuick2("", 0);
    }
}

void QuickElectricElem2::doUpdateQuick2(QString val, int color) { update(); }

void QuickElectricElem2::drawLine(QPainter *painter, BatteryElemNode *item, QVector<QPoint> &points,
                                  int direct, int diode) {
    assert(item != nullptr);
    if (item == nullptr || painter == nullptr || points.length() <= 0) {
        return;
    }

    int diodeLen = 6;
    int direction = 4;

    QPainterPath path2;
    QPainterPath path3;
    QColor lineColor = getColor(item->lineColor());

    painter->save();
    painter->setPen(QPen(lineColor, 2));
    for (int ix = 0; ix < points.length(); ix++) {
        if (ix == 0) {
            path2.moveTo(points[ix].x(), points[ix].y());
        } else {
            path2.lineTo(points[ix].x(), points[ix].y());
            if ((diode > 0) && (ix + 1 >= points.length())) {
                int x = (points[ix].x() + points[ix - 1].x()) / 2;
                int y = (points[ix].y() + points[ix - 1].y()) / 2;
                path3.moveTo(x, y);
                if (diode == 1) { // down
                    path3.lineTo(x - diodeLen, y - diodeLen);
                    path3.lineTo(x + diodeLen, y - diodeLen);
                    path3.lineTo(x, y);
                    path3.lineTo(x - diodeLen, y);
                    path3.lineTo(x + diodeLen, y);
                } else if (diode == 2) { // up
                    path3.lineTo(x - diodeLen, y + diodeLen);
                    path3.lineTo(x + diodeLen, y + diodeLen);
                    path3.lineTo(x, y);
                    path3.lineTo(x - diodeLen, y);
                    path3.lineTo(x + diodeLen, y);
                } else if (diode == 3) { // left
                    path3.lineTo(x + diodeLen, y - diodeLen);
                    path3.lineTo(x + diodeLen, y + diodeLen);
                    path3.lineTo(x, y);
                    path3.lineTo(x, y - diodeLen);
                    path3.lineTo(x, y + diodeLen);
                } else if (diode == 4) { // right
                    path3.lineTo(x - diodeLen, y - diodeLen);
                    path3.lineTo(x - diodeLen, y + diodeLen);
                    path3.lineTo(x, y);
                    path3.lineTo(x, y - diodeLen);
                    path3.lineTo(x, y + diodeLen);
                }
            }
        }
    }

    if (direct == 0) { // down
        path2.moveTo(points[points.length() - 1].x(), points[points.length() - 1].y());
        path2.lineTo(points[points.length() - 1].x() - direction,
                     points[points.length() - 1].y() - direction);
        path2.lineTo(points[points.length() - 1].x() + direction,
                     points[points.length() - 1].y() - direction);
        path2.lineTo(points[points.length() - 1].x(), points[points.length() - 1].y());
    } else if (direct == 1) { // up
        path2.moveTo(points[points.length() - 1].x(), points[points.length() - 1].y());
        path2.lineTo(points[points.length() - 1].x() - direction,
                     points[points.length() - 1].y() + direction);
        path2.lineTo(points[points.length() - 1].x() + direction,
                     points[points.length() - 1].y() + direction);
        path2.lineTo(points[points.length() - 1].x(), points[points.length() - 1].y());
    } else if (direct == 2) { // left
        path2.moveTo(points[points.length() - 1].x(), points[points.length() - 1].y());
        path2.lineTo(points[points.length() - 1].x() + direction,
                     points[points.length() - 1].y() - direction);
        path2.lineTo(points[points.length() - 1].x() + direction,
                     points[points.length() - 1].y() + direction);
        path2.lineTo(points[points.length() - 1].x(), points[points.length() - 1].y());
    } else if (direct == 3) { // right
        path2.moveTo(points[points.length() - 1].x(), points[points.length() - 1].y());
        path2.lineTo(points[points.length() - 1].x() - direction,
                     points[points.length() - 1].y() - direction);
        path2.lineTo(points[points.length() - 1].x() - direction,
                     points[points.length() - 1].y() + direction);
        path2.lineTo(points[points.length() - 1].x(), points[points.length() - 1].y());
    }
    painter->drawPath(path2);
    painter->drawPath(path3);
    painter->restore();
}

void QuickElectricElem2::drawItem(QPainter *painter, BatteryElemNode *item, int left, int top,
                                  int width, int height, bool state) {
    assert(item != nullptr);
    if (item == nullptr || painter == nullptr) {
        return;
    }

    painter->save();
    painter->setPen(QPen(_fontColor, 1));
    QRect txtRect(left, top, width, height / 4);
    painter->drawText(txtRect, Qt::AlignCenter, item->groupName());
    if (item->capacity() != nullptr) {
        QRect txtRect1 = QRect(left, top + height / 4, width, height / 4);
        QString display = item->capacity()->display() + item->capacity()->unit();
        QString currStrIn = item->capacity()->itemName() + ":" + display;
        painter->drawText(txtRect1, Qt::AlignCenter, currStrIn);
    }
    painter->restore();

    if (item->current() != nullptr) {
        painter->save();
        QColor color1 = getColor(item->current());
        painter->setPen(QPen(color1, 1));
        QRect txtRect2 = QRect(left, top + height / 2, width, height / 4);
        QString display = item->current()->display() + item->current()->unit();
        QString currStr = item->current()->itemName() + ":" + display;
        painter->drawText(txtRect2, Qt::AlignCenter, currStr);
        painter->restore();
    }

    if (item->valtage() != nullptr) {
        painter->save();
        QColor color2 = getColor(item->valtage());
        painter->setPen(QPen(color2, 1));
        QRect txtRect3 = QRect(left, top + height / 2 + height / 4, width, height / 4);
        QString display = item->valtage()->display() + item->valtage()->unit();
        QString valtageStr = item->valtage()->itemName() + ":" + display;
        painter->drawText(txtRect3, Qt::AlignCenter, valtageStr);
        painter->restore();
    }

    if (item->borderColor()) {
        QColor acolor = getColor(item->borderColor());
        painter->save();
        painter->setPen(QPen(acolor, 1));
        QPainterPath path2;
        path2.moveTo(left, top);
        path2.lineTo(left + width, top);
        path2.lineTo(left + width, top + height);
        path2.lineTo(left, top + height);
        path2.lineTo(left, top);
        painter->drawPath(path2);
        painter->restore();
    } else {
        QStringList extlist = item->extrakey();
        int dwidth = width / (extlist.size() * 2 + 1);
        int dheight = height / 4;
        QColor aborder = _warnColor;
        for (int i = 0; i < extlist.size(); i++) {
            QString ext = extlist.at(i);
            FlightDisplay *node = item->serchModule(ext);
            if (node != nullptr) {
                QColor borderExt = getColor(node);
                if (aborder != borderExt) {
                    aborder = borderExt;
                }
                painter->save();
                painter->setPen(QPen(borderExt, 1));
                QPainterPath path2;
                path2.moveTo(left + dwidth + i * 2 * dwidth, top + dheight);
                path2.lineTo(left + 2 * dwidth + i * 2 * dwidth, top + dheight);
                path2.lineTo(left + 2 * dwidth + i * 2 * dwidth, top + 2 * dheight);
                path2.lineTo(left + dwidth + i * 2 * dwidth, top + 2 * dheight);
                path2.lineTo(left + dwidth + i * 2 * dwidth, top + dheight);
                painter->drawPath(path2);
                painter->restore();
            }
        }
        painter->save();
        painter->setPen(QPen(aborder, 1));
        QPainterPath path2;
        path2.moveTo(left, top);
        path2.lineTo(left + width, top);
        path2.lineTo(left + width, top + height);
        path2.lineTo(left, top + height);
        path2.lineTo(left, top);
        painter->drawPath(path2);
        painter->restore();
    }
}

void QuickElectricElem2::paint(QPainter *painter) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
    painter->setPen(QPen(_color, 1));
    QRectF rect = boundingRect();
    int width = rect.width();
    int height = rect.height();
    int gapRH = height / 24;

    if (_generator != nullptr) {
        FactListModel *dataModel = _generator->dataModel();
        assert(dataModel != nullptr);
        int counts = dataModel->count();
        counts = counts > 0 ? (counts + 1) : 2;
        int gapRW = width / counts;
        for (int i = 0; i < dataModel->count(); i++) {
            BatteryElemNode *node = dataModel->value<BatteryElemNode *>(i);
            int left = gapRW / counts * (i + 1) + gapRW * i;
            drawItem(painter, node, left, gapRH, gapRW, gapRH * 4, false);
            QVector<QPoint> linePoint;
            linePoint.append(QPoint(left + gapRW / 2, gapRH * 5));
            if (node->groupName().contains("6000W")) {
                linePoint.append(QPoint(left + gapRW / 2, gapRH * 6));
                linePoint.append(QPoint(left - gapRW / counts - gapRW / 2 - 2, gapRH * 6));
                drawLine(painter, node, linePoint, 2, 0);
            } else if (i + 1 >= dataModel->count()) {
                linePoint.append(QPoint(left + gapRW / 2, gapRH * 8));
                drawLine(painter, node, linePoint, 0, 1);
            } else {
                linePoint.append(QPoint(left + gapRW / 2, gapRH * 8));
                drawLine(painter, node, linePoint, 0, 0);
            }
        }
    }

    if (_emic_node != nullptr) {
        FactListModel *dataModel = _emic_node->dataModel();
        assert(dataModel != nullptr);
        int counts = dataModel->count();
        counts = counts > 0 ? (counts + 1) : 2;
        int gapRW = width / counts;
        for (int i = 0; i < dataModel->count(); i++) {
            BatteryElemNode *node = dataModel->value<BatteryElemNode *>(i);
            drawItem(painter, node, gapRW / counts * (i + 1) + gapRW * i, gapRH * 8, gapRW,
                     gapRH * 4, false);
            if (i + 1 >= dataModel->count()) {
                QVector<QPoint> linePoint;
                linePoint.append(QPoint(gapRW / counts * 2 + gapRW * 2 + gapRW / 2, gapRH * 8));
                linePoint.append(QPoint(gapRW / counts * 2 + gapRW * 2 + gapRW / 2, gapRH * 6));
                linePoint.append(QPoint(gapRW / counts * 2 + gapRW + 4, gapRH * 6));
                drawLine(painter, node, linePoint, 2, 3);
            }
        }

        painter->save();
        painter->setPen(QPen(_color, 1));
        QPainterPath path2;
        path2.moveTo(gapRW / counts + gapRW, gapRH * 10);
        path2.lineTo(gapRW / counts * 2 + gapRW, gapRH * 10);
        painter->drawPath(path2);
        painter->restore();
    }

    if (_battery != nullptr) {
        FactListModel *dataModel = _battery->dataModel();
        assert(dataModel != nullptr);
        int gapRW = width / 4;
        for (int i = 0; i < dataModel->count(); i++) {
            BatteryElemNode *node = dataModel->value<BatteryElemNode *>(i);
            int jump = (i == 1 ? gapRW + gapRW / 4 : 0);
            int left = gapRW / 4 * (i + 1) + gapRW * i + jump;
            drawItem(painter, node, left, gapRH * 15, gapRW, gapRH * 4, true);
            QVector<QPoint> dischargePoint;
            dischargePoint.append(QPoint(left + gapRW / 2, gapRH * 15));
            if (i == 0) {
                dischargePoint.append(QPoint(left + gapRW / 2, gapRH * 14));
                dischargePoint.append(QPoint(left + gapRW + gapRW / 4, gapRH * 14));
                dischargePoint.append(QPoint(left + gapRW + gapRW / 4, gapRH * 12));
                drawLine(painter, node, dischargePoint, 1, 2);
            } else {
                dischargePoint.append(QPoint(left + gapRW / 2, gapRH * 14));
                dischargePoint.append(QPoint(left - gapRW / 4, gapRH * 14));
                dischargePoint.append(QPoint(left - gapRW / 4, gapRH * 12));
                drawLine(painter, node, dischargePoint, 1, 2);
            }
            QVector<QPoint> chargePoint;
            if (i == 0) {
                chargePoint.append(QPoint(left + gapRW + gapRW / 2, gapRH * 12));
                chargePoint.append(QPoint(left + gapRW + gapRW / 2, gapRH * 16));
                chargePoint.append(QPoint(left + gapRW, gapRH * 16));
                drawLine(painter, node, chargePoint, 2, 3);
            } else {
                chargePoint.append(QPoint(left - gapRW / 2, gapRH * 12));
                chargePoint.append(QPoint(left - gapRW / 2, gapRH * 16));
                chargePoint.append(QPoint(left, gapRH * 16));
                drawLine(painter, node, chargePoint, 3, 4);
            }
        }
    }
}

QuickElectricElem::QuickElectricElem(QQuickItem *parent)
    : QQuickPaintedItem(parent), _emic_node(nullptr), _battery(nullptr) {
    _warnColor = QColor(255, 155, 0);
    _whiteColor = QColor(255, 255, 255);
    _color = QColor::fromRgbF(255, 255, 255);
    _fontColor = QColor::fromRgbF(255, 255, 255);
    connect(this, &QuickElectricElem::colorChanged, this, [=] { update(); });
    connect(this, &QuickElectricElem::fontColorChanged, this, [=] { update(); });
}

void QuickElectricElem::doUpdateQuick(QString val, int color) { update(); }

QColor QuickElectricElem::getColor(FlightDisplay *elem) {
    if (elem == nullptr) {
        return _color;
    }
    switch (elem->showColor() & 0xff) {
    case GreenColor:
        return _greenColor;
    case RedColor:
    case OrangeColor:
    case YellowColor:
        return _warnColor;
    case WhiteColor:
        return _whiteColor;
        break;
    default:
        return _color;
        break;
    }
}

void QuickElectricElem::setBattery(SimpleElemList *name) {
    if (name == nullptr) {
        return;
    }

    _battery = name;
    FactListModel *dataModel = _battery->dataModel();

    if (dataModel != nullptr) {
        for (int ix = 0; ix < dataModel->count(); ix++) {
            BatteryElemNode *node = dataModel->value<BatteryElemNode *>(ix);
            if (node != nullptr && node->valtage() != nullptr) {
                connect(node->valtage(), &FlightDisplay::updateColorValue, this,
                        &QuickElectricElem::doUpdateQuick);
            }
        }
    }
}

void QuickElectricElem::setElecNode(SimpleElemList *name) {
    if (name == nullptr) {
        return;
    }

    _emic_node = name;
    FactListModel *dataModel = _emic_node->dataModel();

    if (dataModel != nullptr) {
        for (int ix = 0; ix < dataModel->count(); ix++) {
            BusbarElemNode *node = dataModel->value<BusbarElemNode *>(ix);
            if (node != nullptr && node->valtage() != nullptr) {
                connect(node->valtage(), &FlightDisplay::updateColorValue, this,
                        &QuickElectricElem::doUpdateQuick);
            }
        }
    }
}

void QuickElectricElem::paint(QPainter *painter) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
    painter->setPen(QPen(_color, 1));
    QRectF rect = boundingRect();
    int width = rect.width();
    int height = rect.height();
    int gapRW = width / 9;
    int gapRH = height / 20;
    int offHigh = gapRH * 3;
    int offLR = 20;

    QPainterPath path2;
    path2.moveTo(gapRW + gapRW, gapRH * 8);
    path2.lineTo(gapRW * 3, gapRH * 8);
    path2.moveTo(gapRW * 2, gapRH * 12);
    path2.lineTo(gapRW * 2 + gapRW / 2, gapRH * 12);
    path2.lineTo(gapRW * 2 + gapRW / 2, gapRH * 10);
    /// 电机
    path2.moveTo(gapRW / 2, gapRH * 4);
    path2.lineTo(gapRW / 2, offHigh + gapRW / 2);
    path2.lineTo(gapRW + gapRW / 2, offHigh + gapRW / 2);
    path2.lineTo(gapRW + gapRW / 2, offHigh);
    path2.lineTo(gapRW / 2, offHigh);

    QRect txtRect(gapRW / 2, offHigh, gapRW, gapRW / 2);
    painter->drawText(txtRect, Qt::AlignCenter, QStringLiteral("发电机"));
    path2.addEllipse(gapRW * 2 + gapRW / 2 - gapRW / 4, offHigh, gapRW / 2, gapRW / 2);

    QRect txtRect2 = QRect(gapRW * 2 + gapRW / 2 - gapRW / 4, offHigh, gapRW / 2, gapRW / 2);
    painter->drawText(txtRect2, Qt::AlignCenter, QStringLiteral("并网"));

    path2.moveTo(gapRW + gapRW / 2, offHigh + gapRW / 4);
    path2.lineTo(gapRW * 2 + gapRW / 2 - gapRW / 4, offHigh + gapRW / 4);
    path2.moveTo(gapRW * 2 + gapRW / 2, offHigh + gapRW / 2);
    path2.lineTo(gapRW * 2 + gapRW / 2, gapRH * 10);
    painter->drawPath(path2);
    QPainterPath path4;

    // direction
    path4.moveTo(gapRW * 2 + gapRW / 2, gapRH * 10);
    path4.lineTo(gapRW * 2 + gapRW / 2 - 5, gapRH * 10 + 5);
    path4.lineTo(gapRW * 2 + gapRW / 2 + 5, gapRH * 10 + 5);
    path4.lineTo(gapRW * 2 + gapRW / 2, gapRH * 10);

    path4.moveTo(gapRW * 2, offHigh + gapRW / 4);
    path4.lineTo(gapRW * 2 - 5, offHigh + gapRW / 4 - 5);
    path4.lineTo(gapRW * 2 - 5, offHigh + gapRW / 4 + 5);
    path4.lineTo(gapRW * 2, offHigh + gapRW / 4);
    painter->fillPath(path4, QBrush(_color));
    painter->restore();

    if (_battery != nullptr) {
        FactListModel *dataModel = _battery->dataModel();
        if (dataModel != nullptr) {
            for (int ix = 0; ix < dataModel->count(); ix++) {
                BatteryElemNode *node = dataModel->value<BatteryElemNode *>(ix);
                if (node != nullptr) { /// handle draw value changed message
                    FlightDisplay *valtage = node->valtage();
                    FlightDisplay *current = node->current();
                    FlightDisplay *charge = node->charge();
                    // assert(valtage != nullptr);
                    // assert(current != nullptr);
                    // assert(charge != nullptr);

                    QPen pen;
                    pen.setWidth(1);
                    pen.setColor(valtage && valtage->showColor() ? _warnColor : _color);
                    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
                    painter->save();
                    painter->setPen(pen);

                    QPainterPath path1;
                    path1.moveTo(gapRW / 2, gapRH * (1 + (ix + 1) * 5));
                    path1.lineTo(gapRW / 2, gapRH * (5 + (ix + 1) * 5));
                    path1.lineTo(gapRW * 2, gapRH * (5 + (ix + 1) * 5));
                    path1.lineTo(gapRW * 2, gapRH * (1 + (ix + 1) * 5));
                    path1.lineTo(gapRW / 2, gapRH * (1 + (ix + 1) * 5));
                    painter->drawPath(path1);

                    QRect txtRect2(gapRW / 2, gapRH * (1 + (ix + 1) * 5), gapRW + gapRW / 2, gapRH * 4);
                    QString nameNode = node->groupName();

                    if (charge != nullptr) {
                        nameNode += "\n" + charge->itemName() + ":" + charge->display();
                    }
                    if (current != nullptr) {
                        nameNode += "\n" + current->itemName() + ":" + current->display() + current->unit();
                    }
                    if (valtage != nullptr) {
                        nameNode += "\n" + valtage->itemName() + ":" + valtage->display() + valtage->unit();
                    }

                    painter->drawText(txtRect2, Qt::AlignCenter, nameNode);
                    painter->restore();
                }
            }
        }
    }

    if (_emic_node != nullptr) {
        FactListModel *dataModel = _emic_node->dataModel();
        if (dataModel != nullptr) {
            for (int ix = 0; ix < dataModel->count(); ix++) {
                BusbarElemNode *node = dataModel->value<BusbarElemNode *>(ix);
                if (node != nullptr) { /// handle draw value changed message
                    FlightDisplay *valtage = node->valtage();
                    FlightDisplay *current = node->current();
                    assert(valtage != nullptr);
                    assert(current != nullptr);

                    QPen pen;
                    pen.setWidth(1);
                    pen.setColor(valtage && valtage->showColor() ? _warnColor : _color);
                    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
                    painter->save();
                    painter->setPen(pen);
                    QPainterPath path1;
                    path1.moveTo(gapRW * 3 + offLR, gapRH * (5.0 + ix * 3));
                    path1.lineTo(gapRW * 3 + offLR, gapRH * (7.5 + ix * 3));
                    path1.lineTo(gapRW * 6 - offLR, gapRH * (7.5 + ix * 3));
                    path1.lineTo(gapRW * 6 - offLR, gapRH * (5.0 + ix * 3));
                    path1.lineTo(gapRW * 3 + offLR, gapRH * (5.0 + ix * 3));
                    painter->drawPath(path1);

                    QRect txtRect(gapRW * 3 + offLR, gapRH * (5.0 + ix * 3), gapRW * 3 - 2 * offLR,
                                  gapRH * 2.5);

                    QString nameNode = node->groupName();
                    if (current != nullptr) {
                        nameNode += "\n" + current->itemName() + ":" + current->display() + current->unit();
                    }
                    if (valtage != nullptr) {
                        nameNode += "\n" + valtage->itemName() + ":" + valtage->display() + valtage->unit();
                    }

                    painter->drawText(txtRect, Qt::AlignCenter, nameNode);
                    painter->restore();
                }
            }
        }
    }

    QPen pen;
    pen.setWidth(1);
    pen.setColor(_color);
    pen.setStyle(Qt::DashLine);
    painter->save();
    painter->setPen(pen);
    QPainterPath path3;
    // current & voltage
    path3.moveTo(gapRW * 3, gapRH * 4);
    path3.lineTo(gapRW * 3, gapRH * 16 - gapRH / 2);
    path3.lineTo(gapRW * 6, gapRH * 16 - gapRH / 2);
    path3.lineTo(gapRW * 6, gapRH * 4);
    path3.lineTo(gapRW * 3, gapRH * 4);
    painter->drawPath(path3);
    painter->restore();
}
