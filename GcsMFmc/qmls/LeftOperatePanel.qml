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
    property int frameHeight: 400

    width: ctrlWidth
    height: ctrlHeight

    Item {
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.topMargin: 5
        anchors.bottomMargin: 5

        ListView{
            id: framePart
            anchors.fill: parent
            model: leftDeviceGroup ? leftDeviceGroup.dataModel : null
            spacing: 5
            delegate: GcsToggleButton {
                width: eachWidth
                height: lrCtlItemHeight
                text: object.vertGroup
                checkeState: rightMenuGroupSel === 0 && leftMenuGroupSel === index ? true: false
                onClicked: {
                    var mapGroupName = object.mapGroup
                    commandMgr.setDataFrame(mapGroupName)
                    vechicleRC.commands.updateSysCmdGroup(mapGroupName)
                    leftDeviceGroup.selectIndex = index
                    leftMenuGroupSel = index
                    rightMenuGroupSel = 0
                }
            }
        }
    }
}
