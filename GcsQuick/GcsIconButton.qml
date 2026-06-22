import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Button {
    display: Button.IconOnly

    property bool disabled: false
    property int iconSize: 20
    property int radius:4
    property string iconSource
    property string contentDescription: ""
    property color hoverColor: GcsTheme.primaryColor
    property Component iconDelegate: com_icon

    property color iconColor: {
        if(GcsTheme.dark){
            return GcsTheme.White
        }else{
            return GcsTheme.Black
        }
    }

    property color textColor: GcsTheme.fontPrimaryColor

    Accessible.role: Accessible.Button
    Accessible.name: control.text
    Accessible.description: contentDescription
    Accessible.onPressAction: control.clicked()

    id:control

    focusPolicy:Qt.TabFocus

    padding: 0

    verticalPadding: 8
    horizontalPadding: 8

    enabled: !disabled

    font.pixelSize:TextStyle.Caption

    background: Rectangle {
        color: hovered ? hoverColor : "transparent"
    }

    Component{
        id:com_icon
        GcsIcon {
            id:text_icon
            font.pixelSize: iconSize
            iconSize: control.iconSize
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            iconColor: control.iconColor
            iconSource: control.iconSource
        }
    }
    Component{
        id:com_row
        RowLayout{
            GcsLoader{
                sourceComponent: iconDelegate
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                visible: display !== Button.TextOnly
            }
            GcsText{
                text:control.text
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                visible: display !== Button.IconOnly
                color: control.textColor
            }
        }
    }
    Component{
        id:com_column
        ColumnLayout{
            GcsLoader{
                sourceComponent: iconDelegate
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                visible: display !== Button.TextOnly
            }
            GcsText{
                text:control.text
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                visible: display !== Button.IconOnly
                color: control.textColor
            }
        }
    }

    contentItem: GcsLoader{
        sourceComponent: {
            if(display === Button.TextUnderIcon){
                return com_column
            }
            return com_row
        }
    }

    GcsTooltip{
        id:tool_tip
        visible: {
            if(control.text === ""){
                return false
            }
            if(control.display !== Button.IconOnly){
                return false
            }
            return hovered
        }
        text:control.text
        delay: 1000
    }
}
