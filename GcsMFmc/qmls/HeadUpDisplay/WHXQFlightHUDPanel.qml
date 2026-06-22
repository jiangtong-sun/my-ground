import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickAttitudeElem 1.0
import GcsQuick 1.0

Item {
    id: centerPanel
    property string heightSet: ""
    property string highTypeStr:  ""
    property string airspeedSet:  ""
    property string groundSpeed: ""
    property string heightOffset: ""
    property string trueAirspeedSet: ""
    property string radioHeightStr: ""
    property string yawAngleStr: ""
    property string diffAngleStr: ""
    property string fyawAngleStr: ""

    property var lateralVal: null
    property real yawAngle: 0
    property real airspeed:  0
    property real vehPitch:  0
    property real vehRoll:  0
    property real vehOffset:  0
    property real vehHeight: 0
    property real slideAngle: 0
    property real partWidth: centerPanel.width
    property real partHeight: (centerPanel.height)/32
    property real rightSpace: 0
    property int navColumn: 7
    property int vehSlide: 0
    property real drawHeight: partHeight * 11
    property real dWidth: partWidth / 21
    property real splitWidth: dWidth * 3

    Connections{
        target: commandMgr
        function onSignalDrawing () {
            doRedrawLine()
        }
    }

    function doRedrawLine(){
        heightSet = currVehicle.HeightSet.display + currVehicle.HeightSet.unit
        airspeedSet =  currVehicle.AirspeedSet.display + currVehicle.AirspeedSet.unit
        groundSpeed =  currVehicle.GroundSpeed.itemName + ":" + currVehicle.GroundSpeed.display + currVehicle.GroundSpeed.unit
        highTypeStr = currVehicle.HeightSource.itemName + ":" + currVehicle.HeightSource.display
        heightOffset = currVehicle.HeightOffset.itemName + ":" + currVehicle.HeightOffset.display
        trueAirspeedSet =  currVehicle.TrueAirspeed.itemName + ":" + currVehicle.TrueAirspeed.display
        radioHeightStr =  currVehicle.RadioAltimeter.itemName + ":" + currVehicle.RadioAltimeter.display
        yawAngleStr = currVehicle.YawAngle.itemName + ":" + currVehicle.YawAngle.display
        fyawAngleStr = currVehicle.FlightAngle.itemName + ":" + currVehicle.FlightAngle.display
        diffAngleStr = currVehicle.DifferYaw.itemName + ":" + currVehicle.DifferYaw.display
        lateralVal = currVehicle.LateralOffset
        yawAngle = parseInt(currVehicle.YawAngle.display)
        airspeed = parseInt(currVehicle.Airspeed.display)
        vehPitch = parseFloat(currVehicle.PitchAngle.display)
        vehRoll = parseFloat(currVehicle.RollAngle.display)
        vehHeight = parseInt(currVehicle.Elevation.display)
        vehOffset = parseInt(currVehicle.LateralOffset.display)
        altCanvas.requestPaint();
    }

    Item {
        id: upper
        width: partWidth
        height: partHeight*28
        anchors.topMargin: partHeight
        anchors.leftMargin: rightSpace
        anchors.left: centerPanel.left
        anchors.top: centerPanel.top 

        // QuickAttitudeElem {
        //     id: altCanvas
        //     width: partWidth
        //     height: partHeight*23
        //     anchors.top: parent.top
        //     anchors.topMargin: partHeight*2
        //     strokeColor:drawLineColor
        //     fillStyle:drawLineColor
        //     vehicle: currVehicle
        // }

        Item {
            width: partWidth
            height: partHeight*26
            anchors.top: parent.top
            anchors.topMargin: partHeight*2
            Canvas {
                id: altCanvas
                anchors.fill: parent

                function drawHeadingScale(ctx){
                    ctx.beginPath()
                    var offset = 18;
                    var offWidth = partWidth/2;
                    var gap = dWidth * 2 / 4;
                    for (var im = -10; im <= 10; im += 1){
                        var cur = (im + 360 + yawAngle) % 360;
                        var off = (cur % 10 == 0) ? offset : offset / 2;
                        ctx.moveTo(offWidth + im * gap , partHeight*2-off/2);
                        ctx.lineTo(offWidth + im * gap , partHeight*2+off/2);
                        if (cur % 10 == 0){
                            var dlen = cur.toString().length * 4
                            ctx.fillText(cur.toString(), offWidth + im * gap - dlen, partHeight*2-20);
                        }
                        if (im == 0){
                            ctx.moveTo(offWidth + im * gap , partHeight*2);
                            ctx.lineTo(offWidth + im * gap + 5, partHeight*2+off);
                            ctx.lineTo(offWidth + im * gap - 5, partHeight*2+off);
                            ctx.lineTo(offWidth + im * gap , partHeight*2);
                        }
                    }
                    ctx.fill()
                    ctx.stroke()
                }

                function drawRollScale(ctx) {
                    var angle = [-40, -30, -20, -10, 0, 10, 20, 30, 40];
                    var offxs = [5, 5, 5, 5, 3, 10, 10, 20, 20];
                    var offset = 14;
                    var idx = 0;
                    var hrad = drawHeight;
                    var radius = (dWidth * 9 > hrad) ? hrad : dWidth * 9;
                    var diff = -3.14 / 180;
                    var xcenter = partWidth / 2;
                    var ycenter = drawHeight;
                    ctx.beginPath()
                    ctx.arc(xcenter, ycenter, radius+offset, 3.13/4, 3.14*3/4, false);
                    for (var im = -130; im <= -50; im += 10) {
                        var dff = (im % 10) == 0 ? offset : offset / 3;
                        var ys = Math.sin(diff * im) * radius;
                        var xs = Math.cos(diff * im) * radius;
                        var ye = Math.sin(diff * im) * (radius+dff);
                        var xe = Math.cos(diff * im) * (radius+dff);
                        ctx.moveTo(xcenter + xs, ycenter + ys);
                        ctx.lineTo(xcenter + xe, ycenter + ye);
                        if (im % 10 == 0) {
                            var fsz = angle[idx];
                            ctx.fillText(fsz, xcenter + xs - offxs[idx], ycenter + ys - offset);
                            idx += 1;
                        }
                    }
                    ctx.stroke()
                }

                function drawAirspeedScale(ctx) {
                    var gap = partHeight * 3 / 8;
                    var dlen = 50;
                    var offset = 20;
                    ctx.beginPath()

                    for (var im = -20; im <= 20; im += 1) {
                        var cur = airspeed - im;
                        var off = (cur % 10 == 0) ? offset : offset / 3;
                        ctx.moveTo(splitWidth - off / 2, drawHeight + im * gap);
                        ctx.lineTo(splitWidth + off / 2, drawHeight + im * gap);
                        if (im == 0) {
                            ctx.moveTo(splitWidth + 30, drawHeight + im * gap);
                            ctx.lineTo(splitWidth + 0, drawHeight + im * gap);
                            ctx.lineTo(splitWidth + 30, drawHeight + im * gap - 5);
                            ctx.lineTo(splitWidth + 30, drawHeight + im * gap);
                        }
                        if (cur % 10 == 0) {
                            ctx.fillText(cur, splitWidth - off / 2 - dlen, drawHeight + im * gap + 5, 50, 20);
                        }
                    }
                    ctx.stroke()
                }

                function drawAltitudeScale(ctx) {
                    ctx.beginPath()
                    var gap = partHeight * 3 / 8;
                    var dlen = 50;
                    var offset = 20;
                    for (var im = -20; im <= 20; im += 1) {
                        var cur = vehHeight - im;
                        var curhigh = drawHeight + im * gap;
                        var off = (cur % 10 == 0) ? offset : offset / 3;
                        ctx.moveTo(partWidth - splitWidth - off / 2, curhigh);
                        ctx.lineTo(partWidth - splitWidth + off / 2, curhigh);
                        if (im == 0) {
                            ctx.moveTo(partWidth - splitWidth - 30, curhigh);
                            ctx.lineTo(partWidth - splitWidth - 0, curhigh);
                            ctx.lineTo(partWidth - splitWidth - 30, curhigh - 5);
                            ctx.lineTo(partWidth - splitWidth - 30, curhigh);
                        }
                        if (cur % 10 == 0) {
                            ctx.fillText(cur, partWidth - splitWidth + off, curhigh + 5, 50, 20);
                        }
                    }
                    ctx.stroke()
                }

                function drawSlideOffsetScale(ctx) {

                    var gap = dWidth * 13 / 10;
                    var vspace = dWidth * 4;
                    var hloc = partHeight * 25;
                    var offset = 16;
                    var idx = 0;
                    ctx.beginPath()
                    for (var im = -25; im <= 25; im += 5) {
                        var off = (im % 10 == 0) ? offset : offset / 2;
                        ctx.moveTo(vspace + idx * gap, hloc);
                        ctx.lineTo(vspace + idx * gap, hloc + off);
                        if ((im % 10) == 0) {
                            ctx.fillText(im, vspace + idx * gap - 10, hloc + 30, 20, 20);
                        }
                        idx += 1;
                    }
                    ctx.moveTo(vspace - dWidth / 2, hloc);
                    ctx.lineTo(vspace + dWidth * 13 + dWidth / 2, hloc);
                    if (vehOffset > -25 && vehOffset < 25) {
                        gap = dWidth * 13 / 50;
                        var xpos = (vehOffset + 25) * gap + vspace;
                        ctx.moveTo(xpos, hloc);
                        ctx.lineTo(xpos - 10, hloc + 16);
                        ctx.lineTo(xpos + 10, hloc + 16);
                        ctx.lineTo(xpos, hloc);
                    }
                    ctx.stroke()
                }

                function pitchAngleDraw(ctx, pitch, lower, upper, cpart) {
                    for (var im2 = lower; im2 <= upper; im2 += 1) {
                        if (im2 % 5 != 0) {
                            continue;
                        }
                        var tm2 = ((im2 % 10) == 0) ? 1 : 0;
                        var dcenter2 = (pitch - im2) * cpart;
                        ctx.moveTo(-(2.5 + tm2) * dWidth, dcenter2);
                        ctx.lineTo(-1.5 * dWidth, dcenter2);
                        ctx.moveTo(1.5 * dWidth, dcenter2);
                        ctx.lineTo((2.5 + tm2) * dWidth, dcenter2);
                        if (tm2 === 1) {
                            ctx.fillText(im2, -15, dcenter2 - 15, 30, 30);
                        }
                    }
                }

                function drawPitchAngleScale(ctx) {
                    ctx.save();
                    ctx.beginPath()
                    ctx.strokeStyle = "#e0900a";
                    ctx.translate(partWidth / 2, drawHeight);
                    ctx.moveTo(-dWidth, 0);
                    ctx.lineTo(-dWidth / 2, 0);
                    ctx.lineTo(-dWidth / 4, 10);
                    ctx.lineTo(0, 0);
                    ctx.lineTo(dWidth / 4, 10);
                    ctx.lineTo(dWidth / 2, 0);
                    ctx.lineTo(dWidth, 0);
                    ctx.stroke()
                    ctx.restore();

                    ctx.save();
                    ctx.beginPath()
                    ctx.strokeStyle = drawLineColor;
                    ctx.translate(partWidth / 2, drawHeight);
                    ctx.rotate(vehRoll * Math.PI / 180);
                    var hrad = drawHeight;
                    var curHigh = ((dWidth * 9 > hrad) ? hrad : dWidth * 9);
                    if (vehRoll > -40 && vehRoll < 40) {
                        // ctx.moveTo(0, curHigh + 26);
                        // ctx.lineTo(-10, curHigh + 14);
                        // ctx.lineTo(+10, curHigh + 14);
                        // ctx.lineTo(0, curHigh + 26);
                        ctx.moveTo(0, curHigh + 20);
                        ctx.lineTo(-10, curHigh + 34);
                        ctx.lineTo(+10, curHigh + 34);
                        ctx.lineTo(0, curHigh + 20);
                        // var val = vehSlide < 0 ? 14 : 0;
                        // ctx.moveTo(-val + 0, curHigh + 46);
                        // ctx.lineTo(-val + 0, curHigh + 50);
                        // ctx.lineTo(-val + 14, curHigh + 50);
                        // ctx.lineTo(-val + 14, curHigh + 46);
                        // ctx.lineTo(-val + 0, curHigh + 46);
                    }
                    ctx.fill()
                    ctx.stroke()
                    ctx.restore();

                    ctx.save();
                    ctx.beginPath()
                    ctx.translate(partWidth / 2, drawHeight);
                    ctx.rotate(vehRoll * Math.PI / 180);
                    var lower = Math.ceil(vehPitch - 20);
                    var upper = Math.ceil(vehPitch + 20);
                    var cpart = partHeight / 4;
                    if (upper < 0) {
                        pitchAngleDraw(ctx, vehPitch, lower, upper, cpart);
                    } else if (lower > 0) {
                        ctx.strokeStyle = drawLineColor;
                        pitchAngleDraw(ctx, vehPitch, lower, upper, cpart);
                    } else {
                        ctx.strokeStyle = drawLineColor;
                        pitchAngleDraw(ctx, vehPitch, 0, upper, cpart);
                        ctx.stroke()
                        ctx.restore();

                        ctx.save();
                        ctx.beginPath()
                        ctx.translate(partWidth / 2, drawHeight);
                        ctx.rotate(vehRoll * Math.PI / 180);
                        ctx.strokeStyle = "#e0900a";
                        pitchAngleDraw(ctx, vehPitch, lower, 0, cpart);
                    }
                    ctx.stroke()
                    ctx.restore();
                }

                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, partWidth, partHeight*20);
                    ctx.font = frontName;
                    ctx.lineWidth = 2;
                    ctx.strokeStyle = drawLineColor;
                    ctx.fillStyle = drawLineColor;

                    ctx.save();
                    drawHeadingScale(ctx);
                    drawRollScale(ctx);
                    drawAirspeedScale(ctx)
                    drawAltitudeScale(ctx)
                    drawSlideOffsetScale(ctx)
                    ctx.fill()
                    ctx.restore();
                    drawPitchAngleScale(ctx)
                }
            }
        }

        GcsButton {
            radiusVal: 5
            text: airspeedSet
            width: dWidth*3
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 3
            anchors.left: upper.left
            anchors.leftMargin: dWidth/2
            textColor: GcsTheme.setvalueColor
        }

        GcsButton {
            radiusVal: 5
            text: heightSet
            width: dWidth*3
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 3
            anchors.right: upper.right
            anchors.rightMargin: dWidth/2
            textColor: GcsTheme.setvalueColor
        }

        GcsButton {
            radiusVal: 5
            text: highTypeStr
            width: dWidth*3
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 19
            anchors.right: upper.right
            anchors.rightMargin: dWidth/2
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: trueAirspeedSet
            width: dWidth*3
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 19
            anchors.left: upper.left
            anchors.leftMargin: dWidth/2
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: groundSpeed
            width: dWidth*3
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 20
            anchors.left: upper.left
            anchors.leftMargin: dWidth/2
            textColor: GcsTheme.primaryColor
        }


        // GcsButton {
        //     radiusVal: 5
        //     text: fyawAngleStr
        //     width: dWidth*3
        //     height: partHeight
        //     anchors.top: upper.top
        //     anchors.topMargin: partHeight * 20
        //     anchors.right: upper.right
        //     anchors.rightMargin: partWidth/2-dWidth*3-dWidth*3/2
        //     textColor: GcsTheme.primaryColor
        // }

        // GcsButton {
        //     radiusVal: 5
        //     text: yawAngleStr
        //     width: dWidth*3
        //     height: partHeight
        //     anchors.top: upper.top
        //     anchors.topMargin: partHeight * 20
        //     anchors.left: upper.left
        //     anchors.leftMargin: partWidth/2-dWidth*3-dWidth*3/2
        //     textColor: GcsTheme.primaryColor
        // }

        // GcsButton {
        //     radiusVal: 5
        //     text: diffAngleStr
        //     width: dWidth*3
        //     height: partHeight
        //     anchors.top: upper.top
        //     anchors.topMargin: partHeight * 20
        //     anchors.right: upper.right
        //     anchors.rightMargin: partWidth/2-dWidth*3/2
        //     textColor: GcsTheme.primaryColor
        // }

        GcsButton {
            radiusVal: 5
            text: radioHeightStr
            width: dWidth*3
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 20
            anchors.right: upper.right
            anchors.rightMargin: dWidth/2
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: heightOffset
            width: dWidth*3
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 21
            anchors.right: upper.right
            anchors.rightMargin: dWidth/2
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: lateralVal?lateralVal.display + lateralVal.unit : ""
            width: dWidth*3
            height: partHeight-5
            anchors.top: upper.top
            anchors.topMargin: partHeight * 26
            anchors.right: upper.right
            anchors.rightMargin: partWidth/2-dWidth-dWidth/2
            textColor: GcsTheme.primaryColor
        }

        GcsText {
            width: dWidth+20
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: drawHeight+partHeight*2+5
            anchors.left: upper.left
            anchors.leftMargin: splitWidth
            text: airspeed
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        GcsText {
            text: vehHeight
            width: dWidth+20
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: drawHeight+partHeight*2+5
            anchors.right: upper.right
            anchors.rightMargin: splitWidth
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}
