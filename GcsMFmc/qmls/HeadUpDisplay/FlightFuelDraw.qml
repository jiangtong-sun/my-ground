import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {

    property int iWidth: 100
    property int iHeight: 100
    property double rateVal: 1
    property var dataObject

    Connections {
        target: dataObject
        function onDrawValueChanged (val) {
            rateVal = val
            fuelChart.requestPaint();
        }
    }

    height: iHeight
    width: iWidth

    Row {
        anchors.fill: parent
        spacing: 10

        Item {
            height: iHeight
            width: iWidth/2-10

            GcsText{
                width: iWidth/2
                textColor: leftRightColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: dataObject.vertName
            }
        }

        Item {
            height: iHeight
            width: iWidth/2-10
            Canvas {
                id: fuelChart
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d");
                    var ewidth = parent.width
                    var eheight = parent.height
                    var dwidth = ewidth/4
                    var dheight = eheight/10
                    ctx.save();
                    ctx.clearRect(0, 0, ewidth, eheight);
                    ctx.font = frontName;
                    ctx.lineWidth = 2;
                    ctx.strokeStyle = GcsTheme.frameColor;
                    ctx.fillStyle = GcsTheme.itemNormalColor
                    ctx.beginPath()
                    ctx.fillRect(0, 0, ewidth, eheight);
                    ctx.stroke()
                    var curHeight = rateVal * eheight
                    ctx.fillStyle = GcsTheme.primaryColor;
                    ctx.beginPath()
                    ctx.fillRect(0, curHeight, ewidth, eheight - curHeight);
                    ctx.stroke()
                    ctx.restore();
                }
            }
        }
    }
}
