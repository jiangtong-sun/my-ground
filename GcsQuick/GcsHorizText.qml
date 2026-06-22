import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Item {
    property string labelValue: ""
    property string textValue: ""
    property int textFontSize: TextStyle.Body
    property int leftWidth: control.width/2
    property int labelAlign: Text.AlignRight
    property int valueAlign: Text.AlignLeft
    property color textDefColor: GcsTheme.fontPrimaryColor
    property color valueDefColor: GcsTheme.fontPrimaryColor

    id: control

    Item {
        width: leftWidth
        height: control.height
        anchors.left: parent.left
        GcsText {
            defFontSize: textFontSize
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
            defFontSize: textFontSize
            textColor: valueDefColor
            text: control.textValue
            anchors.fill: parent
            horizontalAlignment: valueAlign
        }
    }
}
