import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    id: _adjust
    property int itemHeight: 36
    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property int contentWidth: (_adjust.width - 100)/2
    property int inputWidth: contentWidth / 5
    property var adjustTypes: vechicleRC.commands.adjustTypes
    property var iValidator: RegularExpressionValidator{regularExpression: /^[0-9.]{5}$/}
    property var lastAdjustCmd: null

    width: ctrlWidth
    height: ctrlHeight

    Item {
        id: srcAck
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 30
        anchors.rightMargin: 20
        height: 100
        width: 40

        GcsFilledButton {
            id: ackButn
            anchors.fill: parent
            text: qsTr("发\n送\n确\n认")
            onClicked: {
                ackButn.focus = true
                commandMgr.sendAdjustCommand(lastAdjustCmd);
            }
        }
    }

    GcsDivider{
        orientation: Qt.Horizontal
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 10
        width: parent.width/3
        spacing: 20
    }

    GcsDivider{
        orientation: Qt.Horizontal
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 10
        width: parent.width/3
        spacing: 20
    }

    GcsText{
        height: itemHeight
        anchors.right: parent.right
        anchors.rightMargin: parent.width/3
        anchors.left: parent.left
        anchors.leftMargin: parent.width/3
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: qsTr("控制 & 遥调")
    }

    Item {
        id: devadjustpanel
        anchors.fill: parent
        anchors.topMargin: itemHeight
        anchors.leftMargin: 10
        anchors.right: srcAck.left
        anchors.rightMargin: 10

        Grid{
            anchors.fill: parent
            spacing: 10
            columns: 2
            Repeater {
                model: adjustTypes?adjustTypes.adjustCmds:null
                Item{
                    property var adjustObj: object
                    width: contentWidth
                    height: itemHeight
                    ControlBackground{
                        id: inputCtrl
                        width: inputWidth*2
                        height: itemHeight
                        GcsTextBox {
                            anchors.fill: parent
                            validator: iValidator
                            notifyText: object.placeText
                            text: object.defValue
                            id: devtypeinput
                            onCommit: {
                                adjustObj.setRawValue(devtypeinput.text.trim());
                            }
                            onActiveFocusChanged: {
                                adjustObj.setRawValue(devtypeinput.text.trim());
                                if (devtypeinput.activeFocus) {
                                    adjustObj.working = true
                                    lastAdjustCmd = adjustObj
                                } else {
                                    adjustObj.working = false
                                }
                            }
                        }
                    }
                    GcsToggleButton {
                        width: inputWidth*3
                        height: itemHeight
                        checkeState: adjustObj.working
                        anchors.left: inputCtrl.right
                        text: adjustObj.name
                        onDoubleClicked: {
                            devtypeinput.focus = false
                            adjustObj.working = false
                            lastAdjustCmd = null
                        }
                    }
                }
            }
        }
    }
}
