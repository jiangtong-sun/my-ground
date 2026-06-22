import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Templates 2.15 as T
import GcsQuick 1.0

T.GroupBox {
    id: control
    property int radius: 4
    property int borderWidth : 1
    property color borderColor : GcsTheme.frameActiveColor
    property color color: GcsTheme.frameColor

    font.pixelSize: TextStyle.Body

    label: GcsText {
        width: control.width
        text: control.title
        //elide: Text.ElideRight
        //verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        anchors.fill: parent
        anchors.leftMargin: leftPadding
        anchors.rightMargin: rightPadding
        anchors.topMargin: topPadding
        anchors.bottomMargin: bottomPadding
        border.color: control.borderColor
        border.width: control.borderWidth
        color: control.color
    }
}
