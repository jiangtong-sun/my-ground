import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Item {
    property int radius: 4
    property int borderWidth: 2
    property color borderColor: GcsTheme.windowBorderColor

    id:control
    anchors.fill: parent

    Rectangle{
        width: control.width
        height: control.height
        anchors.centerIn: parent
        color: "#00000000"
        radius: control.radius
        border.width: borderWidth
        border.color: borderColor
    }
}
