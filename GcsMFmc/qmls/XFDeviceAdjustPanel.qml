import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property int columnFirst: 3
    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property var firstModePart: null
    property var selectCmdItem: null
    property var subGroupStr: null
    property int navItemHeight: 36
    property int navItemWidth: (ctrlWidth-100)/3
    property int contentWidth: (ctrlWidth-100)/3
    property var controlRate: vechicleRC.commands.cmdAdjusts
    id: root

    function sendAckOperate() {
        ackButn.focus = true
        /// 判断系统角色
        if (!noticeStationRole()) {
            if (selectCmdItem) {
                selectCmdItem.working = false
            }
        }

        if (selectCmdItem) {
            commandMgr.sendSimpleCommand(selectCmdItem);
            selectCmdItem = null
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
            id: ackButn
            anchors.fill: parent
            text: qsTr("发\n送\n确\n认")
            onClicked: {
                sendAckOperate();
            }
        }
    }

    Item {
        id: common
        anchors.top: parent.top
        anchors.right: srcAck.left
        anchors.topMargin: 10
        anchors.rightMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        width: ctrlWidth-100
        Grid {
            columns: columnFirst
            spacing: 5

            Repeater {
                model: controlRate ? controlRate.adjustCmds : null
                delegate: ControlBackground {
                    property var blockObj: object
                    property var enumValStr: object.enumValStr
                    width: contentWidth
                    height: navItemHeight* (enumValStr.length+1)
                    GcsButton {
                        id: namegroup
                        width: contentWidth
                        height: navItemHeight
                        anchors.top: parent.top
                        text: blockObj.name
                    }
                    Column {
                        anchors.top: parent.top
                        anchors.topMargin: navItemHeight
                        height: navItemHeight * enumValStr.length
                        width: contentWidth
                        Repeater{
                            model: enumValStr
                            GcsToggleButton {
                                id: cmdObjBtn
                                height: navItemHeight
                                width: contentWidth
                                text: modelData
                                checkeState: (selectCmdItem===blockObj)&&(blockObj.selectId === index) ? true: false
                                onClicked: {
                                    if (selectCmdItem != blockObj && selectCmdItem) {
                                        selectCmdItem.selectId = -1
                                    }
                                    blockObj.selectId = index
                                    selectCmdItem = blockObj
                                    cmdObjBtn.focus = true
                                }
                                Keys.onPressed:{
                                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
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
