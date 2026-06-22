import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    id: _root
    property var rcAssociated: currVehicle?currVehicle.rcAssociated: null
    property var selectExtGroupCmds: vechicleRC.commands.extentCmdGroup
    property var selectGroupAdjust: vechicleRC.commands.cmdAdjusts
    property int colGroups: selectExtGroupCmds ? selectExtGroupCmds.columnSize : 3
    property int contentWidth: Math.ceil((_root.width - 90)/colGroups)
    property int adjustWidth: Math.ceil((_root.width - 86)/4)
    property int navItemHeight: 36
    property var selectCmdItem: null
    property var lastAdjustCmd: null
    property int ctrlWidth: 400
    property int ctrlHeight: 400

    function sendAckOperate() {
        ackButn.focus = true

        /// 判断系统角色
        if (!noticeStationRole()) {
            if (selectCmdItem) {
                selectCmdItem.working = false
            }
            if (lastAdjustCmd) {
                lastAdjustCmd.working = false
            }
        }

        if (selectCmdItem) {
            if (selectCmdItem.popup > 0) {
                simpleMsgDialog.title = qsTr("发送确认");
                simpleMsgDialog.message = qsTr("是否发送【")+ selectCmdItem.name+qsTr("】指令");
                simpleMsgDialog.iconSource= ttfIcons["warning"]
                simpleMsgDialog.onSettingClickListener = ()=>{
                    commandMgr.sendControlCommand(selectCmdItem);
                    simpleMsgDialog.close()
                    selectCmdItem = null
                }
                simpleMsgDialog.onCancelClickListener= ()=>{
                    selectCmdItem.working = false
                    simpleMsgDialog.close()
                    selectCmdItem = null
                }
                simpleMsgDialog.open()
            }
            else {
                commandMgr.sendControlCommand(selectCmdItem);
                selectCmdItem = null
            }
        }
        if (lastAdjustCmd) {
            commandMgr.sendAdjustCommand(lastAdjustCmd);
            lastAdjustCmd = null
        }
    }

    anchors.fill: parent
    Item {
        id: srcAck
        anchors.rightMargin: 10
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: navItemHeight
        height: 100
        width: 46

        GcsFilledButton {
            id: ackButn
            anchors.fill: parent
            text: qsTr("发\n送\n确\n认")
            onClicked: {
                sendAckOperate()
            }
        }
    }

    GcsDivider{
        orientation: Qt.Horizontal
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 10
        width: parent.width/3
        spacing: navItemHeight/2
    }

    GcsDivider{
        orientation: Qt.Horizontal
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 10
        width: parent.width/3
        spacing: navItemHeight/2
    }

    GcsText{
        height: navItemHeight
        anchors.right: parent.right
        anchors.rightMargin: parent.width/3
        anchors.left: parent.left
        anchors.leftMargin: parent.width/3
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: qsTr("控制《》遥调")
    }

    Grid{
        id: controlPart
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: navItemHeight
        anchors.leftMargin: 10
        columns: colGroups
        rowSpacing: 20
        spacing: 5
        Repeater {
            model: selectExtGroupCmds ? selectExtGroupCmds.commands : null
            delegate: Item {
                property var blockObj: object
                property int counts: object.counts
                property var subcommands: blockObj.subcommands
                width: contentWidth
                height: (navItemHeight + 5) * counts
                // GcsButton {
                //     id: namegroup
                //     width: contentWidth
                //     height: navItemHeight
                //     anchors.top: parent.top
                //     text: blockObj.groupName
                // }
                Column{
                    anchors.fill: parent
                    spacing: 5
                    Repeater {
                        model: subcommands
                        delegate: GcsToggleButton {
                            property var cmdObj: object
                            property string objName: cmdObj.name
                            id: cmdObjBtn
                            width: contentWidth
                            height: navItemHeight
                            text: objName
                            checkeState: cmdObj.working
                            onClicked: {
                                if (objName.length > 0) {
                                    if (selectCmdItem) {
                                        selectCmdItem.working = false
                                    }
                                    cmdObj.working = !cmdObj.working
                                    selectCmdItem = cmdObj
                                    cmdObjBtn.focus = true
                                    if (lastAdjustCmd) {
                                        lastAdjustCmd.selectId = -1
                                        lastAdjustCmd.working = false
                                        lastAdjustCmd = null
                                    }
                                }
                            }
                            Keys.onPressed:{
                                if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                                    event.accepted = true
                                    sendAckOperate();
                                }
                            }
                            Component.onCompleted: {
                                cmdObj.working = false
                            }
                        }
                    }
                }
            }
        }
    }

    Grid{
        visible: !rcAssociated ? true :false
        anchors.top: controlPart.bottom
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 10
        columnSpacing: 10
        rowSpacing: 5
        columns: 2
        Repeater {
            model: selectGroupAdjust ? selectGroupAdjust.adjustCmds : null
            delegate: Item {
                property var adjustObj: object
                property var enumValStr: object.enumValStr
                property var metaModel: object.moreMetaModel
                property int rowMeta: metaModel.count
                property int rowCount: enumValStr.length
                property int currRows: rowMeta > rowCount ? rowMeta : rowCount
                property int typeItemWidth: (rowMeta > 1) ? adjustWidth*2/5 : adjustWidth

                width: adjustWidth*2
                height: navItemHeight *(currRows > 0? currRows : 1)

                GcsToggleButton {
                    id: adjustTxt
                    width: typeItemWidth
                    height: navItemHeight*(currRows > 0? currRows : 1)
                    checkeState: adjustObj.working
                    onDoubleClicked: {
                        adjustTxt.focus = true
                        if (lastAdjustCmd) {
                            lastAdjustCmd.working = false
                            lastAdjustCmd = null
                        }
                    }
                    Component.onCompleted: {
                        if (rowMeta <= 1) {
                            if (rowCount > 1) {
                                adjustTxt.text = insertNewlineUnicode(adjustObj.name)
                            } else {
                                adjustTxt.text = adjustObj.name
                            }
                        } else {
                            var grpname = adjustObj.groupName
                            adjustTxt.text = insertNewlineUnicode(grpname)
                        }
                    }
                }

                ControlBackground{
                    width: adjustWidth*2 - typeItemWidth
                    height: navItemHeight*(rowCount > 1 ? rowCount : 1)
                    anchors.left: adjustTxt.right
                    visible: rowCount > 1 ? true : false
                    Grid{
                        anchors.fill: parent
                        columns: (rowMeta <= 1) ? 1: 2
                        Repeater {
                            model: enumValStr
                            delegate: Item {
                                height: navItemHeight
                                width: adjustWidth-typeItemWidth/2
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
                                        if (selectCmdItem) {
                                            selectCmdItem.working = false
                                            selectCmdItem = null
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
                    anchors.left: adjustTxt.right
                    width: adjustWidth*2 - typeItemWidth
                    height: navItemHeight*(rowMeta > 1 ? rowMeta : 1)
                    visible: rowMeta > 1 ? true : false
                    Column {
                        anchors.fill: parent
                        Repeater {
                            model: metaModel
                            delegate: Row{
                                property var childObject: object
                                property var enumSubStr: childObject.enumValStr
                                property int subCount: enumSubStr.length
                                property int subRows: subCount > 1 ? Math.ceil(subCount/2): 1
                                visible: childObject.visible
                                Component.onCompleted: {
                                    // 隐藏了某数据行消息
                                    if (!childObject.visible && currRows > 1 && rowMeta > 1) {
                                        currRows -= 1;
                                        rowMeta -= 1;
                                        return;
                                    }
                                }
                                ControlBackground {
                                    width: adjustWidth*3/5
                                    height: navItemHeight*subRows
                                    GcsButton {
                                        txtHorzAlign: Text.AlignHCenter
                                        text: childObject.name
                                        anchors.fill: parent
                                    }
                                }
                                ControlBackground {
                                    width: adjustWidth
                                    height: navItemHeight*subRows
                                    GcsTextBox{
                                        id: intputv
                                        anchors.fill: parent
                                        text: childObject.rawInValue
                                        visible: subCount<=1?true:false
                                        validator: RegExpValidator{regExp: childObject.regExp}
                                        onCommit: {
                                            childObject.setRawValue(intputv.text.trim());
                                            sendAckOperate();
                                        }
                                        onActiveFocusChanged: {
                                            childObject.setRawValue(intputv.text.trim());
                                            if (intputv.activeFocus) {
                                                if (selectCmdItem) {
                                                    selectCmdItem.working = false
                                                    selectCmdItem = null
                                                }
                                                adjustObj.working = true
                                                lastAdjustCmd = adjustObj
                                            }
                                        }
                                    }
                                    Grid {
                                        visible: subCount<=1?false:true
                                        anchors.fill: parent
                                        columns: 2
                                        Repeater {
                                            model: enumSubStr
                                            delegate: GcsToggleSwitch {
                                                id: cmdSwitchBtn
                                                text: modelData
                                                width: adjustWidth/2
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

                ControlBackground{
                    width: typeItemWidth
                    height: navItemHeight
                    anchors.left: adjustTxt.right
                    visible: (rowMeta <= 1) && (rowCount <= 1) ? true : false
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
                                if (selectCmdItem) {
                                    selectCmdItem.working = false
                                    selectCmdItem = null
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

    Grid{
        visible: rcAssociated ? true :false
        anchors.top: controlPart.bottom
        anchors.left: parent.left
        anchors.topMargin: 20
        anchors.leftMargin: 10
        columnSpacing: 36
        rowSpacing: 1
        columns: 2
        Repeater {
            model: rcAssociated?rcAssociated.dataModel:null
            delegate: GcsTextButton {
                property var meta: object
                id: oneElem
                height: 34
                width: adjustWidth*2
                labelValue: meta.itemName

                Component.onCompleted:  {
                    if (meta && meta.display) {
                        oneElem.textValue = meta.display
                    }
                }
                Connections {
                    target: meta
                    function onUpdateColorValue(value, color) {
                        oneElem.textValue = (color >0xf) ? qsTr("--") : value;
                        oneElem.textColor =  txtColorMap[color & 0xf]
                    }
                }
            }
        }
    }
}
