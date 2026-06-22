import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Button {
    property int radiusVal: 0
    property bool disabled: false
    property int btnFontSize: TextStyle.Body
    property int txtHorzAlign: Text.AlignHCenter
    property color normalColor: GcsTheme.itemNormalColor
    property color hoverColor: GcsTheme.itemPressColor
    property color textColor: GcsTheme.fontPrimaryColor

    Accessible.role: Accessible.Button
    Accessible.name: control.text
    Accessible.onPressAction: control.clicked()

    id: control

    focus: false
    enabled: !disabled
    verticalPadding: 0
    horizontalPadding:12
    font.pixelSize: TextStyle.Body
    focusPolicy:Qt.TabFocus

    background: ControlBackground{
        id: controlBk
        anchors.fill: parent
        radius: radiusVal
        borderWidth: 1
        borderColor: hovered?hoverColor:normalColor
    }

    contentItem: GcsText {
        text: control.text
        defFontSize: btnFontSize
        horizontalAlignment: txtHorzAlign
        verticalAlignment: Text.AlignVCenter
        color: control.textColor
    }
}
