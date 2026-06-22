import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    id: control
    property int ctrlHeight: 400
    property var lastAdjustCmd: null
    property int deviceItemWidth: 200
    property var dataLinkDevSource: vechicleRC.commands.dataLinkDevSource

    function sendAckOperate() {
        /// 判断系统角色
        if (!noticeStationRole()) {
            if (lastAdjustCmd) {
                lastAdjustCmd.working = false
            }
        }
        if (lastAdjustCmd) {
            commandMgr.sendAdjustCommand(lastAdjustCmd);
            lastAdjustCmd = null
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
            id: btnAck
            anchors.fill: parent
            text: qsTr("发\n送\n确\n认")
            onClicked: {
                btnAck.focus = true
                sendAckOperate()
            }
        }
    }

    Item {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: srcAck.left
        anchors.rightMargin: 10
        anchors.topMargin: 10
        width: deviceItemWidth

        Column{
            anchors.fill: parent
            spacing: 5
            Repeater {
                model: dataLinkDevSource.adjustCmds
                delegate: ControlBackground {
                    property var adjustObj: object
                    property var enumValStr: object.enumValStr
                    //property var metaModel: object.moreMetaModel
                    //property int rowMeta: metaModel.count
                    property int rowCount: Math.floor(enumValStr.length)
                    property int currRows: rowCount // rowMeta > rowCount ? rowMeta : rowCount
                    height: navItemHeight*(currRows > 0? currRows : 1)
                    width: deviceItemWidth

                    GcsToggleButton {
                        id: adjustTxt
                        checkeState: adjustObj.working
                        anchors.left: parent.left
                        height: navItemHeight*(currRows > 0? currRows : 1)
                        width: deviceItemWidth/2
                        text: adjustObj.name
                        onDoubleClicked: {
                            adjustTxt.focus = true
                            if (lastAdjustCmd) {
                                lastAdjustCmd.working = false
                                lastAdjustCmd = null
                            }
                        }
                    }

                    ControlBackground{
                        width: deviceItemWidth/2
                        height: navItemHeight*(rowCount > 1 ? rowCount : 1)
                        visible: rowCount > 1 ? true : false
                        anchors.left: adjustTxt.right
                        Grid{
                            anchors.fill: parent
                            columns: 1
                            Repeater {
                                model: enumValStr
                                delegate: Item {
                                    height: navItemHeight
                                    width: deviceItemWidth/2
                                    GcsToggleSwitch {
                                        id: cmdOptionBtn
                                        text: modelData
                                        anchors.fill: parent
                                        checked: (lastAdjustCmd && lastAdjustCmd.selectId === index) ? true: false
                                        clickListener: () => {
                                            if (lastAdjustCmd) {
                                                lastAdjustCmd.selectId = -1
                                                lastAdjustCmd.working = false
                                            }
                                            adjustObj.selectId = index
                                            adjustObj.working = true
                                            lastAdjustCmd = adjustObj
                                            cmdOptionBtn.focus = true
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

                    ControlBackground{
                        width: deviceItemWidth/2
                        height: navItemHeight
                        anchors.left: adjustTxt.right
                        visible: (rowCount <= 1) ? true : false
                        GcsTextBox {
                            anchors.fill: parent
                            validator: RegExpValidator{regExp: adjustObj.regExpress}
                            notifyText: object.placeText
                            text: object.defValue
                            id: devtypeinput
                            onCommit: {
                                adjustObj.setRawValue(devtypeinput.text.trim());
                                sendAckOperate();
                            }
                            onActiveFocusChanged: {
                                adjustObj.setRawValue(devtypeinput.text.trim());
                                if (devtypeinput.activeFocus) {
                                    if (lastAdjustCmd) {
                                        lastAdjustCmd.selectId = -1
                                        lastAdjustCmd.working = false
                                    }
                                    adjustObj.working = true
                                    lastAdjustCmd = adjustObj
                                } else {
                                    adjustObj.working = false
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
