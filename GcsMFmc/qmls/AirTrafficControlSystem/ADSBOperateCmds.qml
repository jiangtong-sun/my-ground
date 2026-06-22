import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item{
    id: control
    property var lastAdjustCmd
    property var vhfAdjustCmd
    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property int eachBlockW2: 200
    property int eachBlockH: (ctrlHeight - navItemHeight)
    property var vhfWorkFreqSource: vechicleRC.commands.vhfWorkFreqSource
    property var atcDeviceSource: vechicleRC.commands.atcDeviceSource
    property var vhfWorkFreqItem: vhfWorkFreqSource.firstItem

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
                sendBtn.focus = true
                if (lastAdjustCmd) {
                    commandMgr.sendAdjustATCCommand(lastAdjustCmd, 0);
                    lastAdjustCmd = null
                }
                if (vhfAdjustCmd) {
                    commandMgr.sendAdjustCommand(vhfAdjustCmd)
                    vhfAdjustCmd = null
                }
            }
        }
    }

    function splitChineseString(original) {
        var mid = Math.floor(original.length / 2)
        return original.substring(0, mid) + "\n" + original.substring(mid)
    }

    function countChineseCharacters(str) {
        let count = 0
        for (var ix = 0; ix < str.length; ix++) {
            let code = str.charCodeAt(ix);
            if ((code >= 0) && (code <= 255)) {
                count++;
            }
        }
        return (str.length >= 6 && count !== str.length) ? 1 : 0;
    }

    GridLayout {
        anchors.top: parent.top
        anchors.right: srcAck.left
        anchors.rightMargin: 10
        anchors.topMargin: 10
        width: eachBlockW2 * 7 + 30
        rowSpacing: 5
        columns: 7

        Item {
            Layout.preferredWidth: eachBlockW2
            Layout.preferredHeight: eachBlockH

            GcsToggleButton {
                height: navItemHeight
                width: parent.width
                anchors.top: parent.top
                text: vhfWorkFreqSource.groupName
            }
            ControlBackground {
                id: vehParts
                anchors.fill: parent
                anchors.topMargin: navItemHeight
                height: navItemHeight
                width: eachBlockW2
                GcsButton {
                    anchors.left: parent.left
                    height: navItemHeight
                    width: eachBlockW2/2
                    text: vhfWorkFreqItem.name
                }
                GcsTextBox {
                    id: inputVhfParam
                    anchors.right: parent.right
                    height: navItemHeight
                    width: eachBlockW2/2
                    //validator: RegExpValidator{regExp: vhfWorkFreqItem.regExp}
                    onCommit: {
                        vhfWorkFreqItem.setRawValue(inputVhfParam.text.trim());
                        vhfAdjustCmd = vhfWorkFreqItem
                        lastAdjustCmd = null
                    }
                    onActiveFocusChanged: {
                        if (inputVhfParam.text.length > 0) {
                            vhfWorkFreqItem.setRawValue(inputVhfParam.text.trim());
                        }
                        vhfAdjustCmd = vhfWorkFreqItem
                        lastAdjustCmd = null
                    }
                }
            }

            Column{
                anchors.fill: parent
                anchors.topMargin: navItemHeight*3
                height: navItemHeight * 4
                GcsToggleButton {
                    height: navItemHeight
                    width: parent.width
                    text: qsTr("ATC警告")
                }
                GcsButton {
                    height: navItemHeight
                    width: parent.width
                    text: qsTr("7500")
                    // onClicked: {
                    //     if (lastAdjustCmd && lastAdjustCmd !== atcStateCtrlSource){
                    //         lastAdjustCmd.firstItem.selectId = 0
                    //     }
                    //     atcParamItem.selectId = 1
                    //     commandMgr.sendAdjustATCCommand(atcParamItem, 7500);
                    // }
                }
                GcsButton {
                    height: navItemHeight
                    width: parent.width
                    text: qsTr("7600")
                    // onClicked: {
                    //     atcParamItem.selectId = 2
                    //     if (lastAdjustCmd && lastAdjustCmd !== atcParamsSource){
                    //         lastAdjustCmd.firstItem.selectId = 0
                    //     }
                    //     commandMgr.sendAdjustATCCommand(atcParamItem, 7600);
                    // }
                }
                GcsButton {
                    height: navItemHeight
                    width: parent.width
                    text: qsTr("7700")
                    // onClicked: {
                    //     atcParamItem.selectId = 3
                    //     if (lastAdjustCmd && lastAdjustCmd !== atcParamsSource){
                    //         lastAdjustCmd.firstItem.selectId = 0
                    //     }
                    //     commandMgr.sendAdjustATCCommand(atcParamItem, 7700);
                    // }
                }
            }
        }

        Repeater {
            model: atcDeviceSource?atcDeviceSource.adjustCmds: null
            delegate: ControlBackground {
                property var atcObject: object
                Layout.preferredWidth: eachBlockW2
                Layout.preferredHeight: eachBlockH

                GcsToggleButton {
                    id: hyControl
                    height: navItemHeight
                    width: eachBlockW2
                    anchors.top: parent.top
                    text: atcObject.name
                    checkeState: atcObject.working
                    onClicked: {
                        if(lastAdjustCmd ){
                            lastAdjustCmd.working = false
                        }
                        atcObject.working = true
                        lastAdjustCmd = atcObject
                    }
                }

                Column{
                    anchors.fill: parent
                    anchors.topMargin: navItemHeight
                    spacing: 1
                    Repeater {
                        model: atcObject ? atcObject.moreMetaModel: null
                        delegate: Item {
                            property int metaIndex: index
                            property var detailObject: object
                            property var enumValStr: detailObject.enumValStr
                            property var elemNameVal: detailObject.name
                            property var elemNameHigh: countChineseCharacters(elemNameVal)
                            property var elemName: elemNameHigh > 0 ? splitChineseString(elemNameVal) :elemNameVal
                            property int rowCount: Math.ceil(enumValStr.length/2)
                            property int elemWidth: (enumValStr.length === 1) ? (eachBlockW2/2+20) : (eachBlockW2/4+10)
                            height: navItemHeight * (rowCount > 0 ? rowCount + elemNameHigh : 1 + elemNameHigh)
                            width: eachBlockW2

                            GcsToggleButton {
                                id: cmdOptType
                                text: elemName
                                width: eachBlockW2/2-20
                                height: parent.height
                            }

                            ControlBackground {
                                width: eachBlockW2/2+20
                                height: navItemHeight
                                anchors.left: cmdOptType.right
                                visible: (rowCount <= 0) ? true : false
                                GcsTextBox {
                                    anchors.fill: parent
                                    validator: RegExpValidator{regExp: detailObject.regExp}
                                    // notifyText: detailObject.place
                                    // //text: detailObject.defValue
                                    id: devtypeinput
                                    onCommit: {
                                        detailObject.checked = true
                                        detailObject.setRawValue(devtypeinput.text.trim());
                                    }
                                    onActiveFocusChanged: {
                                        detailObject.setRawValue(devtypeinput.text.trim());
                                    }
                                }
                            }

                            Grid {
                                columns: 2
                                width: eachBlockW2/2+20
                                height: parent.height
                                anchors.left: cmdOptType.right
                                visible: (rowCount > 0) ? true : false
                                Repeater {
                                    model: enumValStr
                                    delegate: GcsToggleButton {
                                        height: navItemHeight
                                        width: elemWidth
                                        text: modelData
                                        checkeState: {
                                            if((lastAdjustCmd === atcObject) && (detailObject.enumSelect === index)){
                                                return true;
                                            }
                                            return false
                                        }
                                        onClicked: {
                                            detailObject.checked = true
                                            detailObject.enumSelect = index
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
