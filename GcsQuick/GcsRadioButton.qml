import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Button {
    property string contentDescription: ""
    property bool disabled: false
    property color checkColor: GcsTheme.primaryColor
    property color normalColor: GcsTheme.frameColor
    property color borderNormalColor: GcsTheme.windowBorderColor
    property color borderCheckColor: GcsTheme.primaryColor
    property alias textColor: btn_text.textColor
    property real size: 18
    property bool textRight: true
    property real textSpacing: 6

    property var clickListener : function(){
        checked = !checked
    }

    Accessible.role: Accessible.Button
    Accessible.name: control.text
    Accessible.description: contentDescription
    Accessible.onPressAction: control.clicked()

    id:control

    enabled: !disabled
    horizontalPadding:2
    verticalPadding: 2
    background: Item {

    }
    focusPolicy:Qt.TabFocus
    font.pixelSize: TextStyle.Body
    onClicked: clickListener()
    onCheckableChanged: {
        if(checkable){
            checkable = false
        }
    }
    contentItem: RowLayout{
        spacing: control.textSpacing
        layoutDirection:control.textRight ? Qt.LeftToRight : Qt.RightToLeft
        Rectangle{
            id:rect_check
            width: control.size
            height: control.size
            radius: size/2
            border.width: 1
            color: {
                if(checked){
                    return checkColor
                }

                return normalColor
            }
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
            Layout.alignment: Qt.AlignVCenter
            visible: text !== ""
        }
    }
}
