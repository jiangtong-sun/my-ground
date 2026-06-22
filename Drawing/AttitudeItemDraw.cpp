#pragma once

#include "AttitudeItemDraw.h"
#include "chchar.h"

#include <QPainterPath>
#include <QQuickPaintedItem>
#include <math.h>
#include <qpainter.h>

QuickAttitudeElem::QuickAttitudeElem(QQuickItem *parent) : QQuickPaintedItem(parent) {
    _fillStyle = QColor::fromRgbF(255, 255, 255);
    _strokeColor = QColor::fromRgbF(255, 255, 255);
    connect(this, &QuickAttitudeElem::fillStyleChanged, this, [=] { update(); });
    connect(this, &QuickAttitudeElem::strokeColorChanged, this, [=] { update(); });
}

void QuickAttitudeElem::updateModel(FlightVechicle *vehicle) {
    curr_veh = vehicle;
    if (curr_veh != nullptr) {
        connect(curr_veh->Airspeed(), &FlightDisplay::updateColorValue, this, [=](QString val, int color){
            veh_speed = curr_veh->Airspeed()->display().toFloat();
            update();
        });
        connect(curr_veh->RollAngle(), &FlightDisplay::updateColorValue, this, [=] (QString val, int color){
            veh_roll = -curr_veh->RollAngle()->display().toFloat();
            update();
        });
        connect(curr_veh->Elevation(), &FlightDisplay::updateColorValue, this, [=] (QString val, int color){
            veh_height = curr_veh->Elevation()->display().toFloat();
            update();
        });
        connect(curr_veh->PitchAngle(), &FlightDisplay::updateColorValue, this, [=] (QString val, int color){
            veh_pitch = curr_veh->PitchAngle()->display().toFloat();
            update();
        });
        connect(curr_veh->SlideAngle(), &FlightDisplay::updateColorValue, this, [=] (QString val, int color){
            veh_slide = curr_veh->SlideAngle()->display().toFloat();
            update();
        });
        connect(curr_veh->LateralOffset(), &FlightDisplay::updateColorValue, this, [=] (QString val, int color){
            veh_offset = curr_veh->LateralOffset()->display().toFloat();
            update();
        });
    }
}

void QuickAttitudeElem::drawRollScale(QPainter *painter) {

    int angle[] = {-40, -30, -20, -10, 0, 10, 20, 30, 40};
    int offxs[] = {20, 20, 20, 20, 10, 10, 8, 6, 6};
    int offset = 10;
    int idx = 0;
    float hrad = drawHeight - partHeight;
    float radius = (dWidth * 8 > hrad) ? hrad : dWidth * 8;
    float diff = -3.14 / 180;
    float xcenter = partWidth / 2;
    float ycenter = drawHeight;
    QPainterPath path4;
    for (int im = -130; im <= -50; im += 10) {
        float dff = (im % 10) == 0 ? offset : offset / 3;
        float ys = std::sin(diff * im) * radius;
        float xs = std::cos(diff * im) * radius;
        float ye = std::sin(diff * im) * (radius - dff);
        float xe = std::cos(diff * im) * (radius - dff);
        path4.moveTo(xcenter + xs, ycenter - ys);
        path4.lineTo(xcenter + xe, ycenter - ye);
        if (im % 10 == 0) {
            QString fsz = QString::number(-angle[idx]);
            QRect txtRect(xcenter + xs - offxs[idx], ycenter - ys - 20, 20, 20);
            painter->drawText(txtRect, Qt::AlignCenter, fsz);
            idx += 1;
        }
    }

    painter->drawPath(path4);
}

