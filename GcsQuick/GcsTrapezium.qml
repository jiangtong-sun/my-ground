import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Button {
    property bool disabled: false
    property string contentDescription: ""
    property color normalColor: GcsTheme.itemNormalColor
    property color hoverColor: GcsTheme.itemHoverColor
    property color textColor: {
        if(GcsTheme.dark){
            if(!enabled){
                return Qt.rgba(131/255,131/255,131/255,1)
            }
            if(pressed){
                return Qt.rgba(162/255,162/255,162/255,1)
            }
            return Qt.rgba(1,1,1,1)
        }else{
            if(!enabled){
                return Qt.rgba(160/255,160/255,160/255,1)
            }
            if(pressed){
                return Qt.rgba(96/255,96/255,96/255,1)
            }
            return Qt.rgba(0,0,0,1)
        }
    }

    Accessible.role: Accessible.Button
    Accessible.name: control.text
    Accessible.description: contentDescription
    Accessible.onPressAction: control.clicked()

    id: control

    enabled: !disabled
    verticalPadding: 0
    horizontalPadding:12
    font.pixelSize: TextStyle.Body
    focusPolicy:Qt.TabFocus

    background: Item {
        implicitWidth: 30
        implicitHeight: 30
    }

    contentItem: GcsText {
        text: control.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: control.textColor
    }
}
