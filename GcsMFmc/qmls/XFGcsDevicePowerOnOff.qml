import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    id: root
    anchors.fill: parent
    property var lastAdjustCmd: null
    property var selectGeneratorItem: null
    property var selectCmdItem: null
    property var selectWarnNode: null

    property var batteryOnOff: vechicleRC.commands.batteryOnOff
    property var devPowerOnOff: vechicleRC.commands.devicePowerOnOff
    property var generatorOnOff: vechicleRC.commands.generatorControl
    property var warnConfGroup: currVehicle? currVehicle.warnConfGroup: nullptr
    property var allWarnMode: warnConfGroup? warnConfGroup.getAllMode: nullptr
    property int countCols: Math.ceil(allWarnMode.count/10)
    property int contentWidth: (common.width-20)/countCols
    property int navItemHeight: (common.height-10)/10
    property int itemHeightExt: (common.height-10)/8
    property var firstDevPower: devPowerOnOff.firstItem
    property var secondDevPower: devPowerOnOff.secondItem
    property int deviceElemCnt: firstDevPower.enumValStr.length
    property int batteryWidth: (root.width - 200) * 2/5
    property int batteryItemW: batteryWidth/4

    function setModePart(first, second, eachHeight, groupStr) {}
    function sendDeviceOnOperate() {
        if (selectWarnNode && selectWarnNode.popupWin > 0 && selectCmdItem){
            simpleMsgDialog.title = qsTr("发送确认");
            simpleMsgDialog.message = qsTr("是否发送【")+ selectWarnNode.warnItemName+qsTr("】指令");
            simpleMsgDialog.iconSource= ttfIcons["warning"]
            simpleMsgDialog.onSettingClickListener = ()=>{
                commandMgr.sendSimpleCommand(firstDevPower);
                simpleMsgDialog.close()
                selectCmdItem = null
                selectWarnNode = null
            }
            simpleMsgDialog.onCancelClickListener= ()=>{
                selectCmdItem.working = false
                simpleMsgDialog.close()
                selectCmdItem = null
                selectWarnNode = null
            }
            simpleMsgDialog.open()
        }else {
            if (firstDevPower && selectCmdItem) {
                commandMgr.sendSimpleCommand(firstDevPower);
                selectCmdItem = null
                selectWarnNode = null
            }
        }
    }
    function sendDeviceOffOperate() {
        if (selectWarnNode && selectWarnNode.popupWin > 0 && selectCmdItem){
            simpleMsgDialog.title = qsTr("发送确认");
            simpleMsgDialog.message = qsTr("是否发送【")+ selectWarnNode.warnItemName+qsTr("】指令");
            simpleMsgDialog.iconSource= ttfIcons["warning"]
            simpleMsgDialog.onSettingClickListener = ()=>{
                commandMgr.sendSimpleCommand(secondDevPower);
                simpleMsgDialog.close()
                selectCmdItem = null
                selectWarnNode = null
            }
            simpleMsgDialog.onCancelClickListener= ()=>{
                selectCmdItem.working = false
                simpleMsgDialog.close()
                selectCmdItem = null
                selectWarnNode = null
            }
            simpleMsgDialog.open()
        } else {
            if (secondDevPower && selectCmdItem) {
                commandMgr.sendSimpleCommand(secondDevPower);
                selectCmdItem = null
                selectWarnNode = null
            }
        }
    }
    function sendGeneratorOperate() {
        if(selectGeneratorItem) {
            commandMgr.sendControlCommand(selectGeneratorItem);
            selectGeneratorItem = null
        }
        if(lastAdjustCmd) {
            commandMgr.sendAdjustCommand(lastAdjustCmd);
            lastAdjustCmd = null
        }
    }
    Component.onCompleted: {
        console.log("onCompleted XFGcsDevicePowerOnOff")
    }

    Component.onDestruction: {
        console.log("onDestruction XFGcsDevicePowerOnOff")
    }
    Item {
        id: srcAck
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.rightMargin: 20
        height: parent.height
        width: 40
        Column{
            anchors.fill: parent
            spacing: 20
            Item {
                height: 100
                width: 40
                GcsFilledButton {
                    anchors.fill: parent
                    text: qsTr("发\n送\n确\n认")
                    onClicked: {
                        sendGeneratorOperate();
                    }
                }
            }
        }
    }

    Item {
        id: storage
        anchors.right: srcAck.left
        anchors.rightMargin: 10
        height: parent.height
        width: batteryWidth

        Row{
            id: battery
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.top: parent.top
            anchors.topMargin: 10
            width: batteryWidth

            Repeater{
                model: batteryOnOff?batteryOnOff.adjustCmds:null
                delegate:  Item {
                    property var adjustObj: object
                    property int currRows: metaModel.count
                    property var metaModel: object.moreMetaModel
                    property int typeItemWidth: currRows > 1? batteryItemW/2 : batteryItemW
                    property int typeCtxWidth: batteryItemW*2 - 40 - typeItemWidth
                    height: navItemHeight *(currRows > 0? currRows : 1) + 40
                    width: batteryItemW*2
                    GroupRectangle {
                        anchors.fill: parent
                        penColor: GcsTheme.dividerColor
                        text: adjustObj.groupName
                    }
                    Column {
                        anchors.fill: parent
                        anchors.margins: 20
                        Repeater {
                            model: metaModel
                            delegate: Row{
                                property var childObject: object
                                property var enumSubStr: childObject.enumValStr
                                property int subRows: Math.ceil(enumSubStr.length/2)
                                Component.onCompleted: {
                                    currRows += (subRows - 1)
                                }
                                ControlBackground {
                                    width: typeItemWidth
                                    height: navItemHeight*subRows
                                    GcsButton {
                                        txtHorzAlign: Text.AlignHCenter
                                        text: childObject.name
                                        anchors.fill: parent
                                    }
                                }
                                ControlBackground {
                                    width: typeCtxWidth
                                    height: navItemHeight*subRows
                                    Grid {
                                        anchors.fill: parent
                                        columns: 2
                                        Repeater {
                                            model: enumSubStr
                                            delegate: GcsToggleSwitch {
                                                id: cmdSwitchBtn
                                                text: modelData
                                                width: typeCtxWidth/2
                                                height: navItemHeight
                                                checked: {
                                                    if((lastAdjustCmd === adjustObj) &&
                                                        (childObject.enumSelect === index)) {
                                                        return true
                                                    }
                                                    return false
                                                }
                                                clickListener: () => {
                                                    if (selectCmdItem) {
                                                        selectCmdItem.working = false
                                                        selectCmdItem = null
                                                    }
                                                    if (selectGeneratorItem) {
                                                        selectGeneratorItem.working = false
                                                        selectGeneratorItem = null
                                                    }
                                                    adjustObj.working = true
                                                    lastAdjustCmd = adjustObj
                                                    childObject.enumSelect = index
                                                    cmdSwitchBtn.focus = true
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

        Item {
            id: generator
            anchors.bottom: parent.bottom
            anchors.top: battery.bottom
            anchors.topMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: 10
            width: batteryWidth


            GroupRectangle {
                anchors.fill: parent
                penColor: GcsTheme.dividerColor
                text: qsTr("发电机投网")
            }

            Row{
                id: controlPart
                anchors.fill: parent
                anchors.margins: 10
                anchors.topMargin: 25
                anchors.leftMargin: generator.width/16
                spacing: generator.width/16
                Repeater {
                    model: generatorOnOff ? generatorOnOff.commands : null
                    delegate: Item {
                        property var blockObj: object
                        property int counts: object.counts
                        property var subcommands: blockObj.subcommands
                        width: generator.width/4
                        height: parent.height
                        Column{
                            anchors.fill: parent
                            spacing: 5
                            Repeater {
                                model: subcommands
                                delegate: GcsToggleButton {
                                    property var cmdObj: object
                                    id: cmdObjBtn
                                    width: generator.width/4
                                    height: itemHeightExt
                                    text: cmdObj.name
                                    checkeState: cmdObj.working
                                    onClicked: {
                                        if (selectCmdItem) {
                                            selectCmdItem.working = false
                                            selectCmdItem = null
                                        }
                                        if (lastAdjustCmd) {
                                            lastAdjustCmd.working = false
                                            lastAdjustCmd = null
                                        }
                                        if (selectGeneratorItem) {
                                            selectGeneratorItem.working = false
                                        }
                                        cmdObj.working = !cmdObj.working
                                        selectGeneratorItem = cmdObj
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

    Item {
        id: deviceonoff
        anchors.right: storage.left
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.rightMargin: 10
        height: parent.height
        width: 40
        Column{
            anchors.fill: parent
            spacing: 20
            Item {
                height: 100
                width: 40
                GcsFilledButton {
                    anchors.fill: parent
                    text: qsTr("接\n通\n确\n认")
                    onClicked: {
                        sendDeviceOnOperate();
                    }
                }
            }
            Item {
                height: 100
                width: 40
                GcsFilledButton {
                    anchors.fill: parent
                    text: qsTr("断\n开\n确\n认")
                    onClicked: {
                        sendDeviceOffOperate();
                    }
                }
            }
        }
    }

    Item {
        id: common
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.right: deviceonoff.left
        height: parent.height
        GroupRectangle {
            bottomMargin: 20
            anchors.fill: parent
            penColor: GcsTheme.dividerColor
            text: firstDevPower.groupName
        }
        Grid {
            anchors.fill: parent
            anchors.topMargin: 20
            anchors.leftMargin: 10
            columns: countCols
            Repeater {
                model: allWarnMode
                delegate: ControlBackground {
                    property int elemIndex: index
                    property var warnNode: object
                    property var displayMeta: warnNode.display

                    Connections{
                        target: displayMeta
                        function onDisplayChanged(avalue, acolor){
                            var state = parseInt(avalue);
                            devPowerBtn.checked = state? true:false
                            devPowerBtn.normalColor = txtColorMap[acolor & 0xf]
                        }
                    }

                    // 文本颜色
                    // Connections {
                    //     target: warnNode
                    //     function onWarnColorChanged(acolor) {
                    //         devPowerBtn.textColor = rmGreenColorMap[acolor & 0xf]
                    //     }
                    // }

                    id: warnBackgrnd
                    height: navItemHeight
                    width: contentWidth
                    borderColor: {
                        if(firstDevPower.selectId === elemIndex &&
                                secondDevPower.selectId === elemIndex) {
                            return GcsTheme.primaryColor
                        }
                        return GcsTheme.windowBorderColor
                    }

                    GcsToggleSwitch {
                        id: devPowerBtn
                        anchors.fill: parent
                        visible: warnNode.sequence>0
                        text: warnNode.warnItemName

                        Component.onCompleted: {
                            // var colr = rmGreenColorMap[warnNode.warnColor & 0xff]
                            // devPowerBtn.textColor = colr

                            if (displayMeta) {
                                var rawVal = displayMeta.rawInValue
                                var color = displayMeta.acolor;
                                if (rawVal && rawVal.length > 0 && (color < 0xf)) {
                                    devPowerBtn.normalColor = txtColorMap[color & 0xf]
                                } else {
                                    devPowerBtn.normalColor = txtColorMap[4]
                                }
                            }
                        }

                        clickListener: () => {
                            if (warnNode.sequence <= deviceElemCnt) {
                                firstDevPower.selectId = elemIndex
                                secondDevPower.selectId = elemIndex
                                secondDevPower.mapSelect = warnNode.sequence
                                firstDevPower.mapSelect = warnNode.sequence
                                selectCmdItem = firstDevPower
                                selectWarnNode = warnNode
                            }
                        }
                    }
                }
            }
        }
    }
}