void QuickAttitudeElem::drawAirspeedScale(QPainter *painter) {

    float gap = partHeight * 3 / 10;
    float dlen = 50;
    int offset = 16;
    QPainterPath path4;
    QPainterPath path3;
    for (int im = -20; im <= 20; im += 1) {
        int cur = veh_speed - im;
        float off = (cur % 10 == 0) ? offset : offset / 3;
        path4.moveTo(splitWidth - off / 2, drawHeight + im * gap);
        path4.lineTo(splitWidth + off / 2, drawHeight + im * gap);
        if (im == 0) {
            path3.moveTo(splitWidth + 30, drawHeight + im * gap);
            path3.lineTo(splitWidth + 0, drawHeight + im * gap);
            path3.lineTo(splitWidth + 30, drawHeight + im * gap - 5);
            path3.lineTo(splitWidth + 30, drawHeight + im * gap);
        }
        if (cur % 10 == 0) {
            QRect txtRect(splitWidth - off / 2 - dlen, drawHeight + im * gap - 10, 50, 20);
            painter->drawText(txtRect, Qt::AlignCenter, QString::number(cur));
        }
    }

    painter->fillPath(path3, _fillStyle);
    painter->drawPath(path4);
}

void QuickAttitudeElem::drawAltitudeScale(QPainter *painter) {
    float gap = partHeight * 3 / 10;
    float dlen = 50;
    int offset = 16;
    QPainterPath path4;
    QPainterPath path3;
    for (int im = -20; im <= 20; im += 1) {
        int cur = static_cast<int>(veh_height) - im;
        float curhigh = drawHeight + im * gap;
        float off = (cur % 10 == 0) ? offset : offset / 3;
        path4.moveTo(partWidth - splitWidth - off / 2, curhigh);
        path4.lineTo(partWidth - splitWidth + off / 2, curhigh);
        if (im == 0) {
            path3.moveTo(partWidth - splitWidth - 30, curhigh);
            path3.lineTo(partWidth - splitWidth - 0, curhigh);
            path3.lineTo(partWidth - splitWidth - 30, curhigh - 5);
            path3.lineTo(partWidth - splitWidth - 30, curhigh);
        }
        if (cur % 10 == 0) {
            QRect txtRect(partWidth - splitWidth + off, curhigh - 10, 50, 20);
            painter->drawText(txtRect, Qt::AlignCenter, QString::number(cur));
        }
    }
    painter->drawPath(path4);
    painter->fillPath(path3, _fillStyle);
}

void QuickAttitudeElem::drawSlideOffsetScale(QPainter *painter) {
    float gap = dWidth * 13 / 10;
    float vspace = dWidth * 4;
    int hloc = partHeight * 18;
    int offset = 16;
    int idx = 0;

    QPainterPath path4;
    for (int im = -25; im <= 25; im += 5) {
        int off = (im % 10 == 0) ? offset : offset / 2;
        path4.moveTo(vspace + idx * gap, hloc);
        path4.lineTo(vspace + idx * gap, hloc + off);
        if ((im % 10) == 0) {
            QRect txtRect(vspace + idx * gap - 10, hloc + off, 20, 20);
            painter->drawText(txtRect, Qt::AlignCenter, QString::number(im));
        }
        idx += 1;
    }
    path4.moveTo(vspace - dWidth / 2, hloc);
    path4.lineTo(vspace + dWidth * 13 + dWidth / 2, hloc);
    painter->drawPath(path4);

    if (veh_offset > -25 && veh_offset < 25) {
        gap = dWidth * 13 / 50;
        float xpos = (veh_offset + 25) * gap + vspace;
        QPainterPath path3;
        path3.moveTo(xpos, hloc);
        path3.lineTo(xpos - 10, hloc - 16);
        path3.lineTo(xpos + 10, hloc - 16);
        path3.lineTo(xpos, hloc);
        painter->fillPath(path3, _fillStyle);
    }
}

