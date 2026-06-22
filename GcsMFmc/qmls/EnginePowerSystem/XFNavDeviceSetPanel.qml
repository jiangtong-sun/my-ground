import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    id: control

    property var curAdjustItem
    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property int opticalItemH: 38
    property int opticalLWidth: (ctrlWidth-100)/4
    property var inforSource: vechicleRC.commands.cmdAdjusts
    property var intValidator: RegularExpressionValidator{regularExpression: /^[0-9]{3}$/}

    function sendAckOperate() {
        sendBtn.focus = true

        if (!noticeStationRole()) {
            if (curAdjustItem) {
                curAdjustItem.working = false
            }
        }

        if (curAdjustItem) {
            commandMgr.sendDeviceCommand(curAdjustItem);
            curAdjustItem = null
        }
    }

    Item {
        id: srcAck
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.rightMargin: 20
        height: 100
        width: 40
        GcsFilledButton {
            id: sendBtn
            anchors.fill: parent
            text: qsTr("发\n送\n确\n认")
            onClicked: {
                sendAckOperate()
            }
        }
    }

    Item {
        id: srcSet
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: 10
        anchors.bottomMargin: 20
        anchors.leftMargin: 10
        anchors.right: srcAck.left
        anchors.rightMargin: 10

        Column{
            anchors.fill: parent
            spacing: 10
            Repeater {
                model: inforSource ? inforSource.adjustCmds: null
                delegate: ControlBackground {
                    property var parentObject: object
                    property var enumValStr: parentObject.enumValStr
                    property int countRow: Math.ceil(enumValStr.length/3)

                    width: parent.width
                    height: opticalItemH * countRow
                    GcsToggleButton {
                        text: parentObject.name
                        checkeState: curAdjustItem === parentObject?true:false
                        height: opticalItemH * countRow
                        width: opticalLWidth
                    }
                    Grid{
                        columns: 3
                        anchors.fill: parent
                        anchors.leftMargin: opticalLWidth
                        Repeater {
                            model: enumValStr
                            delegate: Item {
                                height: opticalItemH
                                width: opticalLWidth
                                GcsToggleSwitch {
                                    id: cmdObjBtn
                                    text: modelData
                                    anchors.fill: parent
                                    checked: {
                                        if (curAdjustItem === parentObject &&
                                                curAdjustItem.selectId === index) {
                                            return true
                                        }
                                        return false
                                    }
                                    clickListener: () => {
                                        if (curAdjustItem) {
                                           curAdjustItem.selectId = -1
                                        }
                                        curAdjustItem = parentObject
                                        curAdjustItem.selectId = index
                                        cmdObjBtn.focus = true
                                    }
                                    Keys.onPressed:{
                                        if (event.key === Qt.Key_Return ||
                                                event.key === Qt.Key_Enter) {
                                            event.accepted = true
                                            sendAckOperate();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
