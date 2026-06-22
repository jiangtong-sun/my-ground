import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property var dataObject
    property int iWidth: 100
    property int iHeight: 100

    height: iHeight
    width: iWidth


    Row {
        anchors.fill: parent
        spacing: 3

        Item {
            height: iHeight
            width: iWidth/3
            GcsText{
                anchors.top: parent.top
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                textColor: leftRightColor
                text: dataObject.vertGroup
            }
        }

        Repeater {
            model: dataObject.dataModel
            Item {
                height: iHeight
                width: iWidth/3-10
                // property var displayObj: object
                // //property real currRateVal: 0
                // Connections{
                //     target: displayObj
                //     function onRateValChanged(value, color) {
                //         rateVal = val
                //         drawObj.requestPaint()
                //         console.log("onRateValChanged", value, color)
                //     }
                // }
                Canvas {
                    id: drawObj
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
                        ctx.strokeStyle = GcsTheme.frameColor
                        ctx.fillStyle = GcsTheme.itemNormalColor
                        ctx.beginPath()
                        ctx.fillRect(0, 0, ewidth, eheight);
                        ctx.stroke()
                        var curHeight = 0 * eheight
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
}
