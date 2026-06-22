import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import GcsLineStage 1.0
import GcsQuick 1.0

Item {
    id: root

    property var flineTable: flineModel.tableModel
    property int flineTableRow: flineTable.count
    property bool updateFWaypoint: false
    property int stageTableView: 400
    property int showVehMission: -1
    property int selectVehicle: 0
    property int itemHeight: 32
    property int headerHeight: 36

    function updateRowSelect() {
        flineStage = null
        flineDB.updateRowSelect();
    }

    function clearSelectStage() {
        editorWaypoint = false
    }

    opacity: 0.8
    width: optPanelWidth

    Item {
        id: flineTree
        height: parent.height/4
        width: optPanelWidth
        GcsTreeViewExt {
            id: flineDB
            anchors.fill: parent
            tableModel: flineTable
            columnProvider: (column)=>{
                if (column === 0) return 70;
                if (column === 2) return 120;
                return optPanelWidth-190;
            }
            // 选择飞行任务
            selectStageListener: (elem, row) => {
                selectCurrFLine = elem
                resetTerrainCurve();
                flineModel.selectFightline(elem)
            }
            // 删除飞行任务
            removeListener:(elem, row) => {
                flineModel.removeFlightLine(elem)
            }
            // 保存飞行任务
            updateListener:(elem, row) => {
                selectCurrFLine = elem
                flineModel.selectFightline(elem)
                if (elem.name.length > 0) {
                    if (elem.unsaved) {
                        flineModel.updateFlightLine(elem)
                        editorWaypoint = false
                    } else {
                        elem.unsaved = true
                        editorWaypoint = true
                    }
                } else {
                    if (selectCurrFLine && elem.unsaved) {
                        flineDlg.currFline = selectCurrFLine
                        flineDlg.flineModel = flineModel
                        flineDlg.clearStage = clearSelectStage
                        flineDlg.open()
                    } else {
                        elem.unsaved = true
                        editorWaypoint = true
                    }
                }
            }
        }
    }

    GcsDivider{
        id: currOperate
        anchors.bottom: flineTree.bottom
        contrast: true
        width: optPanelWidth
        spacing: 0
    }

    FmpFLineDetail {
        id: selectFline
        width: optPanelWidth
        anchors.top: currOperate.bottom
        anchors.bottom: parent.bottom
    }
}
