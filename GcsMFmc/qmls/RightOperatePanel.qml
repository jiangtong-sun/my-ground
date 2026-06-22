import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    id: root

    property int ctrlWidth: 40
    property int ctrlHeight: parent.height
    property int eachWidth: (ctrlWidth - 10)

    width: ctrlWidth
    height: ctrlHeight

    Item {
        id: ctrlPart
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 5

        ListView {
            model: commandTypes
            anchors.fill: parent
            clip: true
            spacing: 5
            delegate: GcsToggleButton {
                width: eachWidth
                height: lrCtlItemHeight
                checkeState: (rightMenuGroupSel>0)&&object.checked
                text: object.vertName
                onClicked: {
                    if (lastCmdSetGroup) {
                        lastCmdSetGroup.checked = false
                    }
                    lastCmdSetGroup = object
                    object.checked = !object.checked
                    var groupName = lastCmdSetGroup.groupName
                    vechicleRC.commands.updateSysCmdGroup(groupName)
                    vechicleRC.commands.uptExtCmdGroup(groupName)
                    vechicleRC.commands.selCmdGroup = index
                    if (currVehicle) { // 控制指令与车辆状态信息
                        currVehicle.setRcGroupName(groupName)
                    }
                    defaultMonitorPage()
                    rightMenuGroupSel = 2
                    selectCmdGroup = 1
                }
            }
        }
    }
}
