import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Button {
    property int radiusVal: 4
    property int paddingVal: 10
    property bool disabled: false
    property string contentDescription: ""
    property color normalColor: GcsTheme.primaryColor
    property color hoverColor: GcsTheme.dark ? Qt.darker(normalColor,1.1) : Qt.lighter(normalColor,1.1)
    property color textColor: GcsTheme.White
    property int pixelSize: TextStyle.Body

    Accessible.role: Accessible.Button
    Accessible.name: control.text
    Accessible.description: contentDescription
    Accessible.onPressAction: control.clicked()

    id: control

    enabled: !disabled
    focusPolicy:Qt.TabFocus
    font.pixelSize: pixelSize
    verticalPadding: 0
    horizontalPadding: paddingVal

    background: ControlBackground{
        anchors.fill: parent
        radius: radiusVal
        color: hovered ? hoverColor :normalColor
    }
    contentItem: GcsText {
        text: control.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        defFontSize: pixelSize
        color: control.textColor
    }
}
