import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    id: control
    property var selectCmdItem: null
    property var getAdjustItem: null
    property var curAdjustItem: null
    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property int opticalItemH: 38
    property int opticalLWidth: ctrlWidth/3
    property var inforSource: vechicleRC.commands.cmdAdjusts
    property var sysCmdGroup: vechicleRC.commands.sysCmdGroup

    function sendAckOperate() {
        sendBtn.focus = true
        if (!noticeStationRole()) {
            if (curAdjustItem) {
                curAdjustItem.working = false
            }
        }

        if (selectCmdItem) {
            commandMgr.sendControlCommand(selectCmdItem);
            selectCmdItem = null
            return
        }

        if (curAdjustItem) {
            commandMgr.sendDeviceCommand(curAdjustItem);
            curAdjustItem = null
            getAdjustItem = null
            return
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
        width: opticalLWidth

        Column{
            anchors.fill: parent
            spacing: 2
            Repeater {
                model: inforSource?inforSource.adjustCmds:null
                delegate: Item {
                    property var parentObject: object
                    width: opticalLWidth
                    height: opticalItemH
                    GcsToggleButton {
                        text: parentObject.name
                        checkeState: getAdjustItem === parentObject?true:false
                        width: opticalLWidth
                        height: opticalItemH
                        onClicked: {
                            if (selectCmdItem) {
                                selectCmdItem.working = false
                                selectCmdItem = null
                            }

                            getAdjustItem = parentObject
                            curAdjustItem = null
                        }
                    }
                }
            }
        }
    }

    Item {
        id: srcSetVal
        anchors.left: srcSet.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: srcAck.left
        anchors.rightMargin: 10
        anchors.topMargin: 10
        anchors.bottomMargin: 20
        anchors.leftMargin: 10

        Column{
            anchors.fill: parent
            spacing: 5
            Repeater {
                model: getAdjustItem?getAdjustItem.moreMetaModel: null
                delegate: ControlBackground {
                    property var subElem: object
                    property var enumValStr: subElem.enumValStr
                    property int rowCount: enumValStr.length
                    height: opticalItemH * rowCount
                    width: opticalLWidth
                    Column{
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        Repeater {
                            model: enumValStr
                            delegate: Item {
                                height: opticalItemH
                                width: opticalLWidth
                                GcsToggleSwitch {
                                    id: cmdObjBtn
                                    text: modelData
                                    anchors.fill: parent
                                    checked: curAdjustItem && subElem.enumSelect === index ? true: false
                                    clickListener: () => {
                                        curAdjustItem = getAdjustItem
                                        curAdjustItem.selectSub = 0 //
                                        subElem.enumSelect = index
                                        cmdObjBtn.focus = true
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // ControlBackground{
    //     anchors.right: parent.right
    //     anchors.rightMargin: 10
    //     anchors.top: srcAck.bottom
    //     anchors.bottom: parent.bottom
    //     anchors.bottomMargin: 10
    //     anchors.topMargin: 20
    //     width: opticalLWidth-40
    //     Column{
    //         anchors.fill: parent
    //         anchors.margins: 10
    //         spacing: 5
    //         Repeater{
    //             model: sysCmdGroup?sysCmdGroup.commands:null
    //             delegate: GcsToggleButton {
    //                 property var cmdObj: object
    //                 id: ctrlObjBtn
    //                 text: object.name
    //                 width: parent.width
    //                 height: opticalItemH
    //                 checkeState: object.working
    //                 onClicked: {
    //                     if (getAdjustItem) {
    //                         getAdjustItem = null
    //                     }
    //                     cmdObj.working = !cmdObj.working
    //                     selectCmdItem = cmdObj
    //                     ctrlObjBtn.focus = true
    //                 }
    //             }
    //         }
    //     }
    // }
}
