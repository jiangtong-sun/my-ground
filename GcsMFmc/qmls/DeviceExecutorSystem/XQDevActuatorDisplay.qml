import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property int ctrlWidth: 100
    property int ctrlHeight: 100
    property var elecSpdCtrl: currVehicle?currVehicle.elecSpdCtrl:null
    property var firstNode: elecSpdCtrl?elecSpdCtrl.firstNode: null
    property var secondNode: elecSpdCtrl?elecSpdCtrl.secondNode: null
    property var thirdNode: elecSpdCtrl?elecSpdCtrl.thirdNode: null
    property var fourNode: elecSpdCtrl?elecSpdCtrl.fourNode: null
    property var fiveNode: elecSpdCtrl?elecSpdCtrl.fiveNode: null
    property var sixNode: elecSpdCtrl?elecSpdCtrl.sixNode: null
    property var sevenNode: elecSpdCtrl?elecSpdCtrl.sevenNode: null
    property var eightNode: elecSpdCtrl?elecSpdCtrl.eightNode: null
    property var flapRunnderList: currVehicle?currVehicle.flapRunnder:null
    property var flapRunner0: flapRunnderList?flapRunnderList.firstNode: null

    property color drawLineColor: GcsTheme.primaryColor
    property string fontName: "16px bold Arial"

    Connections{
        target:commandMgr
        function onSignalDrawing () {
            vehCanvas.requestPaint()
        }
    }

    width: ctrlWidth
    height: ctrlHeight

    Canvas{
        id:vehCanvas
        anchors.fill: parent

        function drawDroplet(ctx, center, point) {
            var dx = point.x - center.x
            var dy = point.y - center.y
            var distance = Math.sqrt(dx * dx + dy * dy)
            var ux = dx / distance
            var uy = dy / distance
            var px = -uy
            var py = ux
            var expandFactor = 0.35
            var controlOffset = distance * expandFactor
            var midX = (center.x + point.x) / 2
            var midY = (center.y + point.y) / 2
            var control1 = { x: midX + px * controlOffset, y: midY + py * controlOffset}
            var control2 = { x: midX - px * controlOffset, y: midY - py * controlOffset}
            ctx.quadraticCurveTo(control1.x, control1.y, point.x, point.y )
            ctx.quadraticCurveTo( control2.x, control2.y,center.x, center.y)
        }

        function drawElectricFan(ctx, centerX, centerY, radius, length) {
            let p1x = centerX;
            let p1y = centerY-length;
            let p2x = centerX + Math.cos(3.14/6) * length;
            let p2y = centerY + Math.sin(3.14/6) * length;
            let p3x = centerX - Math.cos(3.14/6) * length;
            let p3y = centerY + Math.sin(3.14/6) * length;
            ctx.moveTo(centerX, centerY);
            ctx.arc(centerX, centerY, radius/2, 0, 3.14*2, false)
            ctx.moveTo(centerX, centerY);
            drawDroplet(ctx, {x: centerX, y: centerY}, {x: p1x, y: p1y})
            drawDroplet(ctx, {x: centerX, y: centerY}, {x: p2x, y: p2y})
            drawDroplet(ctx, {x: centerX, y: centerY}, {x: p3x, y: p3y})
        }

        function drawElectricFan2(ctx, centerX, centerY, radius, length) {
            let p1x = centerX;
            let p1y = centerY-length;
            let p2y = centerY+length;
            ctx.moveTo(centerX, centerY);
            ctx.arc(centerX, centerY, radius/2, 0, 3.14*2, false)
            ctx.moveTo(centerX, centerY);
            drawDroplet(ctx, {x: centerX, y: centerY}, {x: p1x, y: p1y})
            drawDroplet(ctx, {x: centerX, y: centerY}, {x: p1x, y: p2y})
        }

        function drawVehicle (ctx, width, height) {
            var hwidth = width/2
            var stepW = width/32
            var stepH = height/26
            var offx = 5

            ctx.save()
            ctx.beginPath();

            drawElectricFan(ctx, hwidth - 4.5 * stepW, stepH*4, stepW, stepH*2);
            drawElectricFan(ctx, hwidth + 4.5 * stepW, stepH*4, stepW, stepH*2);
            drawElectricFan(ctx, hwidth - 6 * stepW, stepH*14, stepW, stepH*2);
            drawElectricFan(ctx, hwidth + 6 * stepW, stepH*14, stepW, stepH*2);
            drawElectricFan2(ctx, hwidth - 10.5 * stepW, stepH*13, stepW, stepH*2);
            drawElectricFan2(ctx, hwidth + 10.5 * stepW, stepH*13, stepW, stepH*2);
            drawElectricFan2(ctx, hwidth - 10.5 * stepW, stepH*4, stepW, stepH*2);
            drawElectricFan2(ctx, hwidth + 10.5 * stepW, stepH*4, stepW, stepH*2);

            //header
            ctx.moveTo(hwidth + 2*stepW,stepH + 6*stepH);
            ctx.bezierCurveTo(hwidth + stepW/4, 0, hwidth - stepW/4, 0, hwidth - 2*stepW,stepH + 6*stepH);
            //second
            ctx.moveTo(hwidth - 2*stepW,stepH + 6*stepH);
            ctx.lineTo(hwidth - 4 * stepW,stepH + 6*stepH);
            ctx.lineTo(hwidth - 4 * stepW,stepH + 3*stepH);
            ctx.quadraticCurveTo(hwidth - 4.5 * stepW, 3*stepH,hwidth - 5*stepW,stepH + 3*stepH);
            if (secondNode) {
                var val21 = secondNode.electronic.itemName +qsTr(":") + secondNode.electronic.display
                var val22 = secondNode.variablepitch.itemName +qsTr(":") + secondNode.variablepitch.display
                var val23 = secondNode.tilting.itemName +qsTr(":") + secondNode.tilting.display
                ctx.fillText(val21, hwidth - 10 * stepW + offx, stepH * 2);
                ctx.fillText(val22, hwidth - 10 * stepW + offx, stepH * 3);
                ctx.fillText(val23, hwidth - 10 * stepW + offx, stepH * 4);
            }
            //first
            ctx.lineTo(hwidth - 5*stepW,stepH + 6*stepH);
            ctx.lineTo(hwidth - 10*stepW,stepH + 6*stepH);
            ctx.lineTo(hwidth - 10*stepW,stepH +  3*stepH);
            ctx.quadraticCurveTo(hwidth - 10.5*stepW, 3*stepH, hwidth - 11*stepW,stepH + 3*stepH);
            if (firstNode) {
                var val11 = firstNode.electronic.itemName +qsTr(":") + firstNode.electronic.display
                ctx.fillText(val11, hwidth - 16 * stepW + offx, stepH * 4);
            }
            //left side
            ctx.lineTo(hwidth - 11*stepW,stepH + 6*stepH);
            ctx.lineTo(hwidth - 14*stepW,stepH + 7*stepH);
            ctx.quadraticCurveTo(hwidth - 15*stepW, 7.5 * stepH, hwidth - 15*stepW,stepH + 9*stepH);

            //five node
            ctx.lineTo(hwidth - 11*stepW,stepH + 9*stepH);
            ctx.lineTo(hwidth - 11*stepW,stepH + 12*stepH);
            ctx.quadraticCurveTo(hwidth - 10.5*stepW,stepH + 13* stepH,hwidth - 10*stepW,stepH + 12* stepH);
            if (fiveNode) {
                var val51 = fiveNode.electronic.itemName +qsTr(":") + fiveNode.electronic.display
                ctx.fillText(val51, hwidth - 16 * stepW + offx, stepH * 13);
            }
            //level
            ctx.lineTo(hwidth - 10*stepW,stepH + 9*stepH);
            ctx.lineTo(hwidth - 6*stepW,stepH + 9*stepH);
            //flap
            ctx.moveTo(hwidth - 10*stepW,stepH + 9*stepH);
            ctx.lineTo(hwidth - 10*stepW,stepH + 8*stepH);
            ctx.lineTo(hwidth - 6*stepW,stepH + 8*stepH);
            ctx.lineTo(hwidth - 6*stepW,stepH + 9*stepH);
            ctx.quadraticCurveTo(hwidth - 3 * stepW, stepH + 9*stepH,hwidth - 2 * stepW - 5, stepH + 9.5 * stepH);
            if (flapRunner0) {
                var leftFlaps = flapRunner0.leftFlaps.itemName +qsTr(":") + flapRunner0.leftFlaps.display
                ctx.fillText(leftFlaps, hwidth - 10 * stepW + offx, stepH * 8);
            }
            //six
            ctx.quadraticCurveTo(hwidth - 2.5 * stepW , 13 * stepH, hwidth - 2* stepW, 16.5 * stepH);
            ctx.lineTo(hwidth - 5.5 * stepW,16 * stepH);
            ctx.lineTo(hwidth - 5.5 * stepW,14 * stepH);
            ctx.quadraticCurveTo(hwidth - 6 * stepW, 14 * stepH, hwidth - 6.5 * stepW, 14 * stepH);
            if (sixNode) {
                var val61 = sixNode.electronic.itemName +qsTr(":") + sixNode.electronic.display
                var val62 = sixNode.variablepitch.itemName +qsTr(":") + sixNode.variablepitch.display
                var val63 = sixNode.tilting.itemName +qsTr(":") + sixNode.tilting.display
                ctx.fillText(val61, hwidth -12 * stepW + offx, stepH * 17);
                ctx.fillText(val62, hwidth -12 * stepW + offx, stepH * 18);
                ctx.fillText(val63, hwidth -12 * stepW + offx, stepH * 19);
            }
            //
            ctx.lineTo(hwidth - 6.5 * stepW,18 * stepH);
            ctx.quadraticCurveTo(hwidth - 6 * stepW,20 * stepH,hwidth - 5.5 * stepW,18 * stepH);

            //aileron
            ctx.moveTo(hwidth - 5.5 * stepW, 18 * stepH);
            ctx.lineTo(hwidth - 5.5 * stepW, 17 * stepH);
            ctx.lineTo(hwidth - 3.5 * stepW, 17.5 * stepH);
            ctx.lineTo(hwidth - 3.5 * stepW, 18.5 * stepH);
            ctx.moveTo(hwidth - 5.5 * stepW, 18 * stepH);
            ctx.lineTo(hwidth - 1.5 * stepW, 19 * stepH);
            if (flapRunner0) {
                var leftAileron = flapRunner0.leftAileron.display
                ctx.fillText(leftAileron, hwidth - 5 * stepW + offx, stepH * 20);
            }
            //
            ctx.quadraticCurveTo(hwidth - 1*stepW,21 * stepH,hwidth,23 * stepH);
            ctx.strokeRect(hwidth - 0.25 *stepW,21 *stepH,stepW * 0.5,stepH * 2);
            ctx.strokeRect(hwidth - 0.25 *stepW,23 *stepH,stepW * 0.5,stepH * 2);
            ctx.moveTo(hwidth,24 * stepH);
            ctx.lineTo(hwidth,20 * stepH);
            ctx.moveTo(hwidth,23 * stepH);
            if (flapRunner0) {
                var leftRudder = flapRunner0.leftRudder.itemName +qsTr(":") + flapRunner0.leftRudder.display
                var rightRudder = flapRunner0.rightRudder.itemName +qsTr(":") + flapRunner0.rightRudder.display
                ctx.fillText(leftRudder, hwidth + stepW, stepH * 23);
                ctx.fillText(rightRudder, hwidth + stepW, stepH * 25);
            }
            ctx.quadraticCurveTo(hwidth + 1*stepW,21 * stepH,hwidth + 1.5*stepW,19 * stepH);
            ctx.lineTo(hwidth + 5.5*stepW, 18 * stepH);

            ///aileron
            ctx.moveTo(hwidth + 5.5*stepW, 18 * stepH);
            ctx.lineTo(hwidth + 5.5*stepW, 17 * stepH);
            ctx.lineTo(hwidth + 3.5*stepW, 17.5 * stepH);
            ctx.lineTo(hwidth + 3.5*stepW, 18.5 * stepH);
            if (flapRunner0) {
                var rightAileron = flapRunner0.rightAileron.display
                ctx.fillText(rightAileron, hwidth + 2 * stepW + offx, stepH * 20);
            }
            ctx.moveTo(hwidth + 5.5 * stepW,18 * stepH);
            ctx.quadraticCurveTo(hwidth + 6 * stepW,20 * stepH,hwidth + 6.5 * stepW,18 * stepH);
            ///serven
            ctx.lineTo(hwidth + 6.5 * stepW,14 * stepH);
            ctx.quadraticCurveTo(hwidth + 6 * stepW,14 * stepH,hwidth + 5.5 * stepW,14 * stepH);
            if (sevenNode) {
                var val71 = sevenNode.electronic.itemName +qsTr(":") + sevenNode.electronic.display
                var val72 = sevenNode.variablepitch.itemName +qsTr(":") + sevenNode.variablepitch.display
                var val73 = sevenNode.tilting.itemName +qsTr(":") + sevenNode.tilting.display
                ctx.fillText(val71, hwidth + 7 * stepW + offx, stepH * 17);
                ctx.fillText(val72, hwidth + 7 * stepW + offx, stepH * 18);
                ctx.fillText(val73, hwidth + 7 * stepW + offx, stepH * 19);
            }

            ctx.lineTo(hwidth + 5.5 * stepW,16 * stepH);
            ctx.lineTo(hwidth + 2 * stepW,16.5 * stepH);
            ctx.quadraticCurveTo(hwidth + 2.5 * stepW, 13 * stepH, hwidth + 2 * stepW + 5, stepH + 9.5* stepH);
            ctx.quadraticCurveTo(hwidth + 3 * stepW, stepH + 9*stepH,hwidth + 6 * stepW, stepH + 9 * stepH);
            //flap
            ctx.lineTo(hwidth + 10*stepW,stepH + 9*stepH);
            ctx.lineTo(hwidth + 10*stepW,stepH + 8*stepH);
            ctx.lineTo(hwidth + 6*stepW,stepH + 8*stepH);
            ctx.lineTo(hwidth + 6*stepW,stepH + 9*stepH);
            if (flapRunner0) {
                var rightFlaps = flapRunner0.rightFlaps.itemName +qsTr(":") +flapRunner0.rightFlaps.display
                ctx.fillText(rightFlaps, hwidth + 4 * stepW + offx, stepH * 8);
            }
            ///eight
            ctx.moveTo(hwidth + 10*stepW,stepH + 9*stepH);
            ctx.lineTo(hwidth + 10*stepW,stepH + 12*stepH);
            ctx.quadraticCurveTo(hwidth + 10.5*stepW,stepH + 13* stepH,hwidth + 11*stepW,stepH + 12* stepH);
            if (eightNode) {
                var val81 = eightNode.electronic.itemName +qsTr(":") + eightNode.electronic.display
                ctx.fillText(val81, hwidth + 11 * stepW + offx, stepH * 13);
            }

            //right side
            ctx.lineTo(hwidth + 11*stepW,stepH + 9*stepH);
            ctx.lineTo(hwidth + 15*stepW, stepH + 9*stepH);
            ctx.quadraticCurveTo(hwidth + 15*stepW, 7.5* stepH, hwidth + 14*stepW,stepH + 7*stepH);
            //four
            ctx.lineTo(hwidth + 11*stepW, stepH + 6*stepH);
            ctx.lineTo(hwidth + 11*stepW, stepH + 3*stepH);
            ctx.quadraticCurveTo(hwidth + 10.5*stepW, 3*stepH,hwidth + 10*stepW,stepH + 3*stepH);
            if (fourNode) {
                var val41 = fourNode.electronic.itemName +qsTr(":") + fourNode.electronic.display
                ctx.fillText(val41, hwidth + 11 * stepW + offx, stepH * 4);
            }
            // third
            ctx.lineTo(hwidth + 10*stepW, stepH + 6*stepH);
            ctx.lineTo(hwidth + 5*stepW, stepH + 6*stepH);
            ctx.lineTo(hwidth + 5*stepW, stepH +  3*stepH);
            ctx.quadraticCurveTo(hwidth + 4.5*stepW, 3*stepH, hwidth + 4*stepW,stepH + 3*stepH);
            if (thirdNode) {
                var val31 = thirdNode.electronic.itemName +qsTr(":") + thirdNode.electronic.display
                var val32 = thirdNode.variablepitch.itemName +qsTr(":") + thirdNode.variablepitch.display
                var val33 = thirdNode.tilting.itemName +qsTr(":") + thirdNode.tilting.display
                ctx.fillText(val31, hwidth + 5 * stepW + offx, stepH * 2);
                ctx.fillText(val32, hwidth + 5 * stepW + offx, stepH * 3);
                ctx.fillText(val33, hwidth + 5 * stepW + offx, stepH * 4);
            }
            ctx.lineTo(hwidth + 4*stepW, stepH + 6*stepH);
            ctx.lineTo(hwidth + 2*stepW, stepH + 6*stepH);
            ctx.restore()
            ctx.stroke()
        }

        onPaint: {
            var ctx = getContext("2d")
            var width = vehCanvas.width
            var height = vehCanvas.height

            ctx.font = fontName;
            ctx.strokeStyle = drawLineColor;
            ctx.fillStyle = GcsTheme.fontPrimaryColor;
            ctx.lineWidth = 1;

            ctx.clearRect(0,0,width,height)
            drawVehicle(ctx,width,height)
        }
    }
}
