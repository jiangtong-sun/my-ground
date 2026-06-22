import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import GcsLineOperate 1.0
import GcsLineStage 1.0
import GcsTaskLine 1.0
import GcsQuick 1.0

Item {
    id: root
    property bool selectFline: false
    property int stageTableView: 400
    property int showVehMission: -1
    property int selectVehicle: 0
    property int vehItemHeight: 32
    property int vehItemWidth: 50
    property int itemHeight: 32
    property var flineTable: flineModel.tableModel
    property var moreFStage: [1, 1, 1, 1, 1, 1, 0, 0, 0, 0]
    property int vehPartWidth: optPanelWidth
    property int lineLabelWidth: optPanelWidth/4+30
    property int lineValueWidth: (optPanelWidth - lineLabelWidth - 20)/7
    property string selectTaskName: qsTr("")

    function resetWaypointEdit() {
        waypointEditor.resetWaypointShow()
    }

    function countMStageRow() {
        let countRow = 0;
        for (let a = 0; a < 10; a++) {
            countRow += moreFStage[a]
        }

        stageInfo.height = countRow*vehItemHeight
    }

    opacity: 0.8
    width: optPanelWidth

    Popup{
        id: taskView
        padding: 1
        modal: false
        height: 200
        width: optPanelWidth
        parent: Overlay.overlay
        GcsTreeViewOpt {
            id: allfline
            anchors.fill: parent
            tableModel: flineTable
            showStageListener: (model, select) =>{
                resetTerrainCurve()
                if (selVehMission) {
                    selectTaskName = model.name
                    selVehMission.mergeFlightLine(model);
                    taskView.close()
                }
            }
        }
    }

    Item {
        id: vehInfor
        width: vehPartWidth
        height: 5 * vehItemHeight
        anchors.left: parent.left
        Component {
            id: vehLineOperate
            ControlBackground {
                property var vehObj: object

                id: vehTaskComp
                height: vehItemHeight
                width: vehPartWidth
                color: (selVehMission === vehObj)?GcsTheme.primaryColor:GcsTheme.backgroundColor
                GcsText{
                    anchors.left: parent.left
                    height: vehItemHeight
                    width: vehPartWidth - vehItemWidth*3
                    text: vehObj.vehName + " / " + selectTaskName
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton|Qt.LeftButton

                        GcsSimpleMenu {
                            id: contextMenu
                            implicitHeight: 130
                            GcsMenuItem {
                                text: qsTr("文件加载")
                                selecListener: ()=> {
                                    loadDialog.open()
                                    contextMenu.close()
                                }
                            }
                            GcsMenuItem {
                                text: qsTr("航线另存")
                                selecListener: ()=> {
                                    saveDialog.open()
                                    contextMenu.close()
                                }
                            }
                            GcsMenuItem {
                                text: qsTr("航线上传")
                                selecListener: ()=> {
                                    selVehMission.uploadFline()
                                    contextMenu.close()
                                }
                            }
                            GcsMenuItem {
                                text: qsTr("航线下载")
                                selecListener: ()=> {
                                    updateWaypoint = false
                                    selVehMission.downloadFlightLine(0)
                                    contextMenu.close()
                                }
                            }
                        }

                        onDoubleClicked: {
                            if (selectCurrFLine) {
                                // 删除航线的任务数据
                                selectCurrFLine.removeStageResource()
                            }

                            selVehMission = null
                            selectCurrFLine = null
                            resetTerrainCurve()///

                            commandMgr.currVehicle(0)
                        }

                        onClicked: {
                            if (mouse.button == Qt.RightButton) {
                                contextMenu.popup()
                            }
                            else {
                                resetTerrainCurve()
                                selVehMission = vehObj
                                selectCurrFLine = selVehMission.currFline
                                selectVehicle = selVehMission.vehicleIdx
                                commandMgr.currVehicle(selVehMission.vehicleIdx)
                            }
                        }
                    }
                }

                GcsToggleButton {
                    id: uploadBtn
                    width: vehItemWidth-1
                    height: vehItemHeight
                    visible: (selVehMission === vehObj) ? true : false
                    anchors.right: parent.right
                    anchors.rightMargin: 2*vehItemWidth
                    text: qsTr("上传")
                    onClicked: {
                        selVehMission.uploadFline()
                    }
                }

                GcsToggleButton {
                    id: downloadBtn
                    width: vehItemWidth-1
                    height: vehItemHeight
                    visible: (selVehMission === vehObj) ? true : false
                    anchors.rightMargin: vehItemWidth
                    anchors.right: parent.right
                    text: qsTr("下载")
                    onClicked: {
                        updateWaypoint = false
                        selVehMission.downloadFlightLine(0)
                    }
                }

                GcsToggleButton {
                    id: loadBtn
                    width: vehItemWidth-1
                    height: vehItemHeight
                    visible: (selVehMission === vehObj) ? true : false
                    anchors.right: parent.right
                    text: qsTr("任务")
                    onClicked: {
                        const globalPos = vehTaskComp.mapToItem(null, 0, 0)
                        taskView.y = globalPos.y + vehItemHeight
                        taskView.x = globalPos.x
                        taskView.open()
                    }
                }
            }
        }

        ListView{
            clip: true
            focus: true
            anchors.fill: parent
            ScrollBar.vertical: GcsScrollBar { }
            ScrollIndicator.vertical: GcsScrollIndicator {}
            boundsBehavior: Flickable.StopAtBounds
            model: vehMissions
            delegate: vehLineOperate
        }
    }

    GcsDivider{
        id: vehDivider
        anchors.top: vehInfor.bottom
        width: optPanelWidth
        contrast: true
        spacing: 0
    }

    Item {
        id: stageInfo
        anchors.right: parent.right
        anchors.top: vehDivider.bottom
        height: 6 * vehItemHeight
        width: optPanelWidth
        Column{
            anchors.fill: parent
            Repeater {
                model: selectCurrFLine?selectCurrFLine.allStages:null

                delegate: ControlBackground {
                    property var flineObj: object
                    property bool mouseState: false
                    property int stageCount: flineObj?flineObj.countStage:0
                    property var fstagesMode: flineObj?flineObj.stagesMode:null
                    property int rowCount: stageCount > 0 ? Math.ceil(stageCount/7) : 1

                    Connections {
                        target: flineObj
                        function onCountStagesChanged() {
                            moreFStage[index] = rowCount
                            countMStageRow()
                        }
                    }

                    Component.onCompleted: {
                        moreFStage[index] = rowCount
                        countMStageRow()
                    }

                    height: vehItemHeight*rowCount
                    width: optPanelWidth

                    Item {
                        id: stageBtnOpt
                        width: lineLabelWidth
                        height: vehItemHeight*rowCount

                        GcsIconButton{
                            id:btn_maximize
                            anchors.left: parent.left
                            height: vehItemHeight * rowCount
                            width: 30
                            verticalPadding: 0
                            horizontalPadding: 0
                            iconSource: flineObj && flineObj.visible ? ttfIcons["browse"] : ttfIcons["hide"]
                            onClicked: {
                                flineObj.visible = !flineObj.visible
                            }
                        }

                        GcsButton {
                            anchors.right: parent.right
                            width: lineLabelWidth - 30
                            height: vehItemHeight * rowCount
                            text: flineObj?flineObj.name: qsTr("")
                            onClicked: {
                                if ((flineObj.code === GcsTaskLine.FTaskLineA) ||
                                        (flineObj.code === GcsTaskLine.FTaskLineB) ||
                                        (flineObj.code === GcsTaskLine.FTaskErgency) ) {
                                    toastShow(qsTr("综合航线/迫降航线/驶入驶出航线不可编辑"), 2000)
                                    return;
                                }
                                flineDetailStage = flineObj
                                fileDialog.open()
                            }
                        }
                    }

                    MouseArea{
                        hoverEnabled: true
                        height: vehItemHeight*rowCount
                        anchors.right: parent.right
                        anchors.left: stageBtnOpt.right
                        anchors.leftMargin: 2
                        anchors.rightMargin: 2

                        Item {
                            id: ftaskDetail
                            anchors.fill: parent
                            Grid {
                                columns: 7
                                columnSpacing: 2
                                rowSpacing: 2
                                Repeater{
                                    model: fstagesMode
                                    delegate: GcsToggleButton {
                                        property var stageObj: object
                                        property int currCode: stageObj ? stageObj.stage: 0
                                        property bool editWaypoint: stageObj ? stageObj.canRMWaypoint : false
                                        textColor: GcsTheme.dark?GcsTheme.White:GcsTheme.Black
                                        borderColor: stageObj && stageObj.newElem ? GcsTheme.primaryColor :GcsTheme.itemGrayColor
                                        normalColor: flineStage === stageObj? GcsTheme.primaryColor: GcsTheme.itemNormalColor
                                        text: stageObj ? stageObj.stageAbbr + stageObj.uuid : qsTr("")
                                        height: vehItemHeight - 4
                                        width: lineValueWidth
                                        btnRadius: 5
                                        onClicked: {
                                            resetWaypointEdit() // reset
                                            resetTerrainCurve() // reset
                                            flineObj.recalWaypointPath()
                                            currFlineStage = flineObj
                                            flineStage = stageObj
                                            fStageCode = currCode
                                            stageCode = currCode
                                            updateWaypoint = editWaypoint
                                            editorWaypoint = editWaypoint
                                        }
                                        onDoubleClicked: {
                                            resetWaypointEdit() // reset
                                            resetTerrainCurve() // reset
                                            updateWaypoint = false
                                            flineStage = null
                                            fStageCode = 0
                                            stageCode = 0
                                        }
                                    }
                                }
                            }
                        }

                        Row {
                            visible: flineObj && (flineObj.code&0x1000) ? true:false
                            anchors.right: parent.right
                            anchors.rightMargin: 2
                            width: lineValueWidth*2
                            height: vehItemHeight
                            spacing: 2
                            GcsToggleButton {
                                id: downBtn
                                btnRadius: 5
                                width: lineValueWidth
                                height: vehItemHeight
                                visible: mouseState
                                text: qsTr("下载")
                                onClicked: {
                                    if (flineStage) {
                                        if (flineStage.isLock) {
                                            toastShow(qsTr("LineStageIsLock"), 2000)
                                        }
                                        else {
                                            selVehMission.downloadFlightLine(flineStage.uuid)
                                        }
                                    }
                                }
                            }
                            GcsToggleButton {
                                id: uploadBtn
                                btnRadius: 5
                                width: lineValueWidth
                                height: vehItemHeight
                                visible: mouseState && fstagesMode.count>0
                                text: qsTr("上传")
                                onClicked: {
                                    if (flineStage) {
                                        if (flineStage.isLock) {
                                            toastShow(qsTr("LineStageIsLock"), 2000)
                                        }
                                        else {
                                            selVehMission.uploadStage(flineStage)
                                        }
                                    }
                                }
                            }
                        }

                        onDoubleClicked: {
                            if (flineObj.code & 0x1000) {
                                var mx = flineObj.newFlineStage()
                                if (mx === GcsLineOperate.LineStageIsFull) {
                                    toastShow(qsTr("LineStageIsFull"), 2000)
                                } else if (mx === GcsLineOperate.UnableUpdateStage) {
                                    toastShow(qsTr("UnableUpdateStage"), 2000)
                                } else {
                                    resetWaypointEdit()// reset editor
                                    flineStage = flineObj.selectStage
                                    fStageCode = flineStage.stage
                                    stageCode = flineStage.stage
                                    currFlineStage = flineObj
                                    editorWaypoint = true
                                    updateWaypoint = true
                                }
                            } else {
                                toastShow(qsTr("综合航线/驶入驶出/迫降航线不可编辑"), 2000)
                            }
                        }

                        onClicked: {
                            resetTerrainCurve()
                            flineObj.recalWaypointPath()
                            currFlineStage = flineObj

                        }

                        onEntered: {
                            mouseState = true
                        }

                        onExited: {
                            mouseState = false
                        }
                    }
                }
            }
        }
    }

    GcsDivider{
        id: stageDivider
        anchors.top: stageInfo.bottom
        width: optPanelWidth
        contrast: true
        spacing: 0
    }

    FmpMissionEditor{
        id: waypointEditor
        visible: updateWaypoint
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: stageDivider.bottom
        anchors.bottom: parent.bottom
        width: optPanelWidth
    }

    FmpStageWaypoints {
        visible: !updateWaypoint
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: stageDivider.bottom
        anchors.bottom: parent.bottom
        width: optPanelWidth
    }

    // FmpDetailStage {
    //     visible: !updateWaypoint
    //     anchors.left: parent.left
    //     anchors.right: parent.right
    //     anchors.top: stageDivider.bottom
    //     anchors.bottom: parent.bottom
    //     width: optPanelWidth
    // }
}
