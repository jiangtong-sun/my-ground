import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    id: root
    anchors.fill: parent
    property var firstModePart: null
    property var selectCmdItem: null
    property var subGroupStr: null
    property int navItemHeight: 36
    property int navItemWidth: 200
    property int contentWidth: 180
    property int columnFirst: 1

    property var controlRate: vechicleRC.commands.controlRateControl
    function setModePart(first, second, eachHeight, groupStr) {
        columnFirst = controlRate.columnSize
        navItemHeight = eachHeight
    }

    function sendAckOperate() {
        srcAck.focus = true
        if (selectCmdItem) {
            commandMgr.sendControlCommand(selectCmdItem);
            selectCmdItem = null
        }
    }

    Keys.onPressed:{
        console.log("onCompleted GcsControlRatePanel")
    }

    Component.onCompleted: {
        console.log("onCompleted GcsControlRatePanel")
        setModePart(null, null, 36, null)
    }

    Component.onDestruction: {
        console.log("onDestruction GcsControlRatePanel")
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
        width: columnFirst * navItemWidth

        Grid {
            anchors.fill: parent
            columns: columnFirst
            spacing: 10

            Repeater {
                model: controlRate ? controlRate.commands : null
                delegate: ControlBackground {
                    property var blockObj: object
                    property var subcommands: blockObj.subcommands
                    property int countRow: blockObj.counts
                    width: contentWidth
                    height: navItemHeight* (countRow+1)
                    GcsButton {
                        id: namegroup
                        width: contentWidth
                        height: navItemHeight
                        anchors.top: parent.top
                        text: blockObj.groupName
                    }
                    Column{
                        width: contentWidth
                        height: navItemHeight*countRow
                        anchors.top: namegroup.bottom
                        Repeater {
                            model: subcommands
                            delegate: GcsToggleButton {
                                property var elemObj: object
                                id: firstNode
                                width: contentWidth
                                height: navItemHeight
                                text: elemObj.name
                                checkeState: elemObj.working

                                onClicked: {
                                    if (selectCmdItem) {
                                        selectCmdItem.working = false
                                    }
                                    elemObj.working = !elemObj.working
                                    selectCmdItem = elemObj
                                    firstNode.focus = true

                                    console.log("onCompleted GcsControlRatePanel")
                                }
                                Keys.onPressed:{

                                    console.log("onCompleted GcsControlRatePanel")
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
