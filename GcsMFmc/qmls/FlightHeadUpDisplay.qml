import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item{
    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property real imageYPos: 0
    property real vehRoll:  0
    property real rotateDirect: configMgr.hudReferLevel
    property real partHeight: ctrlHeight/32
    property real drawHeight: ctrlHeight/2-40
    property real dWidth: ctrlWidth/21
    property real lrWidth: dWidth*6

    width: ctrlWidth
    height: ctrlHeight

    // Image {
    //     id: bkgImg
    //     scale: 1.5
    //     smooth: true
    //     width: ctrlWidth
    //     height: ctrlHeight
    //     anchors.fill: parent
    //     anchors.topMargin: -82
    //     rotation: vehRoll
    //     transformOrigin: Item.Center
    //     //fillMode: Image.PreserveAspectFit
    //     source: "qrc:///res/ackground.png"
    // }

    Item {
        width: ctrlWidth
        height: ctrlHeight
        anchors.fill: parent
        Canvas {
            id: bkgCanvas
            width: parent.width
            height: parent.height
            anchors.fill: parent
            onPaint: {
                var ctx = getContext("2d");
                var hoff = ctrlWidth/2*Math.tan(vehRoll*3.14/180)
                ctx.clearRect(0, 0, ctrlWidth, ctrlHeight);
                ctx.save();
                ctx.beginPath()
                ctx.strokeStyle = "deepskyblue";
                ctx.fillStyle = "deepskyblue";
                ctx.moveTo(0, 0);
                ctx.lineTo(0, drawHeight + imageYPos + hoff);
                ctx.lineTo(ctrlWidth, drawHeight+imageYPos - hoff);
                ctx.lineTo(ctrlWidth, 0);
                ctx.lineTo(0, 0);
                ctx.fill()
                ctx.stroke()
                ctx.restore();
                ctx.save();
                ctx.beginPath()
                ctx.strokeStyle = "sienna";
                ctx.fillStyle = "sienna";
                ctx.moveTo(0, drawHeight+imageYPos + hoff);
                ctx.lineTo(0, ctrlHeight);
                ctx.lineTo(ctrlWidth, ctrlHeight);
                ctx.lineTo(ctrlWidth, drawHeight+imageYPos - hoff);
                ctx.lineTo(0, drawHeight+imageYPos + hoff);
                ctx.fill()
                ctx.stroke()
                ctx.restore();
            }
        }
    }
    Connections{
        target: currVehicle?currVehicle.PitchAngle:null
        function onUpdateColorValue(value, color) {
            if (rotateDirect > 0) {
                return;
            }

            var pitch = (value === "NA") ? 0 : -parseFloat(value)
            var cpart = pitch * (partHeight * 3 / 10);
            imageYPos = -cpart
            if (cpart > 512) {
                imageYPos = -512;
            } else if(cpart < -512) {
                imageYPos = 512;
            }

            bkgCanvas.requestPaint()
        }
    }
    Connections{
        target: currVehicle?currVehicle.RollAngle:null
        function onUpdateColorValue(value, color) {
            vehRoll = value === "NA"?0: parseFloat(value)
        }
    }


    Component {
        id: cxfhudcentr
        XFFlightHUDCenterPanel{
            id: center
            anchors.fill: parent
        }
    }

    GcsLoader {
        sourceComponent: {
            return cxfhudcentr;
        }
    }
}