void QuickAttitudeElem::pitchAngleDraw(QPainter *painter, int pitch, int lower, int upper,
                                       float cpart) {
    QPainterPath path4;
    for (int im2 = lower; im2 <= upper; im2 += 1) {
        if (im2 % 5 != 0) {
            continue;
        }
        int tm2 = ((im2 % 10) == 0) ? 1 : 0;
        float dcenter2 = (pitch - im2) * cpart;
        path4.moveTo(-(2.5 + tm2) * dWidth, dcenter2);
        path4.lineTo(-1.5 * dWidth, dcenter2);
        path4.moveTo(1.5 * dWidth, dcenter2);
        path4.lineTo((2.5 + tm2) * dWidth, dcenter2);
        if (tm2 == 1) {
            QRect txtRect(-15, dcenter2 - 15, 30, 30);
            painter->drawText(txtRect, Qt::AlignCenter, QString::number(im2));
        }
    }
    painter->drawPath(path4);
}

void QuickAttitudeElem::drawPitchAngleScale(QPainter *painter) {
    painter->save();
    QPainterPath path4;
    painter->setPen(QPen(QColor(220, 150, 10), 2));
    painter->translate(partWidth / 2, drawHeight);
    path4.moveTo(-dWidth, 0);
    path4.lineTo(-dWidth / 2, 0);
    path4.lineTo(-dWidth / 4, 10);
    path4.lineTo(0, 0);
    path4.lineTo(dWidth / 4, 10);
    path4.lineTo(dWidth / 2, 0);
    path4.lineTo(dWidth, 0);
    painter->drawPath(path4);
    painter->restore();
    painter->save();
    painter->translate(partWidth / 2, drawHeight);
    painter->rotate(veh_roll);
    QPainterPath path3;
    float hrad = drawHeight - partHeight;
    int curHigh = -((dWidth * 8 > hrad) ? hrad : dWidth * 8);
    if (veh_roll > -40 && veh_roll < 40) {
        path3.moveTo(0, curHigh + 26);
        path3.lineTo(-10, curHigh + 14);
        path3.lineTo(+10, curHigh + 14);
        path3.lineTo(0, curHigh + 26);
        path3.moveTo(0, curHigh + 30);
        path3.lineTo(-10, curHigh + 44);
        path3.lineTo(+10, curHigh + 44);
        path3.lineTo(0, curHigh + 30);
        int val = veh_slide < 0 ? 14 : 0;
        path3.moveTo(-val + 0, curHigh + 46);
        path3.lineTo(-val + 0, curHigh + 50);
        path3.lineTo(-val + 14, curHigh + 50);
        path3.lineTo(-val + 14, curHigh + 46);
        path3.lineTo(-val + 0, curHigh + 46);
    }
    painter->fillPath(path3, _fillStyle);
    int lower = std::ceil(veh_pitch - 20);
    int upper = std::ceil(veh_pitch + 20);
    float cpart = partHeight / 4;
    if (upper < 0) {
        painter->setPen(QPen(QColor(220, 150, 10), 2));
        pitchAngleDraw(painter, veh_pitch, lower, upper, cpart);
    } else if (lower > 0) {
        painter->setPen(QPen(_strokeColor, 2));
        pitchAngleDraw(painter, veh_pitch, lower, upper, cpart);
    } else {
        painter->setPen(QPen(_strokeColor, 2));
        pitchAngleDraw(painter, veh_pitch, 0, upper, cpart);
        painter->setPen(QPen(QColor(220, 150, 10), 2));
        pitchAngleDraw(painter, veh_pitch, lower, 0, cpart);
    }
    painter->restore();
}

void QuickAttitudeElem::paint(QPainter *painter) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setFont(QFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, 1));
    painter->setPen(QPen(_strokeColor, 2));
    QRectF rect = boundingRect();
    partWidth = rect.width();
    partHeight = rect.height() / 20;
    drawHeight = partHeight * 9;
    dWidth = partWidth / 21;
    splitWidth = dWidth * 3;
    painter->save();
    drawRollScale(painter);
    drawAirspeedScale(painter);
    drawAltitudeScale(painter);
    drawSlideOffsetScale(painter);
    painter->restore();
    drawPitchAngleScale(painter);
}
