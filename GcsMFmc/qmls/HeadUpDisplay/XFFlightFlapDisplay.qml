import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    property string frontName: "16px bold Arial"
    property var leftBrake: currVehicle ? currVehicle.LeftBrake: null
    property var rightBrake: currVehicle ? currVehicle.RightBrake: null
    property var frontWheelAngle: currVehicle ? currVehicle.FrontWheelAngle: null
    property var leftWheelState: currVehicle ? currVehicle.leftWheelState: null
    property var rightWheelState: currVehicle ? currVehicle.rightWheelState: null
    property var frontWheelState: currVehicle ? currVehicle.frontWheelState: null
    property var airOrGround: currVehicle ? currVehicle.AirOrGround: null
    // engine part
    property var engineMode: currVehicle?currVehicle.engineMode:null
    property var engineCurNode: engineMode?engineMode.deftElemNode:null
    property var phyRSpeed: engineCurNode ? engineCurNode.physicalRSpeed: null
    property var givenSpeed: engineCurNode ? engineCurNode.givenSpeed: null
    // flap part
    property var flapRundder: currVehicle?currVehicle.flapRunnder:null
    property var uptDataSrc: flapRundder?flapRundder.deftElemNode:null
    property var leftAileron: uptDataSrc ? uptDataSrc.leftAileron: null//襟翼
    property var leftFlaps: uptDataSrc ? uptDataSrc.leftFlaps: null //副翼
    property var rightFlaps: uptDataSrc ? uptDataSrc.rightFlaps: null//副翼
    property var rightAileron: uptDataSrc ? uptDataSrc.rightAileron: null//襟翼
    property var leftElevator: uptDataSrc ? uptDataSrc.leftElevator: null //升降舵
    property var rightElevator: uptDataSrc ? uptDataSrc.rightElevator: null//升降舵
    property var leftRudder: uptDataSrc ? uptDataSrc.leftRudder: null //方向舵
    property var rightRudder: uptDataSrc ? uptDataSrc.rightRudder: null//方向舵
    property real leftElevatorVal: 0
    property real rightElevatorVal: 0
    property real leftRudderVal:  0
    property real rightRudderVal: 0
    property real lFlapPosition: 0
    property real rFlapPosition: 0
    property real lAileronPosition: 0
    property real rAileronPosition: 0
    property real phyRSpeedRate: 0.5
    property real lwheelRate: 0.3
    property real rwheelRate: 0.6
    property real rundderElevatorFlatRes: configMgr.rundderElevatorFlatRes
    property color engineSpdColor: GcsTheme.primaryColor
    property color leftBrakeColor: GcsTheme.yellow
    property color rightBrakeColor: GcsTheme.yellow
    property color leftWheelColor: GcsTheme.orange
    property color rightWheelColor: GcsTheme.orange
    property color frontWheelColor: GcsTheme.orange
    property int onGround: 1

    function displayValueFormat(data) {
        if (data && data.display.indexOf("NA") === -1) {
            var tm = parseFloat(data.display);
            return tm.toFixed(2);
        }
        return 0;
    }

    Connections{
        target: commandMgr
        function onSignalDrawing () {

            leftWheelColor = leftWheelState? txtColorMap[leftWheelState.showColor & 0xff] : GcsTheme.orange
            rightWheelColor = rightWheelState? txtColorMap[rightWheelState.showColor & 0xff] : GcsTheme.orange
            frontWheelColor = frontWheelState? txtColorMap[frontWheelState.showColor & 0xff] : GcsTheme.orange
            onGround = airOrGround?parseInt(airOrGround.display):1
            phyRSpeedRate = phyRSpeed? phyRSpeed.rateValue:0
            lwheelRate = leftBrake?leftBrake.rateValue:0
            rwheelRate = rightBrake?rightBrake.rateValue:0
            engineSpdColor = phyRSpeed? txtColorMap[phyRSpeed.showColor & 0xff] : GcsTheme.primaryColor
            leftBrakeColor = leftBrake? txtColorMap[leftBrake.showColor & 0xff] : GcsTheme.yellow
            rightBrakeColor = rightBrake? txtColorMap[rightBrake.showColor & 0xff] : GcsTheme.yellow

            leftElevatorVal = displayValueFormat(leftElevator)
            rightElevatorVal= displayValueFormat(rightElevator)
            leftRudderVal = displayValueFormat(leftRudder)
            rightRudderVal= displayValueFormat(rightRudder)
            lFlapPosition= displayValueFormat(leftFlaps)
            rFlapPosition= displayValueFormat(rightFlaps)
            lAileronPosition= displayValueFormat(leftAileron)
            rAileronPosition= displayValueFormat(rightAileron)

            vehCanvas.requestPaint()
        }
    }

    Canvas {
        id: vehCanvas
        anchors.fill: parent

        function drawVehicle(ctx, width, height, rpm){
            var stepH = height/16;
            var stepW = width/16;
            var hwidth = width/2;
            ctx.font = frontName;
            ctx.strokeStyle=GcsTheme.primaryColor;
            ctx.lineWidth = 1;

            ctx.beginPath()
            ctx.moveTo(hwidth - stepW*2 - 10, stepH*13);
            ctx.lineTo(hwidth - stepW*2 - 10, stepH*8);
            ctx.lineTo(hwidth - stepW*7, stepH*7);
            ctx.bezierCurveTo(hwidth - stepW*8, stepH*8-10,
                              hwidth - stepW*7-10, stepH*5+10,
                              hwidth - stepW*7, stepH*5+10);
            ctx.lineTo(hwidth - stepW*2, stepH*5);
            ctx.bezierCurveTo(hwidth - stepW, stepH*5,
                              hwidth - stepW, stepH+stepH/4,
                              hwidth, stepH);
            ctx.bezierCurveTo(hwidth + stepW, stepH+stepH/4,
                              hwidth + stepW, stepH*5,
                              hwidth + stepW*2, stepH*5);
            ctx.lineTo(hwidth + stepW*7, stepH*5+10);
            ctx.bezierCurveTo(hwidth + stepW*7+10, stepH*5+10,
                              hwidth + stepW*8, stepH*8-10,
                              hwidth + stepW*7, stepH*7);
            ctx.lineTo(hwidth + stepW*2 + 10, stepH*8);
            ctx.lineTo(hwidth + stepW*2 + 10, stepH*13);
            // ctx.strokeRect(hwidth + stepW*2-10, stepH*13, 20, stepH*2)
            // ctx.strokeRect(hwidth - stepW*2-10, stepH*13, 20, stepH*2)
            // ctx.strokeRect(hwidth - stepW*2+10, stepH*14, stepW*2-10, stepH)
            // ctx.strokeRect(hwidth, stepH*14, stepW*2 -10, stepH)
            ctx.strokeRect(hwidth + stepW*2-10, stepH*13, 10, stepH*2)
            ctx.strokeRect(hwidth + stepW*2, stepH*13, 10, stepH*2)
            ctx.strokeRect(hwidth - stepW*2-10, stepH*13, 10, stepH*2)
            ctx.strokeRect(hwidth - stepW*2, stepH*13, 10, stepH*2)
            ctx.strokeRect(hwidth - stepW*2+10, stepH*14, stepW*2-10, stepH/2)
            ctx.strokeRect(hwidth - stepW*2+10, stepH*14+stepH/2, stepW*2-10, stepH/2)
            ctx.strokeRect(hwidth, stepH*14, stepW*2 -10, stepH/2)
            ctx.strokeRect(hwidth, stepH*14+stepH/2, stepW*2 -10, stepH/2)

            ctx.moveTo(hwidth + stepW*2-stepW/4-1, stepH*14);
            ctx.lineTo(hwidth - stepW*2+stepW/4+1, stepH*14);
            ctx.bezierCurveTo(hwidth - stepW*2+stepW/4+1, stepH*7,
                              hwidth - stepW*2+stepW/4+1, stepH*7,
                              hwidth - stepW*2+stepW/2+stepW/4, stepH*11);
            ctx.moveTo(hwidth + stepW*2-stepW/4-1, stepH*14);
            ctx.bezierCurveTo(hwidth + stepW*2-stepW/4-1, stepH*7,
                              hwidth + stepW*2-stepW/4-1, stepH*7,
                              hwidth + stepW*2-stepW/2-stepW/4, stepH*11);
            ctx.stroke()

            ctx.beginPath()
            ctx.lineWidth = 1;
            // ctx.moveTo(hwidth + stepW*7, stepH*7);
            // ctx.lineTo(hwidth + stepW*7, stepH*7-stepH);
            // ctx.lineTo(hwidth + stepW*4+stepW/2, stepH*7-stepH/2);
            // ctx.lineTo(hwidth + stepW*4+stepW/2, stepH*7+stepH/2);
            // ctx.moveTo(hwidth - stepW*7, stepH*7);
            // ctx.lineTo(hwidth - stepW*7, stepH*7-stepH);
            // ctx.lineTo(hwidth - stepW*5+stepW/2, stepH*7-stepH/2);
            // ctx.lineTo(hwidth - stepW*5+stepW/2, stepH*7+stepH/2);
            ctx.moveTo(hwidth + stepW*7, stepH*7);
            ctx.lineTo(hwidth + stepW*7, stepH*7-stepH);
            ctx.lineTo(hwidth + stepW*4+stepW/2, stepH*7-stepH/2);
            ctx.lineTo(hwidth + stepW*4+stepW/2, stepH*7+stepH/2);
            ctx.moveTo(hwidth + stepW*7, stepH*7-stepH/2);
            ctx.lineTo(hwidth + stepW*4+stepW/2, stepH*7);
            ctx.moveTo(hwidth - stepW*7, stepH*7);
            ctx.lineTo(hwidth - stepW*7, stepH*7-stepH);
            ctx.lineTo(hwidth - stepW*5+stepW/2, stepH*7-stepH/2);
            ctx.lineTo(hwidth - stepW*5+stepW/2, stepH*7+stepH/2);
            ctx.moveTo(hwidth - stepW*7, stepH*7-stepH/2);
            ctx.lineTo(hwidth - stepW*5+stepW/2, stepH*7);
            ctx.stroke()

            ctx.beginPath()
            ctx.lineWidth = 2;
            ctx.moveTo(hwidth - stepW*2+stepW/2+stepW/4, stepH*11);
            ctx.arc(hwidth, stepH*11, stepW+stepW/4,Math.PI, 0, false);
            ctx.stroke()

            ctx.beginPath()
            ctx.lineWidth = 1;
            ctx.strokeStyle=frontWheelColor;
            ctx.fillStyle = frontWheelColor;
            ctx.moveTo(hwidth, stepH*3-stepW/4);
            ctx.lineTo(hwidth - 10, stepH*3+stepW/4);
            ctx.lineTo(hwidth + 10, stepH*3+stepW/4);
            ctx.lineTo(hwidth, stepH*3-stepW/4);
            ctx.fill();
            ctx.stroke()

            ctx.beginPath()
            ctx.strokeStyle=leftWheelColor;
            ctx.fillStyle = leftWheelColor;
            ctx.lineWidth = 1;
            ctx.moveTo(hwidth - stepW, stepH*6-10);
            ctx.lineTo(hwidth - stepW-10, stepH*6);
            ctx.lineTo(hwidth - stepW+10, stepH*6);
            ctx.lineTo(hwidth - stepW, stepH*6-10);
            ctx.strokeRect(hwidth - stepW-10, stepH*6+10, 20, stepH*2)
            ctx.fill();
            ctx.stroke()

            ctx.beginPath()
            ctx.strokeStyle=leftBrakeColor;
            ctx.fillStyle = leftBrakeColor;
            ctx.lineWidth = 1;
            var lrate = lwheelRate * stepH*2
            ctx.fillRect(hwidth - stepW-10, stepH*8+10 - lrate, 20, lrate)
            ctx.stroke()

            ctx.beginPath()
            ctx.strokeStyle=rightWheelColor;
            ctx.fillStyle = rightWheelColor;
            ctx.lineWidth = 1;
            ctx.moveTo(hwidth + stepW, stepH*6-10);
            ctx.lineTo(hwidth + stepW-10, stepH*6);
            ctx.lineTo(hwidth + stepW+10, stepH*6);
            ctx.lineTo(hwidth + stepW, stepH*6-10);
            ctx.strokeRect(hwidth + stepW-10, stepH*6+10, 20, stepH*2)
            ctx.fill();
            ctx.stroke()

            ctx.beginPath()
            ctx.strokeStyle=rightBrakeColor;
            ctx.fillStyle = rightBrakeColor;
            ctx.lineWidth = 1;
            var rrate = rwheelRate * stepH*2
            ctx.fillRect(hwidth + stepW-10, stepH*8+10 - rrate, 20, rrate)
            ctx.stroke()

            ctx.beginPath()
            ctx.strokeStyle= engineSpdColor;
            ctx.fillStyle = engineSpdColor;
            ctx.lineWidth = 2;
            var locx = (stepW)*Math.sin(3.14*3/2 - Math.PI * phyRSpeedRate);
            var locy = (stepW)*Math.cos(3.14*3/2 - Math.PI * phyRSpeedRate);
            ctx.moveTo(hwidth-3, stepH*11 + 2);
            ctx.lineTo(hwidth + locx, stepH*11 + locy);
            ctx.lineTo(hwidth+3, stepH*11 - 2);
            ctx.lineTo(hwidth-3, stepH*11 + 2);
            ctx.arc(hwidth, stepH*11, 6, -Math.PI, Math.PI, false);
            ctx.fill()
            ctx.stroke()

            ctx.beginPath()
            ctx.strokeStyle= GcsTheme.primaryColor;
            ctx.fillStyle = GcsTheme.primaryColor;

            if (Math.abs(leftElevatorVal) > rundderElevatorFlatRes){
                if (leftElevatorVal < -rundderElevatorFlatRes) {
                    ctx.fillRect(hwidth - stepW*2+15, stepH*14, stepW*2-20, stepH/2)
                } else if(leftElevatorVal > rundderElevatorFlatRes) {
                    ctx.fillRect(hwidth - stepW*2+15, stepH*14+stepH/2, stepW*2-20, stepH/2)
                }
            }

            ctx.fill()
            ctx.stroke()

            ctx.beginPath()
            ctx.strokeStyle= GcsTheme.primaryColor;
            ctx.fillStyle = GcsTheme.primaryColor;

            if (Math.abs(rightElevatorVal) > rundderElevatorFlatRes){
                if (rightElevatorVal < -rundderElevatorFlatRes) {
                    ctx.fillRect(hwidth + 5, stepH*14, stepW*2 -20, stepH/2)
                } else if(rightElevatorVal > rundderElevatorFlatRes) {
                    ctx.fillRect(hwidth + 5, stepH*14+stepH/2, stepW*2 -20, stepH/2)
                }
            }

            ctx.fill()
            ctx.stroke()


            ctx.beginPath()
            ctx.strokeStyle= GcsTheme.primaryColor;
            ctx.fillStyle = GcsTheme.primaryColor;

            if (Math.abs(rightRudderVal) > rundderElevatorFlatRes){
                if (rightRudderVal > 0.01) {
                    ctx.fillRect(hwidth + stepW*2-10, stepH*13, 10, stepH*2)
                } else if(rightRudderVal < -0.01) {
                    ctx.fillRect(hwidth + stepW*2, stepH*13, 10, stepH*2)
                }
            }
            ctx.fill()
            ctx.stroke()

            ctx.beginPath()
            ctx.strokeStyle= GcsTheme.primaryColor;
            ctx.fillStyle = GcsTheme.primaryColor;

            if (Math.abs(leftRudderVal) > rundderElevatorFlatRes){
                if (leftRudderVal > rundderElevatorFlatRes) {
                    ctx.fillRect(hwidth - stepW*2-10, stepH*13, 10, stepH*2)
                } else if(leftRudderVal < -rundderElevatorFlatRes) {
                    ctx.fillRect(hwidth - stepW*2, stepH*13, 10, stepH*2)
                }
            }

            ctx.fill()
            ctx.stroke()

            ctx.beginPath()
            ctx.strokeStyle= GcsTheme.primaryColor;
            ctx.fillStyle = GcsTheme.primaryColor;

            if (Math.abs(lFlapPosition) > rundderElevatorFlatRes){
                if (lFlapPosition < -rundderElevatorFlatRes) {
                    ctx.moveTo(hwidth - stepW*7, stepH*7-stepH/2);
                    ctx.lineTo(hwidth - stepW*7, stepH*7-stepH);
                    ctx.lineTo(hwidth - stepW*5+stepW/2, stepH*7-stepH/2);
                    ctx.lineTo(hwidth - stepW*5+stepW/2, stepH*7);
                    ctx.lineTo(hwidth - stepW*7, stepH*7-stepH/2);
                } else if(lFlapPosition > rundderElevatorFlatRes) {
                    ctx.moveTo(hwidth - stepW*7, stepH*7);
                    ctx.lineTo(hwidth - stepW*7, stepH*7-stepH/2);
                    ctx.lineTo(hwidth - stepW*5+stepW/2, stepH*7);
                    ctx.lineTo(hwidth - stepW*5+stepW/2, stepH*7+stepH/2);
                    ctx.lineTo(hwidth - stepW*7, stepH*7);
                }
            }
            ctx.fill()
            ctx.stroke()
            ctx.beginPath()
            ctx.strokeStyle= GcsTheme.primaryColor;
            ctx.fillStyle = GcsTheme.primaryColor;

            if (Math.abs(rFlapPosition) > rundderElevatorFlatRes){
                if (rFlapPosition < -rundderElevatorFlatRes) {
                    ctx.moveTo(hwidth + stepW*7, stepH*7-stepH/2);
                    ctx.lineTo(hwidth + stepW*7, stepH*7-stepH);
                    ctx.lineTo(hwidth + stepW*4+stepW/2, stepH*7-stepH/2);
                    ctx.lineTo(hwidth + stepW*4+stepW/2, stepH*7);
                    ctx.lineTo(hwidth + stepW*7, stepH*7-stepH/2);
                } else if(rFlapPosition > rundderElevatorFlatRes) {
                    ctx.moveTo(hwidth + stepW*7, stepH*7);
                    ctx.lineTo(hwidth + stepW*7, stepH*7-stepH/2);
                    ctx.lineTo(hwidth + stepW*4+stepW/2, stepH*7);
                    ctx.lineTo(hwidth + stepW*4+stepW/2, stepH*7+stepH/2);
                    ctx.lineTo(hwidth + stepW*7, stepH*7);
                }
            }

            ctx.fill()
            ctx.stroke()

            ctx.beginPath()
            ctx.textAlign = "center";
            ctx.fillStyle = GcsTheme.fontPrimaryColor;
            ctx.fillText(leftElevatorVal, hwidth - stepW, stepH*15+stepH/2+5);
            ctx.fillText(rightElevatorVal, hwidth + stepW, stepH*15+stepH/2+5);
            ctx.fillText(leftRudderVal, hwidth - stepW*3, stepH*14+stepH/2+5);
            ctx.fillText(rightRudderVal, hwidth + stepW*3, stepH*14+stepH/2+5);
            ctx.fillText(lFlapPosition, hwidth - stepW*6, stepH*8);
            ctx.fillText(rFlapPosition, hwidth + stepW*6, stepH*8);
            ctx.fillText(leftBrake.display, hwidth - stepW*2, stepH*7);
            ctx.fillText(rightBrake.display, hwidth + stepW*2, stepH*7);
            ctx.fillText(frontWheelAngle.display, hwidth, stepH*4);
            ctx.fillText(phyRSpeed.display, hwidth, stepH*12);
            ctx.stroke()
            ctx.closePath()

        }

        onPaint: {
            var ctx = getContext("2d");
            var width = vehCanvas.width
            var height = vehCanvas.height
            ctx.clearRect(0, 0, width, height);
            ctx.save()
            drawVehicle(ctx, width, height, 2022);
            ctx.restore()
        }
    }
}
