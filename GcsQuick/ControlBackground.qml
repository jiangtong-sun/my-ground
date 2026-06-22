import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Item{
    id:control

    property bool shadow: true

    property int radius: 1

    property int borderWidth: 1
    property var bottomMargin: undefined

    property color borderColor: GcsTheme.windowBorderColor
    property color color: GcsTheme.backgroundColor

    Rectangle{
        width: control.width
        height: control.height
        radius: control.radius
        border.width: borderWidth
        border.color: borderColor
        color: control.color
    }
}
