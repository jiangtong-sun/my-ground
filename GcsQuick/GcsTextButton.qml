import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Button {
    property bool disabled: false
    property string labelValue: ""
    property string textValue: ""
    property int btnFontSize: TextStyle.Body
    property color normalColor: GcsTheme.itemNormalColor
    property color hoverColor: GcsTheme.itemPressColor
    property color textColor: GcsTheme.fontPrimaryColor
    property color labelColor: GcsTheme.primaryColor

    Accessible.role: Accessible.Button
    Accessible.name: control.text
    Accessible.description: labelValue
    Accessible.onPressAction: control.clicked()

    id: control

    enabled: !disabled
    verticalPadding: 0
    horizontalPadding:12
    font.pixelSize:TextStyle.Body
    focusPolicy:Qt.TabFocus

    background: ControlBackground{
        anchors.fill: parent
        borderColor: hovered ? hoverColor :normalColor
    }

    contentItem: Item {
        anchors.fill: parent
        GcsText {
            defFontSize: btnFontSize
            text: control.labelValue
            width: control.width/2
            height: control.height
            anchors.left: parent.left
            anchors.leftMargin: 5
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            color: control.labelColor
        }

        GcsText {
            defFontSize: btnFontSize
            width: control.width/2
            height: control.height
            anchors.right: parent.right
            anchors.rightMargin: 5
            text: control.textValue
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            color: control.textColor
        }
    }
}
