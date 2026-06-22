import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Button {
    property int iconSize: 24
    property bool iconState: true
    property color textColor: GcsTheme.fontPrimaryColor
    property color iconColor: GcsTheme.fontPrimaryColor
    property color hoverColor: GcsTheme.windowBorderColor

    Accessible.role: Accessible.Button
    Accessible.name: control.text
    Accessible.onPressAction: control.clicked()

    id: control
    leftPadding: 5
    rightPadding: 5
    font.pixelSize: TextStyle.Caption
    background: ControlBackground {
        width: control.width-20
        height: control.height
        borderColor: hovered ? iconColor : "transparent"
    }
    contentItem: Row {
        spacing: 5
        anchors.fill: parent
        anchors.topMargin: (control.height-iconSize)/2
        ControlBackground{
            width: iconSize
            height: iconSize
            color: iconState? iconColor: hoverColor
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            radius: iconSize/2
        }
        GcsText{
            text:control.text
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            color: control.textColor
        }
    }
}
