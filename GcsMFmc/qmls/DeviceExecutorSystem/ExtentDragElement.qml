import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

GcsDragElement {
    property var lastAdjustCmd: null
    property var deviceOperate: vechicleRC.commands.cmdAdjusts

    id: moveItem

    GcsFilledButton {
        id: ackButn
        width: 50
        height: moveItem.height
        anchors.right: parent.right
        anchors.rightMargin: 2
        text: qsTr("发\n送\n确\n认")
        onClicked: {
            ackButn.focus = true
            if (lastAdjustCmd) {
                commandMgr.sendAdjustCommand(lastAdjustCmd);
                lastAdjustCmd = null
            }
        }
    }

    Grid {
        anchors.fill: parent
        anchors.right: ackButn.left
        anchors.topMargin: 5
        anchors.leftMargin: 2
        columns: columnFirst
        spacing: 4
        Repeater {
            model: deviceOperate ? deviceOperate.adjustCmds : null
            delegate: ControlBackground {
                property var adjustObj: object
                property var metaModel: object.moreMetaModel
                property int rowCount: metaModel.count>0?metaModel.count:1
                property int itemTypeWidth: rowCount > 1? (moveItem.width-60)/9: (moveItem.width-60)/6
                width: itemTypeWidth*(rowCount>1?3:2)
                height: navItemHeight*rowCount
                GcsToggleButton {
                    id: adjustTxt
                    width: itemTypeWidth
                    height: navItemHeight*rowCount
                    checkeState: adjustObj.working
                    text: rowCount > 1 ? adjustObj.groupName : adjustObj.name
                }
                ControlBackground{
                    width: itemTypeWidth*2
                    height: navItemHeight*rowCount
                    anchors.left: adjustTxt.right
                    visible: rowCount > 1 ? true : false
                    Column {
                        anchors.fill: parent
                        Repeater {
                            model: metaModel
                            delegate: Row {
                                property var childObject: object
                                ControlBackground {
                                    width: itemTypeWidth+20
                                    height: navItemHeight
                                    GcsButton {
                                        horizontalPadding: 8
                                        txtHorzAlign: Text.AlignLeft
                                        text: childObject.name
                                        anchors.fill: parent
                                    }
                                }
                                ControlBackground {
                                    width: itemTypeWidth-20
                                    height: navItemHeight
                                    GcsTextBox{
                                        id: intputv
                                        notifyText: childObject.place
                                        validator: RegExpValidator{regExp: childObject.regExp}
                                        anchors.fill: parent
                                        onTextEdited: {
                                            childObject.setRawValue(intputv.text.trim());
                                        }
                                        onCommit: {
                                            childObject.setRawValue(intputv.text.trim());
                                        }
                                        onActiveFocusChanged: {
                                            childObject.setRawValue(intputv.text.trim());
                                            if (intputv.activeFocus) {
                                                adjustObj.working = true
                                                lastAdjustCmd = adjustObj
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                Row{
                    width: itemTypeWidth
                    height: navItemHeight*rowCount
                    anchors.left: adjustTxt.right
                    visible: (rowCount <= 1) ? true : false

                    GcsTextBox {
                        width: itemTypeWidth*2/3
                        height: navItemHeight*rowCount
                        validator: RegExpValidator{regExp: adjustObj.regExpress}
                        notifyText: object.placeText
                        text: object.defValue
                        id: devtypeinput

                        onTextEdited: {
                            adjustObj.setRawValue(devtypeinput.text.trim());
                        }
                        onCommit: {
                            adjustObj.setRawValue(devtypeinput.text.trim());
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
                    GcsText{
                        width: itemTypeWidth/3
                        height: navItemHeight*rowCount
                        text: object.units
                    }
                }
            }
        }
    }
}
