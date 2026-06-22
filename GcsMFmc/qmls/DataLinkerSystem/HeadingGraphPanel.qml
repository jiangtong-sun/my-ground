import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

ControlBackground {
    id: root
    anchors.left: pitchGraph.right
    height: parent.height
    width: parent.height

    Canvas {
        id: canvas
        anchors.fill: parent
        readonly property real circleStep: Math.PI / 12
        readonly property real circleRadian: Math.PI * 2
        readonly property int centerX: parent.width >> 1
        readonly property int centerY: parent.height >> 1
        readonly property var lineDash: [15, 2, 5, 2, 5, 2]

        onPaint: {
            let ctx = getContext("2d");
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            // draw circle
            ctx.beginPath();
            ctx.save();
            ctx.lineWidth = 1;
            ctx.strokeStyle = GcsTheme.primaryColor;
            ctx.arc(centerX, centerY, root.width >> 2, 0, circleRadian, true);
            ctx.stroke();
            ctx.restore();
            ctx.closePath();
            // draw cross line
            ctx.beginPath();
            ctx.save();
            ctx.lineWidth = 1;
            ctx.strokeStyle = GcsTheme.windowBorderColor;
            ctx.moveTo(0, centerY);
            ctx.lineTo(width, centerY);
            ctx.moveTo(centerX, 0);
            ctx.lineTo(centerX, height);
            ctx.stroke();
            ctx.restore();
            ctx.closePath();
            // draw dash line
            ctx.beginPath();
            ctx.save();
            ctx.setLineDash(lineDash);
            ctx.lineWidth = 1;
            ctx.strokeStyle = GcsTheme.windowBorderColor;
            let circleRadius = canvas.width
            let radian = 0.0;
            let x = 0.0
            let y = 0.0
            while (radian < circleRadian)
            {
                ctx.moveTo(centerX, centerY);
                ctx.lineTo(centerX - circleRadius * Math.cos(radian), centerY - circleRadius * Math.sin(radian));
                radian += circleStep;
            }

            ctx.stroke();
            ctx.restore();
            ctx.closePath();
        }
    }
}
