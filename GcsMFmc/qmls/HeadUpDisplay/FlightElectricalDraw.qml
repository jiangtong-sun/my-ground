import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property var dataModel
    property int iWidth: 100
    property int iHeight: 100
    property string name: qsTr("")

    height: iHeight
    width: iWidth

    Row {
        anchors.fill: parent
        spacing: 10

        Item {
            height: iHeight
            width: iWidth/2

            GcsText{
                width: iWidth/2
                textColor: leftRightColor
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: name
            }
        }

        Repeater {
            model: dataModel
            Item {
                height: iHeight
                width: iWidth/4-10
                property double rateVal: 0

                Connections {
                    target: object
                    function onDrawValueChanged (val, color) {
                        rateVal = val
                        drawObj.requestPaint();
                    }
                }

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
}

