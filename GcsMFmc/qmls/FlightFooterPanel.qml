import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    id: basebar

    property var lastControlCmd: null
    property var commandGroup: vechicleRC.commands.commandGroup
    property int vWidth: (parent.width - headHeight * 2 - 10 )/12 - 5
    property int vHeight: headHeight - 10

    Item{
        id: statebar
        anchors.right: parent.right
        anchors.top: parent.top
        height: headHeight
        width: headHeight
    }

    Item{
        id: setbar
        anchors.left: parent.left
        anchors.top: parent.top
        height: headHeight
        width: headHeight
    }

    Row {
        spacing: 5
        height: headHeight
        anchors.top: parent.top
        anchors.right: statebar.left
        anchors.left: setbar.right
        anchors.topMargin: 4
        Repeater{
            model: sysCmdGroup?sysCmdGroup.commands:null
            delegate: GcsToggleButton {
                width: vWidth
                height: vHeight
                text: object.name
                normalColor: {
                    if (object.feed <= 0) {
                        return "transparent"
                    }

                    if(checkeState){
                        return GcsTheme.primaryColor
                    } else{
                        return GcsTheme.itemNormalColor
                    }
                }
                checkeState: object.working
                onClicked: {
                    object.working = true
                    if (object.popup > 0) {
                        simpleMsgDialog.title = qsTr("发送确认");
                        simpleMsgDialog.message = qsTr("是否发送【")+ object.name+qsTr("】指令");
                        simpleMsgDialog.iconSource= ttfIcons["warning"]
                        simpleMsgDialog.onSettingClickListener = ()=>{
                            commandMgr.sendControlCommand(object);
                            simpleMsgDialog.close()
                        }
                        simpleMsgDialog.onCancelClickListener= ()=>{
                            simpleMsgDialog.close()
                            object.working = false
                        }
                        simpleMsgDialog.open()
                    }
                    else {
                        commandMgr.sendControlCommand(object);
                    }
                }
            }
        }
    }
}
