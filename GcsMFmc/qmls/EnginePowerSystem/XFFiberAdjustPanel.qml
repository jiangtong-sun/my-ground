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
    property int opticalItemH: 32
    property int opticalLWidth: 110
    property int opticalMWidth: (ctrlWidth - 90)/2
    property int opticalMHeight: (ctrlHeight - 40)/2
    property int opticalRWidth: opticalMWidth - 110
    property var fiberoptSrc: vechicleRC.commands.cmdAdjusts
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
        anchors.right: srcAck.left
        anchors.rightMargin: 10
        anchors.topMargin: 10
        anchors.bottomMargin: 20
        anchors.leftMargin: 10

        Grid{
            anchors.fill: parent
            spacing: 15
            columns: 2
            Repeater {
                model: fiberoptSrc?fiberoptSrc.adjustCmds:null
                delegate: Item {
                    property var parentObject: object
                    property var metaModel: object.moreMetaModel

                    width: opticalMWidth
                    height: opticalMHeight

                    GcsToggleButton {
                        text: parentObject.name
                        checkeState: parentObject.working
                        width: opticalMWidth
                        height: opticalItemH
                    }

                    Column {
                        anchors.fill: parent
                        anchors.topMargin: opticalItemH
                        Repeater {
                            model: metaModel
                            Row {
                                property var childObject: object
                                property int isEnum: childObject.isEnum
                                property var enumValStr: childObject.enumValStr
                                property int rowCounts: isEnum ? enumValStr.length : 1
                                property int rowHeights: opticalItemH * rowCounts
                                spacing: 0
                                GcsButton {
                                    txtHorzAlign: Text.AlignLeft
                                    text: childObject.name
                                    width: opticalLWidth
                                    height: rowHeights
                                }
                                Column {
                                    height: rowHeights
                                    width: opticalRWidth
                                    visible: isEnum
                                    Repeater{
                                        model: enumValStr
                                        GcsToggleButton {
                                            id: cmdObjBtn
                                            height: opticalItemH
                                            width: opticalRWidth
                                            text: modelData
                                            checkeState: {
                                                if(curAdjustItem === parentObject &&
                                                    childObject.enumSelect === index) {
                                                    return true
                                                }
                                                return false
                                            }
                                            onClicked: {
                                                if (curAdjustItem && curAdjustItem !== parentObject) {
                                                    curAdjustItem.working = false
                                                }
                                                curAdjustItem = parentObject
                                                curAdjustItem.working = true
                                                childObject.enumSelect = index
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
                                ControlBackground {
                                    visible: !isEnum
                                    width: opticalRWidth
                                    height: rowHeights
                                    GcsTextBox{
                                        id: intput
                                        validator: intValidator
                                        anchors.fill: parent
                                        onCommit: {
                                            childObject.setRawValue(intput.text.trim());
                                            sendAckOperate();
                                        }
                                        onActiveFocusChanged: {
                                            if (intput.text.length > 0) {
                                                childObject.setRawValue(intput.text.trim());
                                                curAdjustItem = parentObject
                                                curAdjustItem.working = true
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
}
