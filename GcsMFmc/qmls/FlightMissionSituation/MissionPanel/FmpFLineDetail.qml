import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import GcsLineOperate 1.0
import GcsLineStage 1.0
import GcsTaskLine 1.0
import GcsQuick 1.0

Item {
    property var moreStage: [1, 1, 1, 1, 1, 0, 0, 0, 0, 0]
    property bool takeoffPoint: false
    property int panelWidth: 400

    function countStageRow() {
        let countRow = 0;
        for (let a = 0; a < 10; a++) {
            countRow += moreStage[a]
        }

        flineStages.height = countRow*headerHeight
    }

    Item {
        id: flineStages
        width: panelWidth
        height: 5*headerHeight
        Column{
            anchors.fill: parent
            Repeater {
                model: selectCurrFLine ? selectCurrFLine.taskStages : null
                delegate: MouseArea{
                    property var stageObj: object
                    property int stageHeight: headerHeight
                    property int stageCount: object.countStage
                    property int rowCount: stageCount > 0 ? Math.ceil(stageCount/4) : 1

                    id: lineStages
                    width: panelWidth
                    height: stageHeight*rowCount
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    Connections {
                        target: stageObj
                        function onCountStagesChanged() {
                            moreStage[index] = rowCount
                            countStageRow()
                        }
                    }

                    Component.onCompleted: {
                        moreStage[index] = rowCount
                        countStageRow()
                    }

                    onClicked: (mouse) => {
                        if (stageObj && (mouse.button === Qt.RightButton)){
                            stageObj.removeAllStages()
                        }
                    }

                    onDoubleClicked: (mouse) => {
                        const globalPos = lineStages.mapToItem(null, 0, 0)
                        missionMgr.selectTreeIndex(stageObj.code)
                        resetTerrainCurve() // reset variables
                        currFlineStage = stageObj              
                        if (editorWaypoint || (selectCurrFLine && selectCurrFLine.unsaved)) {
                            treeView.y = globalPos.y + stageHeight*rowCount
                            treeView.x = globalPos.x
                            treeView.open()
                        }
                    }
                    ControlBackground {
                        anchors.fill: parent
                        GcsToggleButton {
                            id: typeStage
                            borderColor: GcsTheme.frameColor
                            anchors.left: parent.left
                            height: stageHeight*rowCount
                            width: optPanelWidth/4
                            text: stageObj?stageObj.name:qsTr("")
                        }
                        Grid {
                            anchors.left: typeStage.right
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.topMargin: 2
                            anchors.leftMargin: 2
                            height: stageHeight*rowCount-2
                            columns: 4
                            spacing: 1
                            Repeater{
                                model: stageObj?stageObj.stagesMode:null
                                delegate: GcsToggleButton {
                                    property var stageItem: object
                                    property int currCode: stageItem?stageItem.stage:0
                                    textColor: GcsTheme.dark?GcsTheme.White:GcsTheme.Black
                                    borderColor: stageItem && stageItem.defNode ?GcsTheme.primaryColor :GcsTheme.itemGrayColor
                                    normalColor: flineStage === stageItem? GcsTheme.primaryColor: GcsTheme.itemNormalColor
                                    text: stageItem?stageItem.name:qsTr("")
                                    width: optPanelWidth*3/16-5
                                    height: itemHeight
                                    onClicked: {
                                        resetTerrainCurve()
                                        currFlineStage = stageObj
                                        flineStage = stageItem
                                        fStageCode = currCode //
                                        flineStage.recalWaypointPath();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    GcsDivider{
        id: operate
        anchors.top: flineStages.bottom
        width: panelWidth
        contrast: true
        spacing: 0
    }

    Popup{
        id: treeView
        width: panelWidth
        height: 300
        padding: 1
        modal: false
        parent: Overlay.overlay
        GcsTreeViewAction{
            id: stageTreeView
            height: parent.height
            width: panelWidth
            tableModel: missionMgr.stageTree
            selectListener: (type, mode) => {
                if (currFlineStage) {
                    let vz = currFlineStage.appendRemoveStage(mode)
                    if( vz === GcsLineOperate.TaskOperationSuccess){
                        toastShow(qsTr("更新航段成功"), 2000)
                    } else if(vz === GcsLineOperate.InvalidParameter){
                        toastShow(qsTr("输入参数无效"), 2000)
                    } else if(vz === GcsLineOperate.UnableUpdateStage){
                        toastShow(qsTr("不支持选择的航段"), 2000)
                    } else if(vz === GcsLineOperate.TaskStageOverLimit){
                        toastShow(qsTr("添加过多航段"), 2000)
                    }else {
                        toastShow(qsTr("更新航段失败"), 2000)
                    }
                }
            }
        }
    }

    Item {
        id: showDetail
        anchors.left: parent.left
        anchors.top: operate.bottom
        anchors.bottom: parent.bottom
        width: panelWidth
        Component {
            id: flineWaypoint
            Item {
                property var wayObj: object
                function checkeStateFunc() {
                    return (lastSelWaypoint == index) ? true : false
                }

                height: itemHeight
                width: panelWidth/4
                GcsToggleButton {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    height: itemHeight
                    width: panelWidth/4
                    checkeState: checkeStateFunc()
                    text: qsTr("航点: ") + (index  + 1)
                    onClicked: {
                        if (lastSelWaypoint === index) {
                            lastSelWaypoint = -1
                            resetWaypointShow()
                        } else {
                            if ((index === 0) && (fStageCode === GcsLineStage.Curise)){
                                takeoffPoint = true
                            }
                            else {
                                takeoffPoint = false
                            }
                            currObjectItem = wayObj
                            lastSelWaypoint = index
                        }
                    }
                }
            }
        }
        ListView{
            id: waypointView
            width: panelWidth/4
            height: parent.height
            anchors.left: parent.left
            ScrollBar.vertical: GcsScrollBar { }
            ScrollIndicator.vertical: GcsScrollIndicator {}
            boundsBehavior: Flickable.StopAtBounds
            clip: true
            focus: true
            model: flineStage ? flineStage.visualPoints : null
            delegate: flineWaypoint
        }
        Item{
            visible: takeoffPoint
            anchors.right: parent.right
            anchors.left: waypointView.right
            anchors.leftMargin: 5
            height: parent.height
            FmpTakeoffWaypointEdit{
                stageLine: flineStage
                currWaypoint: currObjectItem
            }
        }
        ScrollView{
            id: normalArea
            visible: !takeoffPoint
            anchors.right: parent.right
            anchors.left: waypointView.right
            anchors.leftMargin: 5
            height: parent.height
            clip: true
            FmpWaypointEdit{
                id: normalpoint
                stageLine: flineStage
                currWaypoint: currObjectItem
                waypointTypes: (flineStage && fStageCode === GcsLineStage.Curise) ? flineStage.stagePointType: null
                removeWaypoint: function (point) {
                    flineStage.removeWaypoint(point)
                }
            }
        }
    }
}

