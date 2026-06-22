#pragma once

#include "ActuatorItemDraw.h"

#include <QPainterPath>
#include <QQuickPaintedItem>
#include <qpainter.h>

void QuickActuatorElem::setData(FlightVechicle *name) { _dataNode = name; }

QuickActuatorElem::QuickActuatorElem(QQuickItem *parent) : QQuickPaintedItem(parent) {
    _color = QColor::fromRgbF(255, 255, 255);
    connect(this, &QuickActuatorElem::colorChanged, this, [=] { update(); });
}

void QuickActuatorElem::doUpdateQuick() { update(); }

void QuickActuatorElem::paint(QPainter *painter) {
    QPen pen;
    pen.setWidth(1);
    pen.setColor(_color);
    // pen.setStyle(Qt::DashLine);

    FlightDisplay *frontWheel = _dataNode ? _dataNode->FrontWheelAngle() : nullptr;
    FlightDisplay *leftWheel = _dataNode ? _dataNode->LeftBrake() : nullptr;
    FlightDisplay *rightWheel = _dataNode ? _dataNode->RightBrake() : nullptr;
    FlightDisplay *leftInFlap = nullptr;     //襟翼
    FlightDisplay *rightInFlap = nullptr;    //襟翼
    FlightDisplay *leftOutFlap = nullptr;    //襟翼
    FlightDisplay *rightOutFlap = nullptr;   //襟翼
    FlightDisplay *leftAilerons = nullptr;   //副翼
    FlightDisplay *rightAilerons = nullptr;  //副翼
    FlightDisplay *leftElevators = nullptr;  //升降
    FlightDisplay *rightElevators = nullptr; //升降
    FlightDisplay *frontRudder1 = nullptr;   //方向
    FlightDisplay *frontRudder2 = nullptr;   //方向
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(pen);
    QRectF rect = boundingRect();
    int width = rect.width() / 12;
    int height = rect.height() / 21;
    int cWidth = rect.width() / 2;
    painter->save();
    QPainterPath path;
    path.addRect(cWidth - width * 4 - height / 2, height * 9, width + height / 2, height * 2);
    QRect txtRect(cWidth - width * 4 - height / 2, height * 11, width + height / 2, height);
    painter->drawText(txtRect, Qt::AlignCenter, leftOutFlap ? leftOutFlap->display() : "");

    path.addRect(cWidth - width * 3, height * 9, width, height * 2);
    txtRect = QRect(cWidth - width * 3, height * 11, width, height);
    painter->drawText(txtRect, Qt::AlignCenter, leftInFlap ? leftInFlap->display() : "");

    path.addRect(cWidth + width * 2, height * 9, width, height * 2);
    txtRect = QRect(cWidth + width * 2, height * 11, width, height);
    painter->drawText(txtRect, Qt::AlignCenter, rightInFlap ? rightInFlap->display() : "");

    path.addRect(cWidth + width * 3, height * 9, width + height / 2, height * 2);
    txtRect = QRect(cWidth + width * 3, height * 11, width + height / 2, height);
    painter->drawText(txtRect, Qt::AlignCenter, rightOutFlap ? rightOutFlap->display() : "");

    path.addRect(cWidth, height * 19, width * 3 / 2, height);
    txtRect = QRect(cWidth, height * 19, width * 3 / 2, height);
    painter->drawText(txtRect, Qt::AlignCenter, leftElevators ? leftElevators->display() : "");

    path.addRect(cWidth - width * 3 / 2, height * 19, width * 3 / 2, height);
    txtRect = QRect(cWidth - width * 3 / 2, height * 19, width * 3 / 2, height);
    painter->drawText(txtRect, Qt::AlignCenter, rightElevators ? rightElevators->display() : "");

    path.addRect(cWidth - height - height / 2, height * 10, height / 2, height * 2);
    txtRect = QRect(cWidth - width - height / 2, height * 10 + height / 2, width, height);
    painter->drawText(txtRect, Qt::AlignLeft, leftWheel ? leftWheel->display() : "");

    path.addRect(cWidth + height, height * 10, height / 2, height * 2);
    txtRect = QRect(cWidth + height / 2, height * 10 + height / 2, width, height);
    painter->drawText(txtRect, Qt::AlignRight, rightWheel ? rightWheel->display() : "");

    path.moveTo(cWidth - width * 3 / 2, height * 19);
    path.cubicTo(cWidth - width * 3 / 2, height * 19, cWidth - height, height * 18, cWidth - height,
                 height * 17);
    path.cubicTo(cWidth - height, height * 17, cWidth - width, height * 12, cWidth - width - height,
                 height * 11);
    path.cubicTo(cWidth - width - height, height * 11, cWidth - width - height, height * 11,
                 cWidth - width * 4 - height / 2, height * 11);
    path.cubicTo(cWidth - width * 4 - height / 2, height * 11, cWidth - 5 * width, height * 10,
                 cWidth - 5 * width, height * 9);
    path.cubicTo(cWidth - 5 * width, height * 9, cWidth - 5 * width, height * 9, cWidth - width,
                 height * 9);
    path.cubicTo(cWidth - width, height * 9, cWidth - width / 4 - 10, height * 6, cWidth,
                 height * 6);
    path.cubicTo(cWidth, height * 6, cWidth + width / 4 + 10, height * 6, cWidth + width,
                 height * 9);
    path.cubicTo(cWidth + width, height * 9, cWidth + 5 * width, height * 9, cWidth + 5 * width,
                 height * 9);
    path.cubicTo(cWidth + 5 * width, height * 9, cWidth + 5 * width, height * 10,
                 cWidth + 4 * width + height / 2, height * 11);
    path.cubicTo(cWidth + 4 * width + height / 2, height * 11, cWidth + width, height * 11,
                 cWidth + width + height, height * 11);
    path.cubicTo(cWidth + width + height - 4, height * 11, cWidth + width, height * 12,
                 cWidth + height, height * 17);
    path.cubicTo(cWidth + height, height * 17, cWidth + height, height * 18, cWidth + width * 3 / 2,
                 height * 19);

    QPainterPath path4;
    int leftHeight = (leftWheel ? leftWheel->display().toFloat() : 0) * height * 2;
    int rightHeight = (rightWheel ? rightWheel->display().toFloat() : 0) * height * 2;
    path4.addRect(cWidth - height - height / 2, height * 12 - leftHeight, height / 2, leftHeight);
    path4.addRect(cWidth + height, height * 12 - rightHeight, height / 2, rightHeight);

    ///襟翼动态显示
    int leftOutboard = (leftOutFlap ? leftOutFlap->display().toFloat() : 0) * height;
    int leftInboard = (leftInFlap ? leftInFlap->display().toFloat() : 0) * height;
    int rightInboard = (rightInFlap ? rightInFlap->display().toFloat() : 0) * height;
    int rightOutboard = (rightOutFlap ? rightOutFlap->display().toFloat() : 0) * height;
    path4.addRect(cWidth - width * 4 - height / 2,
                  height * 10 + (leftOutboard > 0 ? 0 : leftOutboard), width + height / 2,
                  abs(leftOutboard));
    path4.addRect(cWidth - width * 3, height * 10 + (leftInboard > 0 ? 0 : leftInboard), width,
                  abs(leftInboard));
    path4.addRect(cWidth + width * 2, height * 10 + (rightInboard > 0 ? 0 : rightInboard), width,
                  abs(rightInboard));
    path4.addRect(cWidth + width * 3, height * 10 + (rightOutboard > 0 ? 0 : rightOutboard),
                  width + height / 2, abs(rightOutboard));

    path4.moveTo(cWidth, height * 5);
    path4.lineTo(cWidth, height * 9);
    txtRect = QRect(cWidth - width / 2, height * 8 + height / 2, width, height);
    painter->drawText(txtRect, Qt::AlignCenter, frontWheel ? frontWheel->display() : "");
    float curRudder = (frontRudder1 ? frontRudder1->display().toFloat() : 0) * 3 * 3.14 / 180;
    float xyoffs = width / 2 * std::sin(curRudder);
    float xyoffc = width / 2 * std::cos(curRudder);
    txtRect = QRect(cWidth - width / 2, height * 17 - height / 2, width, height);
    painter->drawText(txtRect, Qt::AlignCenter, frontRudder1 ? frontRudder1->display() : "");
    path4.moveTo(cWidth - 2, height * 18 - height / 2);
    path4.lineTo(cWidth + xyoffs, height * 18 - height / 2 + xyoffc);
    path4.lineTo(cWidth + 2, height * 18 - height / 2);
    path4.lineTo(cWidth - 2, height * 18 - height / 2);

    painter->fillPath(path4, QBrush(_color));
    painter->drawPath(path4);
    path.moveTo(cWidth - width / 2 * std::sin(3.14 / 4),
                height * 18 - height / 2 + width / 2 * std::sin(3.14 / 4));
    path.arcTo(cWidth - width / 2, height * 19 - height / 2 - width, width, width, -135, 90);
    // QFontMetrics fm = painter->fontMetrics();
    // QString text = QString(tr("-15°"));
    // int pixelHigh = fm.height();
    // txtRect = QRect(cWidth - width / 2 * std::sin(3.14 / 4) - width,
    //                 height * 18 - pixelHigh + width / 2 * std::sin(3.14 / 4), width, height);
    // painter->drawText(txtRect, Qt::AlignRight, "-15°");
    // txtRect = QRect(cWidth + width / 2 * std::sin(3.14 / 4) + 3,
    //                 height * 18 - pixelHigh + width / 2 * std::sin(3.14 / 4), width, height);
    // painter->drawText(txtRect, Qt::AlignLeft, "15°");
    path.moveTo(cWidth + width * 4 + height / 2, height * 10);
    path.lineTo(cWidth + 5 * width, height * 9);
    path.moveTo(cWidth - width * 4 - height / 2, height * 10);
    path.lineTo(cWidth - 5 * width, height * 9);
    painter->drawPath(path);
    painter->restore();
    painter->save();
    QPainterPath pathFront;
    painter->resetTransform();
    painter->translate(cWidth, height * 8);
    painter->rotate(frontWheel ? frontWheel->display().toFloat() : 0);
    pathFront.addEllipse(-height / 4, -height / 2, height / 2, height);
    path.addRect(cWidth + width * 3, height * 9, width + height / 2, height * 2);
    painter->fillPath(pathFront, QBrush(_color));
    painter->drawPath(pathFront);
    painter->restore();
}
