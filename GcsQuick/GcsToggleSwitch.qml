import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Button {
    property bool disabled: false
    property string contentDescription: ""
    property color checkColor: GcsTheme.primaryColor
    property color normalColor: GcsTheme.frameColor
    property color borderNormalColor: GcsTheme.windowBorderColor
    property color borderCheckColor: GcsTheme.primaryColor
    property real textSpacing: 6
    property bool textRight: true
    property alias textColor: btn_text.textColor
    property var clickListener : function(){
        checked = !checked
    }

    id: control
    Accessible.role: Accessible.Button
    Accessible.name: control.text
    Accessible.description: contentDescription
    Accessible.onPressAction: control.clicked()
    enabled: !disabled
    focusPolicy: Qt.TabFocus
    onClicked: clickListener()
    padding: 0
    horizontalPadding: 3

    onCheckableChanged: {
        if(checkable){
            checkable = false
        }
    }

    background : Item {
        anchors.fill: parent
    }

    contentItem: Item {
        anchors.fill: parent
        Rectangle {
            id: iconArea
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: (parent.height-20)/2
            implicitWidth: 20
            implicitHeight: 20
            radius: 10
            color: {
                if(checked){
                    return checkColor
                }
                return normalColor
            }
            border.width: 1
            border.color: {
                if(checked){
                    return borderCheckColor
                }
                return borderNormalColor
            }
        }
        GcsText{
            id:btn_text
            text: control.text
            height: parent.height
            anchors.left: iconArea.right
            anchors.leftMargin: textSpacing
            anchors.right: parent.right
            horizontalAlignment: Qt.AlignLeft
            visible: text !== ""
        }
    }
}
