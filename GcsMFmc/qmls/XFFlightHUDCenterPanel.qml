import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    id: centerPanel
    property real minSpeed: currVehicle? currVehicle.minAirspeed: 45
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

    property real heightOffsetVal: 0
    property real yawAngle: 0
    property real airspeed:  0
    property real vehPitch:  0
    property real vehRoll:  0
    property real vehOffset:  0
    property real vehHeight: 0
    property real slideAngle: 0
    property real vSpeed: 0

    property string drawFontSet: TextStyle.Body + 'px bold Arial'
    property int fontHeight: TextStyle.Body
    property real drawWidth: dWidth*5
    property real offWidth: 10.5
    property int timeCount: 0

    function formatFloatValue(value) {
        let str = value.toFixed(1);
        if (str.length >= 5 ) return str;
        return " ".repeat(5 - str.length) + str;
    }

    Connections{
        target: commandMgr
        function onSignalDrawing() {
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
                radioHeightStr = currVehicle.RadioAltimeter.itemName
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
            var vehRollTmp  = parseFloat(currVehicle.RollAngle.display    === "NA" ? "0" : currVehicle.RollAngle.display)
            vehHeight = parseFloat(currVehicle.Elevation.display     === "NA" ? "0" : currVehicle.Elevation.display)
            vehOffset = parseFloat(currVehicle.LateralOffset.display === "NA" ? "0" : currVehicle.LateralOffset.display)
            var heightOffsetTmp = parseFloat(currVehicle.HeightOffset.display === "NA" ? "0" : currVehicle.HeightOffset.display)

            vehRoll = vehRollTmp.toFixed(1);
            heightOffsetVal = heightOffsetTmp.toFixed(2);

            if (yawAngleTmp > 360) yawAngleTmp -= 360;
            yawAngle = yawAngleTmp.toFixed(1);

            altCanvas.requestPaint();
        }
    }

    FontMetrics {
        id: fontMetrics
        font.family: TextStyle.family
        font.pixelSize: fontHeight
    }

    Item {
        id: upper
        width: ctrlWidth
        height: ctrlHeight
        anchors.left: centerPanel.left
        anchors.top: centerPanel.top 

        Canvas {
            id: altCanvas
            width: parent.width
            height: parent.height
            anchors.fill: parent

            // function drawHeadingScale(ctx, angle, offset){
            //     ctx.beginPath()
            //     ctx.strokeStyle = GcsTheme.White;
            //     ctx.fillStyle = GcsTheme.White;
            //     var gap = dWidth / 2;
            //     var hpart = drawHeight + partHeight * 12
            //     for (var im = -10; im <= 10; im += 1){
            //         var cur = (im + 360 + angle) % 360;
            //         var off = offset / 3;
            //         if((cur % 5) == 0) {
            //             off *= 2;
            //         }
            //         if((cur % 10) == 0) {
            //             off = offset;
            //         }
            //         ctx.moveTo(dWidth*offWidth + im * gap , hpart-off);
            //         ctx.lineTo(dWidth*offWidth + im * gap , hpart);
            //         if (cur % 5 == 0){
            //             var fsz = String(cur).length * 4;
            //             ctx.fillText(cur.toString(), dWidth*offWidth + im * gap - fsz, hpart-offset-5);
            //         }
            //         if (im == 0){
            //            ctx.lineTo(dWidth*offWidth + im * gap + 5, hpart - 12);
            //            ctx.lineTo(dWidth*offWidth + im * gap - 5, hpart - 12);
            //            ctx.lineTo(dWidth*offWidth + im * gap , hpart);
            //         }
            //     }
            //     ctx.fill()
            //     ctx.stroke()
            // }

            function drawHeadingScale(ctx, angle, offset){
                ctx.beginPath()
                ctx.strokeStyle = GcsTheme.White;
                ctx.fillStyle = GcsTheme.White;
                var gap = dWidth / 2;
                var hpart = drawHeight - partHeight * 11
                for (var im = -10; im <= 10; im += 1){
                    var cur = (im + 360 + angle) % 360;
                    var off = offset / 3;
                    if((cur % 5) == 0) {
                        off *= 2;
                    }
                    if((cur % 10) == 0) {
                        off = offset;
                    }
                    ctx.moveTo(dWidth*offWidth + im * gap , hpart-off);
                    ctx.lineTo(dWidth*offWidth + im * gap , hpart);
                    if (cur % 5 == 0){
                        var fsz = String(cur).length * 4;
                        ctx.fillText(cur.toString(), dWidth*offWidth + im * gap - fsz, hpart-offset-5);
                    }
                    if (im == 0){
                       var dyaw = (yawAngle - cur) * gap;
                       ctx.moveTo(dWidth*offWidth + dyaw + im * gap , hpart);
                       ctx.lineTo(dWidth*offWidth + dyaw + im * gap + 5, hpart + 10);
                       ctx.lineTo(dWidth*offWidth + dyaw + im * gap - 5, hpart + 10);
                       ctx.lineTo(dWidth*offWidth + dyaw + im * gap , hpart);
                    }
                }
                ctx.fill()
                ctx.stroke()
            }

            // function drawRollScale(ctx, xcenter, ycenter, radius, direction){
            //     var idx = 0;
            //     var offset = 14
            //     var angle = [30, 20, 10, 0, -10, -20, -30];
            //     var offt = [4, 3, 1, 0, -1, -3, -4];
            //     var diff = -3.14/180
            //     ctx.beginPath()
            //     ctx.strokeStyle = GcsTheme.White;
            //     ctx.fillStyle = GcsTheme.White;
            //     for (var im = 55; im <= 125; im += 5){
            //         var dff = offset / 3;
            //         if((im % 5) == 0) {
            //             dff *= 2;
            //         }
            //         if((im % 10) == 0) {
            //             dff = offset;
            //         }
            //         var ys = Math.sin(diff*im) * radius;
            //         var xs = Math.cos(diff*im) * radius;
            //         var ye = Math.sin(diff*im) * (radius+dff);
            //         var xe = Math.cos(diff*im) * (radius+dff);
            //         ctx.moveTo(xcenter + xs , ycenter + ys);
            //         ctx.lineTo(xcenter + xe , ycenter + ye);
            //         if (im % 10 == 0){
            //             var dangle = direction * angle[idx]
            //             var fsz = dangle.toString();
            //             var fontSz = fontMetrics.advanceWidth(fsz.toString())
            //             ctx.fillText(fsz, xcenter + xe - fontSz/2 + offt[idx], ycenter + ye - offset/2);
            //             idx += 1;
            //         }
            //     }
            //     ctx.stroke()
            // }

            function drawRollScale(ctx, xcenter, ycenter, radius, direction){
                var idx = 0;
                var offset = 14
                var angle = [-30, -20, -10, 0, 10, 20, 30];
                var offt = [-4, -3, -1, 0, 1, 3, 4];
                var diff = -3.14/180
                ctx.beginPath()
                ctx.strokeStyle = GcsTheme.White;
                ctx.fillStyle = GcsTheme.White;
                for (var im = -125; im <= -55; im += 5){
                    var dff = offset / 3;
                    if((im % 5) == 0) {
                        dff *= 2;
                    }
                    if((im % 10) == 0) {
                        dff = offset;
                    }
                    var ys = Math.sin(diff*im) * radius;
                    var xs = Math.cos(diff*im) * radius;
                    var ye = Math.sin(diff*im) * (radius+dff);
                    var xe = Math.cos(diff*im) * (radius+dff);
                    ctx.moveTo(xcenter + xs , ycenter + ys);
                    ctx.lineTo(xcenter + xe , ycenter + ye);
                    if (im % 10 == 0){
                        var dangle = direction * angle[idx]
                        var fsz = dangle.toString();
                        var fontSz = fontMetrics.advanceWidth(fsz.toString())
                        ctx.fillText(fsz, xcenter + xe - fontSz/2 + offt[idx], ycenter + ye + offset);
                        idx += 1;
                    }
                }
                ctx.stroke()
            }

            function drawAirspeedScale(ctx, speed, offset){
                ctx.beginPath()
                ctx.fillStyle = grayStyle;
                ctx.fillRect(dWidth*3-20, drawHeight - 11 * partHeight, dWidth*2+20, 22 * partHeight);
                let spd = Math.ceil(speed)
                let gap = partHeight/2;

                ctx.fill()
                ctx.stroke()
                ctx.strokeStyle = GcsTheme.White;
                ctx.fillStyle = GcsTheme.White;

                for (var im = -20; im <= 20; im += 1){
                    var cur = spd - im;
                    var off = ((cur % 10) === 0) ? offset : offset / 2;
                    ctx.moveTo(drawWidth - off, drawHeight + im * gap);
                    ctx.lineTo(drawWidth, drawHeight + im * gap);
                    if ((cur % 10) === 0){
                        var fontSz = fontMetrics.advanceWidth(cur.toString());
                        ctx.fillText(cur.toString(), drawWidth - dWidth - fontSz, drawHeight + im * gap + 5);
                    }
                }
                ctx.fill()
                ctx.stroke()

                // if (Math.abs(speed - minSpeed) < 20) {
                //     ctx.beginPath()
                //     ctx.strokeStyle = GcsTheme.orange;
                //     ctx.fillStyle = GcsTheme.orange;
                //     var minv =(speed - minSpeed) * partHeight/2;
                //     ctx.moveTo(drawWidth, drawHeight + minv)
                //     ctx.lineTo(drawWidth-dWidth, drawHeight + minv)
                //     ctx.fill()
                //     ctx.stroke()
                // }
            }

            function drawVSpeedScale(ctx, vspeed, offset){
                var gap = partHeight - 3;
                // ctx.beginPath()
                // ctx.strokeStyle = grayStyle;
                // ctx.fillStyle = grayStyle;
                // ctx.moveTo(ctrlWidth - dWidth*2, drawHeight - 8 * gap);
                // ctx.lineTo(ctrlWidth - dWidth*2, drawHeight + 8 * gap );
                // ctx.lineTo(ctrlWidth - dWidth, drawHeight + 8 * gap );
                // ctx.lineTo(ctrlWidth - dWidth/2, drawHeight + 7 * gap );
                // ctx.lineTo(ctrlWidth - dWidth/2, drawHeight - 7 * gap );
                // ctx.lineTo(ctrlWidth - dWidth, drawHeight - 8 * gap );
                // ctx.lineTo(ctrlWidth - dWidth*2, drawHeight - 8 * gap);
                // ctx.fill();
                // ctx.stroke()

                ctx.beginPath()
                ctx.strokeStyle = GcsTheme.White;
                ctx.fillStyle = GcsTheme.White;
                for (var im = -6; im <= 6; im += 1){
                    var val = (im+6) % 2
                    ctx.moveTo(ctrlWidth - dWidth * 2 + 4, drawHeight + im * gap);
                    ctx.lineTo(ctrlWidth - dWidth * 2 + (val?10:16), drawHeight + im * gap);
                    if ( val == 0 && im != 0) {
                        var mx = (-im + vspeed).toString();
                        ctx.fillText(mx, ctrlWidth - dWidth - 4, drawHeight + im * gap + 5);
                    }
                }
                ctx.moveTo(ctrlWidth - dWidth - 4, drawHeight)
                ctx.lineTo(ctrlWidth - dWidth * 2 + 10, drawHeight)
                ctx.lineTo(ctrlWidth - dWidth * 2 + 16, drawHeight+3)
                ctx.lineTo(ctrlWidth - dWidth * 2 + 16, drawHeight-3)
                ctx.lineTo(ctrlWidth - dWidth * 2 + 10, drawHeight)
                var curr = vspeed.toString();
                ctx.fillText(curr, ctrlWidth - dWidth, drawHeight + 5);
                ctx.fill()
                ctx.stroke()
            }

            function drawAltitudeScale(ctx, alti, offset){
                ctx.beginPath()
                ctx.fillStyle = grayStyle;
                ctx.fillRect(ctrlWidth - drawWidth, drawHeight - 11 * partHeight, dWidth*2+20, 22 * partHeight);
                var gap = partHeight/2;
                ctx.fill()
                ctx.stroke()
                ctx.strokeStyle = GcsTheme.White;
                ctx.fillStyle = GcsTheme.White;
                for (var im = -20; im <= 20; im += 1){
                    var cur = alti - im;
                    var curhigh = drawHeight + im * gap
                    var off = (cur % 10 == 0) ? offset : offset / 2;
                    ctx.moveTo(ctrlWidth - drawWidth, curhigh );
                    ctx.lineTo(ctrlWidth - drawWidth + off, curhigh );
                    if (cur % 10 == 0){
                        ctx.fillText(cur.toString(), ctrlWidth - drawWidth + dWidth , curhigh+5);
                    }
                }
                ctx.fill();
                ctx.stroke()
            }

            function drawGroundRefer(ctx, rollAngle) {
                ctx.save();
                ctx.beginPath()
                ctx.strokeStyle = GcsTheme.White;
                ctx.fillStyle = GcsTheme.White;
                ctx.translate(ctrlWidth/2, drawHeight);
                ctx.rotate(rollAngle * Math.PI / 180);
                ctx.moveTo(-dWidth, 0);
                ctx.lineTo(-dWidth/2, 0);
                ctx.lineTo(-dWidth/4, 10);
                ctx.lineTo(0, 0);
                ctx.lineTo(dWidth/4, 10);
                ctx.lineTo(dWidth/2, 0);
                ctx.lineTo(dWidth, 0);
                ctx.stroke()
                ctx.restore();
            }

            function drawSlideOffsetScale(ctx, offset) {
                ctx.beginPath()
                ctx.strokeStyle = GcsTheme.White;
                ctx.fillStyle = GcsTheme.White;
                var gap = dWidth * 10 / 50;
                var hval = drawHeight + 15 * partHeight + partHeight/2
                for (var im = -25; im <= 25; im += 5) {
                    var off = (im % 10 == 0) ? offset : offset / 2;
                    ctx.moveTo(ctrlWidth/2 + im * gap, hval - off);
                    ctx.lineTo(ctrlWidth/2 + im * gap, hval);

                    if ((im % 10) == 0) {
                        var fontSz = fontMetrics.advanceWidth(im.toString());
                        ctx.fillText(im, ctrlWidth/2 + im * gap - fontSz/2, hval + 15);
                    }
                }

                ctx.moveTo(ctrlWidth/2 - dWidth * 5, hval);
                ctx.lineTo(ctrlWidth/2 + dWidth * 5, hval);
                let showLO = vehOffset
                if (showLO < -25) {
                    showLO = -25;
                } else if(showLO > 25) {
                    showLO = 25;
                }
                var xpos = ctrlWidth/2 + showLO * gap;
                ctx.moveTo(xpos, hval - 8);
                ctx.lineTo(xpos - 6, hval - 18);
                ctx.lineTo(xpos + 6, hval - 18);
                ctx.lineTo(xpos, hval - 8);
                ctx.fill()
                ctx.stroke()
            }

            function drawPitchRollScale(ctx, offset, roll, pitch){
                var cpart = partHeight * 12 / 40;
                //var rloc = -partHeight*11;
                var rloc = partHeight*11;

                ctx.save();
                ctx.beginPath()
                ctx.lineWidth = 2;
                ctx.strokeStyle = GcsTheme.White;
                ctx.fillStyle = GcsTheme.White;
                ctx.translate(ctrlWidth/2, drawHeight);
                let droll = -roll;
                if (droll < -35) {
                    droll = -35
                } else if(droll > 35) {
                    droll = 35
                }
                ctx.rotate(droll * Math.PI / 180);
                // ctx.moveTo(0, rloc-10);
                // ctx.lineTo(-5, rloc+5);
                // ctx.lineTo(+5, rloc+5);
                // ctx.lineTo(0, rloc-10);
                ctx.moveTo(0, rloc+5);
                ctx.lineTo(-5, rloc-6);
                ctx.lineTo(+5, rloc-6);
                ctx.lineTo(0, rloc+5);
                ctx.fill()
                ctx.stroke()
                ctx.restore();

                ctx.save();
                ctx.beginPath()
                ctx.strokeStyle = GcsTheme.White;
                ctx.fillStyle = GcsTheme.White;
                ctx.translate(ctrlWidth/2, drawHeight);
                ctx.rotate(roll * Math.PI / 180);
                var lower = Math.ceil(pitch - 20)
                var upper = Math.ceil(pitch + 20)
                for (var im = lower; im <= upper ; im++){
                    if (im % 5 != 0) {
                        continue;
                    }
                    var tm = ((im % 10) == 0) ? 1 : 0;
                    var dcenter = (pitch - im) * cpart
                    ctx.moveTo(-(2.5+tm)*dWidth, dcenter);
                    ctx.lineTo(     -1.5*dWidth, dcenter);
                    ctx.moveTo(      1.5*dWidth, dcenter);
                    ctx.lineTo( (2.5+tm)*dWidth, dcenter);
                    if (tm === 1){
                        var fsz = String(im).length * 2;
                        var fontSz = fontMetrics.advanceWidth(im.toString());
                        ctx.fillText(im.toString(), (2.5+tm)*dWidth + 5, dcenter+4);
                        ctx.fillText(im.toString(), -(2.5+tm)*dWidth-fontSz-5, dcenter+4);
                    }
                }
                ctx.stroke()
                ctx.restore();
            }

            onPaint: {
                var ctx = getContext("2d");
                ctx.clearRect(0, 0, ctrlWidth, ctrlHeight);
                ctx.font = drawFontSet;
                ctx.lineWidth = 2;


                ctx.save();
                drawHeadingScale(ctx, parseInt(yawAngle), 14);
                drawRollScale(ctx, ctrlWidth/2, drawHeight, partHeight*11, rotateDirect);
                drawAirspeedScale(ctx, airspeed, 20);
                drawAltitudeScale(ctx, parseInt(heightOffsetVal), 20);
                drawVSpeedScale(ctx, vSpeed , 10);
                drawSlideOffsetScale(ctx, 10)
                ctx.restore();

                drawGroundRefer(ctx, rotateDirect > 0 ? vehRoll : 0);
                drawPitchRollScale(ctx, 20, rotateDirect * vehRoll, vehPitch);
            }
        }

        GcsButton {
            radiusVal: 5
            width: dWidth*2+20
            height: 25
            anchors.top: parent.top
            //anchors.topMargin: drawHeight + 11 * partHeight + 5
            anchors.topMargin: drawHeight + 9 * partHeight - 5
            anchors.left: parent.left
            anchors.leftMargin:ctrlWidth/2-dWidth-10
            text: vehRoll
        }

        GcsButton {
            radiusVal: 5
            width: dWidth*2+20
            height: 25
            anchors.top: parent.top
            anchors.topMargin: drawHeight - 9 * partHeight - 10
            anchors.left: parent.left
            anchors.leftMargin:ctrlWidth/2-dWidth-10
            text: yawAngle
        }

        GcsButton{
            radiusVal: 5
            width: dWidth*2+20
            height: partHeight+partHeight/2
            anchors.top: parent.top
            anchors.topMargin: drawHeight - 12 * partHeight
            anchors.left: parent.left
            anchors.leftMargin: dWidth*3-20
            textColor: GcsTheme.setvalueColor
            text: airspeedSetVal
        }

        GcsButton{
            radiusVal: 5
            width: dWidth*2+20
            height: partHeight+partHeight/2
            anchors.top: parent.top
            anchors.topMargin: drawHeight - 12 * partHeight
            anchors.right: parent.right
            anchors.rightMargin: dWidth*3-20
            textColor: GcsTheme.setvalueColor
            text: heightSetVal
        }

        GcsButton {
            text: airspeed
            radiusVal: 5
            width: dWidth*2+10
            height: 30
            anchors.top: upper.top
            anchors.topMargin: drawHeight-15
            anchors.left: upper.left
            anchors.leftMargin: dWidth*3-20
        }

        GcsButton {
            text: heightOffsetVal
            radiusVal: 5
            width: dWidth*2+10
            height: 30
            anchors.top: upper.top
            anchors.topMargin: drawHeight-15
            anchors.right: upper.right
            anchors.rightMargin: dWidth*3-20
        }

        GcsText {
            text: highTypeStr
            width: dWidth*3-20
            height: 25
            anchors.top: parent.top
            anchors.topMargin: drawHeight + 10* partHeight
            anchors.right: upper.right
            anchors.rightMargin: 0
        }

        GcsButton {
            width: dWidth*2+20
            height: 25
            anchors.top: parent.top
            anchors.topMargin: drawHeight + 10* partHeight
            anchors.right: upper.right
            anchors.rightMargin: dWidth*3-20
            btnFontSize: TextStyle.BodySmall
            text: highTypeStrVal
        }

        GcsText {
            text: trueAirspeedSet
            width: dWidth*3-20
            height: 25
            anchors.top: parent.top
            anchors.topMargin: drawHeight +10* partHeight
            anchors.left: parent.left
            anchors.leftMargin: 0
        }

        GcsButton{
            width: dWidth*2+20
            height: 25
            anchors.top: parent.top
            anchors.topMargin: drawHeight +10* partHeight
            anchors.left: parent.left
            anchors.leftMargin: dWidth*3-20
            text: trueAirspeedSetVal
        }

        GcsText {
            text: groundSpeed
            width: dWidth*3-20
            height: 25
            anchors.top: parent.top
            anchors.topMargin: drawHeight +10* partHeight + 25
            anchors.left: parent.left
            anchors.leftMargin: 0
        }

        GcsButton{
            width: dWidth*2+20
            height: 25
            anchors.top: parent.top
            anchors.topMargin: drawHeight +10* partHeight + 25
            anchors.left: parent.left
            anchors.leftMargin: dWidth*3-20
            text: groundSpeedVal
        }

        GcsText {
            text: radioHeightStr
            width: dWidth*3-20
            height: 25
            anchors.top: parent.top
            anchors.topMargin: drawHeight + 10* partHeight + 25
            anchors.right: upper.right
            anchors.rightMargin: 0
        }

        GcsButton {
            width: dWidth*2+20
            height: 25
            anchors.top: parent.top
            anchors.topMargin: drawHeight +10* partHeight + 25
            anchors.right: upper.right
            anchors.rightMargin: dWidth*3-20
            btnFontSize: TextStyle.BodySmall
            text: radioHeightStrVal
        }

        GcsText {
            //text: heightOffset
            text: vehHeightStr
            width: dWidth*3-20
            height: 25
            anchors.top: parent.top
            anchors.topMargin: drawHeight + 10* partHeight + 50
            anchors.right: upper.right
            anchors.rightMargin: 0
        }

        GcsButton {
            width: dWidth*2+20
            height: 25
            anchors.top: parent.top
            anchors.topMargin: drawHeight +10* partHeight + 50
            anchors.right: upper.right
            anchors.rightMargin: dWidth*3-20
            btnFontSize: TextStyle.BodySmall
            text: vehHeight
        }

        GcsButton{
            radiusVal: 5
            height: 25
            width: dWidth*2+20
            anchors.bottom: upper.bottom
            anchors.bottomMargin: partHeight/2
            anchors.right: upper.right
            anchors.rightMargin: ctrlWidth/2-dWidth-10
            text: vehOffset
        }
    }
}
