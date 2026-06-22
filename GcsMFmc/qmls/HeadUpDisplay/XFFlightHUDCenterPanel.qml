import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickAttitudeElem 1.0
import GcsQuick 1.0

Item {
    id: centerPanel
    property string heightSet: ""
    property string heightSetVal: ""
    property string highTypeStr:  ""
    property string highTypeStrVal: ""
    property string airspeedSet:  ""
    property string airspeedSetVal: ""
    property string groundSpeed: ""
    property string groundSpeedVal: ""
    property string heightOffset: ""
    property string trueAirspeedSet: ""
    property string trueAirspeedSetVal: ""
    property string radioHeightStr: ""
    property string radioHeightStrVal: ""
    property string vehHeightStr: ""
    property real vehTrail:  0
    property real climbslide: 0
    property real heightOffsetVal: 0
    property real yawAngle: 0
    property real airspeed:  0
    property real vehPitch:  0
    property real vehRoll:  0
    property real vehOffset:  0
    property real vehHeight: 0
    property real slideAngle: 0
    property real vSpeed: 0
    property real rightSpace: 0
    property int navColumn: 7
    property int vehSlide: 0

    property real partWidth: centerPanel.width
    property real partHeight: (centerPanel.height)/32
    property real drawHeight: partHeight * 11
    property real dWidth: partWidth / 21
    property real splitWidth: dWidth * 4

    Connections{
        target: commandMgr
        function onSignalDrawing () {
            doRedrawLine()
        }
    }

    FontMetrics {
        id: fontMetrics
        font.family: TextStyle.family
        font.pixelSize: TextStyle.Body
    }

    function formatFloatValue(value) {
        let str = value.toFixed(1);
        if (str.length >= 5 ) return str;
        return " ".repeat(5 - str.length) + str;
    }

    function doRedrawLine(){
        if (heightSet.length <= 0) {
            heightSet = currVehicle.HeightSet.itemName
        }

        if (airspeedSet.length <= 0) {
            airspeedSet = currVehicle.AirspeedSet.itemName
        }

        if (groundSpeed.length <= 0) {
            groundSpeed =  currVehicle.GroundSpeed.itemName
        }

        if (highTypeStr.length <= 0) {
            highTypeStr = currVehicle.HeightSource.itemName
        }

        if (heightOffset.length <= 0) {
            heightOffset = currVehicle.HeightOffset.itemName
        }

        if (trueAirspeedSet.length <= 0) {
            trueAirspeedSet =  currVehicle.TrueAirspeed.itemName
        }

        if (radioHeightStr.length <= 0) {
            radioHeightStr =  currVehicle.RadioAltimeter.itemName
        }

        if (vehHeightStr.length <= 0) {
            vehHeightStr = currVehicle.Elevation.itemName
        }

        heightSetVal = currVehicle.HeightSet.display
        highTypeStrVal = currVehicle.HeightSource.display
        radioHeightStrVal =  currVehicle.RadioAltimeter.display

        var spd1 = parseFloat(currVehicle.AirspeedSet.display === "NA" ? "0" : currVehicle.AirspeedSet.display)
        var spd2 = parseFloat(currVehicle.GroundSpeed.display === "NA" ? "0" : currVehicle.GroundSpeed.display)
        var spd3 = parseFloat(currVehicle.TrueAirspeed.display === "NA" ? "0" : currVehicle.TrueAirspeed.display)
        var spd4 = parseFloat(currVehicle.Airspeed.display === "NA" ? "0" : currVehicle.Airspeed.display)

        airspeedSetVal     = formatFloatValue(spd1 *3600/1000)
        groundSpeedVal     = formatFloatValue(spd2 *3600/1000)
        trueAirspeedSetVal = formatFloatValue(spd3 *3600/1000)
        airspeed           = formatFloatValue(spd4 *3600/1000)

        var spd5 = parseFloat(currVehicle.VerticalSpeed.display === "NA" ? "0" : currVehicle.VerticalSpeed.display)
        vSpeed = spd5.toFixed(0)

        var yawAngleTmp = parseFloat(currVehicle.YawAngle.display       === "NA" ? "0" : currVehicle.YawAngle.display)
        vehPitch = parseFloat(currVehicle.PitchAngle.display   === "NA" ? "0" : currVehicle.PitchAngle.display)
        var vehRollTmp = parseFloat(currVehicle.RollAngle.display    === "NA" ? "0" : currVehicle.RollAngle.display)
        vehHeight = parseFloat(currVehicle.Elevation.display     === "NA" ? "0" : currVehicle.Elevation.display)
        vehOffset = parseFloat(currVehicle.LateralOffset.display === "NA" ? "0" : currVehicle.LateralOffset.display)
        var heightOffsetTmp = parseFloat(currVehicle.HeightOffset.display === "NA" ? "0" : currVehicle.HeightOffset.display)
        vehTrail = parseInt(currVehicle.FlightAngle.display     === "NA" ? "0" : currVehicle.FlightAngle.display)
        climbslide = spd2 > 0 ? Math.atan(spd5/spd2) : 0
        heightOffsetVal = heightOffsetTmp.toFixed(2);

        vehRoll = vehRollTmp.toFixed(1);
        if (yawAngleTmp > 360) yawAngleTmp -= 360;
        yawAngle = yawAngleTmp.toFixed(1);

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

        Item {
            width: partWidth
            height: partHeight*26
            anchors.top: parent.top
            anchors.topMargin: partHeight*2
            Canvas {
                id: altCanvas
                anchors.fill: parent


                // function drawHeadingScale(ctx, ofy){
                //     ctx.beginPath()
                //     var offset = 18;
                //     var offWidth = partWidth/2;
                //     var gap = dWidth * 2 / 4;
                //     for (var im = -10; im <= 10; im += 1){
                //         var cur = (im + 360 + parseInt(yawAngle)) % 360;
                //         var off = offset / 3;
                //         if((cur % 5) == 0) {
                //             off *= 2;
                //         }
                //         if((cur % 10) == 0) {
                //             off = offset;
                //         }
                //         ctx.moveTo(offWidth + im * gap , partHeight*ofy-off);
                //         ctx.lineTo(offWidth + im * gap , partHeight*ofy);
                //         if (cur % 5 == 0){
                //             var dlen = cur.toString().length * 4
                //             ctx.fillText(cur.toString(), offWidth + im * gap - dlen, partHeight*ofy-20);
                //         }
                //         if (im == 0){
                //             ctx.moveTo(offWidth + im * gap , partHeight*ofy);
                //             ctx.lineTo(offWidth + im * gap + 5, partHeight*ofy+offset);
                //             ctx.lineTo(offWidth + im * gap - 5, partHeight*ofy+offset);
                //             ctx.lineTo(offWidth + im * gap , partHeight*ofy);
                //         }
                //     }
                //     ctx.fill()
                //     ctx.stroke()
                //     ctx.closePath()
                // }

                function drawHeadingScale(ctx, ofy){
                    ctx.beginPath()
                    var offset = 18;
                    var offWidth = partWidth/2;
                    var gap = dWidth * 2 / 4;
                    for (var im = -10; im <= 10; im += 1){
                        var cur = (im + 360 + parseInt(yawAngle)) % 360;
                        var off = offset / 3;
                        if((cur % 5) == 0) {
                            off *= 2;
                        }
                        if((cur % 10) == 0) {
                            off = offset;
                        }
                        ctx.moveTo(offWidth + im * gap , partHeight*ofy-off);
                        ctx.lineTo(offWidth + im * gap , partHeight*ofy);
                        if (cur % 5 == 0){
                            var dlen = cur.toString().length * 4
                            ctx.fillText(cur.toString(), offWidth + im * gap - dlen, partHeight*ofy-20);
                        }
                        if (im == 0){
                            var dyaw = (yawAngle - cur) * gap;
                            ctx.moveTo(offWidth + dyaw + im * gap , partHeight*ofy);
                            ctx.lineTo(offWidth + dyaw + im * gap + 5, partHeight*ofy+offset);
                            ctx.lineTo(offWidth + dyaw + im * gap - 5, partHeight*ofy+offset);
                            ctx.lineTo(offWidth + dyaw + im * gap , partHeight*ofy);
                        }
                    }
                    ctx.fill()
                    ctx.stroke()
                    ctx.closePath()
                }

                function drawRollScale2(ctx, direct) {
                    var angle = [-30, -20, -10, 0, 10, 20, 30];
                    var offxy = [-3, -2, -1, 0, 1, 4, 6];
                    var offset = 16;
                    var idx = 0;
                    var hrad = drawHeight;
                    var radius = (dWidth * 9 > hrad) ? hrad : dWidth * 9;
                    var diff = -3.14 / 180;
                    var xcenter = partWidth / 2;
                    var ycenter = drawHeight;
                    var offangle = 3.14/18;
                    ctx.beginPath()
                    for (var im = -125; im <= -55; im += 5) {
                        var dff = offset / 3;
                        if((im % 5) == 0) {
                            dff *= 2;
                        }
                        if((im % 10) == 0) {
                            dff = offset;
                        }
                        var ys = Math.sin(diff * im) * radius;
                        var xs = Math.cos(diff * im) * radius;
                        var ye = Math.sin(diff * im) * (radius+dff);
                        var xe = Math.cos(diff * im) * (radius+dff);
                        ctx.moveTo(xcenter + xs, ycenter + ys);
                        ctx.lineTo(xcenter + xe, ycenter + ye);
                        if (im % 10 == 0) {
                            var fsz = angle[idx] * direct;
                            var fontSz = fontMetrics.advanceWidth(fsz.toString());
                            ctx.fillText(fsz, xcenter + xe - fontSz/2 + offxy[idx], ycenter + ye + offset);
                            idx += 1;
                        }
                    }
                    ctx.stroke()
                    ctx.closePath()
                }

                // function drawRollScale2(ctx, direct) {
                //     var angle = [30, 20, 10, 0, -10, -20, -30];
                //     var offxy = [5, 4, 1, 0, 1, -4, -5];
                //     var offset = 16;
                //     var idx = 0;
                //     var hrad = drawHeight;
                //     var radius = (dWidth * 9 > hrad) ? (hrad-10) : (dWidth * 9-10);
                //     var diff = -3.14 / 180;
                //     var xcenter = partWidth / 2;
                //     var ycenter = drawHeight;
                //     var offangle = 3.14/18;
                //     ctx.beginPath()
                //     for (var im = 55; im <= 125; im += 5) {
                //         var dff = offset / 3;
                //         if((im % 5) == 0) {
                //             dff *= 2;
                //         }
                //         if((im % 10) == 0) {
                //             dff = offset;
                //         }
                //         var ys = Math.sin(diff * im) * radius;
                //         var xs = Math.cos(diff * im) * radius;
                //         var ye = Math.sin(diff * im) * (radius+dff);
                //         var xe = Math.cos(diff * im) * (radius+dff);
                //         ctx.moveTo(xcenter + xs, ycenter + ys);
                //         ctx.lineTo(xcenter + xe, ycenter + ye);
                //         if (im % 10 == 0) {
                //             var fsz = angle[idx] * direct;
                //             var fontSz = fontMetrics.advanceWidth(fsz.toString());
                //             ctx.fillText(fsz, xcenter + xe - fontSz/2 + offxy[idx], ycenter + ye - offset/5);
                //             idx += 1;
                //         }
                //     }
                //     ctx.stroke()
                //     ctx.closePath()
                // }

                function drawAirspeedScale(ctx) {
                    let spd = Math.ceil(airspeed)
                    let gap = partHeight * 3 / 8;
                    let dlen = 50;
                    let offset = 20;
                    ctx.beginPath()
                    for (var im = -20; im <= 20; im += 1) {
                        var cur = spd - im;
                        var off = (cur % 10 === 0) ? offset : offset / 3;
                        ctx.moveTo(splitWidth - off / 2, drawHeight + im * gap);
                        ctx.lineTo(splitWidth + off / 2, drawHeight + im * gap);
                        if (im == 0) {
                            ctx.moveTo(splitWidth + 30, drawHeight + im * gap);
                            ctx.lineTo(splitWidth + 0, drawHeight + im * gap);
                            ctx.lineTo(splitWidth + 30, drawHeight + im * gap - 5);
                            ctx.lineTo(splitWidth + 30, drawHeight + im * gap);
                        }
                        if (cur % 10 === 0) {
                            ctx.fillText(cur, splitWidth - off / 2 - dlen, drawHeight + im * gap + 5, 50, 20);
                        }
                    }
                    ctx.stroke()
                    ctx.closePath()
                }

                function drawAltitudeScale(ctx) {
                    ctx.beginPath()
                    var gap = partHeight * 3 / 8;
                    var dlen = 50;
                    var offset = 20;
                    for (var im = -20; im <= 20; im += 1) {
                        var cur = parseInt(heightOffsetVal) - im;
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
                    ctx.closePath()
                }

                function drawFlightTrail(ctx, trail, angle) {
                    if (trail > 10 || trail < -10) {
                        return;
                    }

                    ctx.beginPath()
                    ctx.strokeStyle = GcsTheme.fontPrimaryColor;
                    var cpart = partHeight / 4;
                    var offWidth = partWidth/2;
                    var gap = dWidth * 2 / 4;
                    var xpos = offWidth + gap * trail
                    var ypos = drawHeight + cpart * angle
                    ctx.arc(xpos, ypos, 5, 0, 3.14*2, false);
                    ctx.moveTo(xpos - 10, ypos);
                    ctx.lineTo(xpos - 5, ypos);
                    ctx.moveTo(xpos + 5, ypos);
                    ctx.lineTo(xpos + 10, ypos);
                    ctx.moveTo(xpos, ypos -10);
                    ctx.lineTo(xpos, ypos);
                    ctx.stroke()
                    ctx.closePath()
                }

                function drawSlideOffsetScale(ctx) {
                    var hloc = partHeight * 25;
                    var gap = dWidth * 12 / 50;
                    var vspace = partWidth/2;
                    var offset = 14;

                    ctx.beginPath()
                    for (var im = -25; im <= 25; im += 5) {
                        var off = (im % 10 == 0) ? offset : offset / 2;
                        ctx.moveTo(vspace + im * gap, hloc-off);
                        ctx.lineTo(vspace + im * gap, hloc);
                        if ((im % 10) == 0) {
                            var fontSz = fontMetrics.advanceWidth(im.toString());
                            ctx.fillText(im, vspace + im * gap - fontSz/2, hloc + 22 , 20, 20);
                        }
                    }
                    ctx.moveTo(vspace - dWidth*6, hloc);
                    ctx.lineTo(vspace + dWidth*6, hloc);
                    let showLO = vehOffset
                    if (showLO < -25) {
                        showLO = -25;
                    } else if(showLO > 25) {
                        showLO = 25;
                    }
                    var xpos = showLO * gap + vspace;
                    ctx.moveTo(xpos, hloc-10);
                    ctx.lineTo(xpos - 6, hloc - 18);
                    ctx.lineTo(xpos + 6, hloc - 18);
                    ctx.lineTo(xpos, hloc-10);
                    ctx.fill()
                    ctx.stroke()
                    ctx.closePath()
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
                            var fontSz = fontMetrics.advanceWidth(im2.toString());
                            ctx.fillText(im2.toString(), (2.5+tm2)*dWidth + 5, dcenter2+4);
                            ctx.fillText(im2.toString(), -(2.5+tm2)*dWidth-fontSz-5, dcenter2+4);
                        }
                    }
                }

                function drawGroundRefer(ctx, roll) {
                    ctx.save();
                    ctx.beginPath()
                    ctx.strokeStyle = "#e0900a";
                    ctx.translate(partWidth / 2, drawHeight);
                    ctx.setLineDash([2, 2])
                    ctx.lineDashOffset = 0
                    ctx.moveTo(-dWidth*5, 0)
                    ctx.lineTo( dWidth*5, 0)
                    ctx.stroke()
                    ctx.restore()

                    ctx.save();
                    ctx.beginPath()
                    ctx.setLineDash([200,0])
                    ctx.strokeStyle = "#e0900a";
                    ctx.translate(partWidth / 2, drawHeight);
                    ctx.rotate(roll * Math.PI / 180);
                    ctx.moveTo(-dWidth, 0);
                    ctx.lineTo(-dWidth / 2, 0);
                    ctx.lineTo(-dWidth / 4, 10);
                    ctx.lineTo(0, 0);
                    ctx.lineTo(dWidth / 4, 10);
                    ctx.lineTo(dWidth / 2, 0);
                    ctx.lineTo(dWidth, 0);
                    ctx.stroke()
                    ctx.restore()
                }

                function drawPitchAngleScale(ctx, roll) {
                    ctx.save();
                    ctx.beginPath()
                    ctx.strokeStyle = drawLineColor;
                    ctx.translate(partWidth / 2, drawHeight);
                    var hrad = drawHeight;
                    var curHigh = ((dWidth * 9 > hrad) ? hrad : dWidth * 9);
                    let droll = -roll;
                    if (droll < -35) {
                        droll = -35
                        ctx.strokeStyle = "#e0900a"
                    } else if(droll > 35) {
                        droll = 35
                        ctx.strokeStyle = "#e0900a"
                    }
                    ctx.rotate(droll * Math.PI / 180);
                    // ctx.moveTo(0, curHigh + 6);
                    // ctx.lineTo(-6, curHigh + 15);
                    // ctx.lineTo(6, curHigh + 15);
                    // ctx.lineTo(0, curHigh + 6);
                    ctx.moveTo(0, curHigh - 2);
                    ctx.lineTo(-6, curHigh - 20);
                    ctx.lineTo(6, curHigh - 20);
                    ctx.lineTo(0, curHigh - 2);
                    ctx.fill()
                    ctx.stroke()
                    ctx.restore();

                    ctx.save();
                    ctx.beginPath()
                    ctx.translate(partWidth / 2, drawHeight);
                    ctx.rotate(roll * Math.PI / 180);
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
                        ctx.rotate(roll * Math.PI / 180);
                        ctx.strokeStyle = "#e0900a";
                        pitchAngleDraw(ctx, vehPitch, lower, 0, cpart);
                    }

                    ctx.stroke()
                    ctx.restore();
                }

                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, partWidth, partHeight*26);
                    ctx.font = frontName;
                    ctx.lineWidth = 2;
                    ctx.strokeStyle = drawLineColor;
                    ctx.fillStyle = drawLineColor;

                    ctx.save();
                    drawHeadingScale(ctx, 2);
                    drawRollScale2(ctx, rotateDirect);
                    drawAirspeedScale(ctx)
                    drawAltitudeScale(ctx)
                    drawSlideOffsetScale(ctx)
                    drawFlightTrail(ctx, vehTrail - vehTrail, climbslide)
                    ctx.restore();

                    drawGroundRefer(ctx, rotateDirect>0?vehRoll:0)
                    drawPitchAngleScale(ctx, rotateDirect*vehRoll)
                }
            }
        }

        GcsButton {
            radiusVal: 5
            text: vehPitch
            width: dWidth+10
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 7
            anchors.left: upper.left
            anchors.leftMargin: dWidth*5-10
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: vSpeed
            width: dWidth+10
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 7
            anchors.right: upper.right
            anchors.rightMargin: dWidth*5-10
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: yawAngle
            width: dWidth*2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 2 + 15
            anchors.left: upper.left
            anchors.leftMargin: partWidth/2 - dWidth
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: vehRoll
            width: dWidth*2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 21
            anchors.left: upper.left
            anchors.leftMargin: partWidth/2 - dWidth
            textColor: GcsTheme.primaryColor
        }

        GcsText {
            text: airspeedSet
            width: dWidth*2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 4
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: airspeedSetVal
            width: dWidth*2+dWidth/2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 4
            anchors.left: upper.left
            anchors.leftMargin: dWidth*2
            textColor: GcsTheme.setvalueColor
        }

        GcsText {
            text: heightSet
            width: dWidth*2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 4
            anchors.right: upper.right
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: heightSetVal
            width: dWidth*2+dWidth/2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 4
            anchors.right: upper.right
            anchors.rightMargin: dWidth*2
            textColor: GcsTheme.setvalueColor
        }

        GcsText {
            text: highTypeStr
            width: dWidth*2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 19
            anchors.right: upper.right
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: highTypeStrVal
            width: dWidth*2+dWidth/2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 19
            anchors.right: upper.right
            anchors.rightMargin: dWidth*2
            textColor: GcsTheme.primaryColor
            btnFontSize: TextStyle.BodySmall
        }

        GcsText {
            text: trueAirspeedSet
            width: dWidth*2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 19
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: trueAirspeedSetVal
            width: dWidth*2+dWidth/2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 19
            anchors.left: upper.left
            anchors.leftMargin: dWidth*2
            textColor: GcsTheme.primaryColor
        }

        GcsText {
            text: groundSpeed
            width: dWidth*2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 20
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: groundSpeedVal
            width: dWidth*2+dWidth/2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 20
            anchors.left: upper.left
            anchors.leftMargin: dWidth*2
            textColor: GcsTheme.primaryColor
        }

        GcsText {
            text: radioHeightStr
            width: dWidth*2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 20
            anchors.right: upper.right
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: radioHeightStrVal
            width: dWidth*2+dWidth/2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 20
            anchors.right: upper.right
            anchors.rightMargin: dWidth*2
            textColor: GcsTheme.primaryColor
            btnFontSize: TextStyle.BodySmall
        }

        GcsText {
            //text: heightOffset
            text: vehHeightStr
            width: dWidth*2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 21
            anchors.right: upper.right
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            text: vehHeight
            width: dWidth*2+dWidth/2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: partHeight * 21
            anchors.right: upper.right
            anchors.rightMargin: dWidth*2
            textColor: GcsTheme.primaryColor
            btnFontSize: TextStyle.BodySmall
        }

        GcsButton {
            radiusVal: 5
            text: vehOffset
            width: dWidth*3
            height: partHeight-5
            anchors.top: upper.top
            anchors.topMargin: partHeight * 28
            anchors.right: upper.right
            anchors.rightMargin: partWidth/2-dWidth-dWidth/2
            textColor: GcsTheme.primaryColor
        }

        GcsButton {
            radiusVal: 5
            width: dWidth*2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: drawHeight+partHeight*3/2
            anchors.left: upper.left
            anchors.leftMargin: dWidth*2
            textColor: GcsTheme.primaryColor
            text: airspeed
        }

        GcsButton {
            radiusVal: 5
            text: heightOffsetVal
            width: dWidth*2
            height: partHeight
            anchors.top: upper.top
            anchors.topMargin: drawHeight+partHeight*3/2
            anchors.right: upper.right
            anchors.rightMargin: dWidth*2
            textColor: GcsTheme.primaryColor
        }
    }
}
