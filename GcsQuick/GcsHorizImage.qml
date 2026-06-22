import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Item {
    property string labelValue:  qsTr("")
    property string textValue:  qsTr("")
    property string valueUnit:  qsTr("")
    property real activeRate: 0
    property int showImage: 0
    property int leftWidth: control.width/3
    property int labelAlign: Text.AlignRight
    property color drawingColor: GcsTheme.fontPrimaryColor
    property color textDefColor: GcsTheme.fontPrimaryColor
    property color valueDefColor: GcsTheme.fontPrimaryColor
    function drawImage() { drawImg.requestPaint(); }
    id: control
    Item {
        width: leftWidth
        height: control.height
        anchors.left: parent.left
        GcsText {
            textColor: textDefColor
            text: control.labelValue
            anchors.rightMargin: 5
            anchors.fill: parent
            horizontalAlignment: labelAlign
        }
    }
    Item {
        width: control.width - leftWidth
        height: control.height
        anchors.right: parent.right
        GcsText {
            visible: showImage === 1?false:true
            text: control.textValue + valueUnit
            textColor: valueDefColor
            anchors.fill: parent
        }
        Row {
            visible: showImage === 1?true:false
            anchors.fill: parent
            Item {
                width: leftWidth
                height: parent.height
                Canvas {
                    id: drawImg
                    anchors.fill: parent
                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.clearRect(0, 0, drawImg.width, drawImg.height);
                        ctx.strokeStyle = drawingColor;
                        ctx.fillStyle = drawingColor;
                        ctx.lineWidth = 1;
                        ctx.save();
                        var actlen = activeRate * leftWidth;
                        ctx.strokeRect(0, drawImg.height/4, leftWidth, drawImg.height/2)
                        ctx.fillRect(0, drawImg.height/4, actlen, drawImg.height/2)
                        ctx.fill();
                        ctx.stroke()
                        ctx.restore();
                    }
                }
            }
            Item {
                width: leftWidth
                height: parent.height
                GcsText {
                    textColor: valueDefColor
                    text: control.textValue + valueUnit
                    anchors.fill: parent
                }
            }
        }
    }
}
