import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Button {
    property real size: 20
    property bool disabled: false
    property string contentDescription: ""
    property var iconSourceAccept: null
    property var iconSourceNormal: null
    property color normalColor: GcsTheme.itemNormalColor
    property color checkedColor: GcsTheme.primaryColor
    property color borderNormalColor: GcsTheme.windowBorderColor
    property color bordercheckedColor: Qt.lighter(borderNormalColor,1.3)
    property color borderDisableColor: Qt.darker(borderNormalColor,1.3)
    property color checkedDisableColor: Qt.darker(checkedColor,1.3)
    property alias textColor: btn_text.textColor
    property bool textRight: true
    property real textSpacing: 6
    property bool indeterminate : false
    property var clickListener : function(){
        checked = !checked
    }

    id:control
    enabled: !disabled
    onClicked: clickListener()
    onCheckableChanged: {
        if(checkable){
            checkable = false
        }
    }
    background: Item{ }
    focusPolicy:Qt.TabFocus
    horizontalPadding:0
    verticalPadding: 0
    padding: 0
    Accessible.role: Accessible.Button
    Accessible.name: control.text
    Accessible.description: contentDescription
    Accessible.onPressAction: control.clicked()
    contentItem: Row{
        spacing: control.textSpacing
        layoutDirection:control.textRight ? Qt.LeftToRight : Qt.RightToLeft
        anchors.topMargin: (control.height-control.size)/2
        anchors.top: parent.top
        Rectangle{
            Layout.alignment: Qt.AlignVCenter
            width: control.size
            height: control.size
            radius: 4
            border.width: 1
            border.color: {
                if(!enabled){
                    return borderDisableColor
                }
                if(checked){
                    return bordercheckedColor
                }
                return borderNormalColor
            }
            color: {
                if(checked){
                    if(!enabled){
                        return checkedDisableColor
                    }
                    return checkedColor
                }
                if(!enabled){
                    return checkedDisableColor
                }
                return normalColor
            }
            GcsIcon {
                anchors.centerIn: parent
                iconSource: iconSourceNormal
                iconSize: 14
                visible: indeterminate
                iconColor: GcsTheme.dark ? Qt.rgba(0,0,0,1) : Qt.rgba(1,1,1,1)
            }
            GcsIcon {
                anchors.centerIn: parent
                iconSource: iconSourceAccept
                iconSize: 14
                visible: checked && !indeterminate
                iconColor: GcsTheme.dark ? Qt.rgba(0,0,0,1) : Qt.rgba(1,1,1,1)
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
