import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import GcsLineStage 1.0
import GcsQuick 1.0

Item {
    id: root

    property bool switchFline: false
    property bool selectFline: false
    property bool showTempline: false
    property int stageTableView: 400
    property int showVehMission: -1
    property int selectVehicle: 0
    property int itemHeight: 32

    function updateRowSelect() {
        stageView.updateRowSelect();
    }

    opacity: 0.8
    width: optPanelWidth

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: root.top
        anchors.bottom: parent.bottom
        width: optPanelWidth

        Item {
            id: editmode
            anchors.top: parent.top
            height: modelStages.count *itemHeight
            width: optPanelWidth/3
            Column{
                anchors.fill: parent
                Repeater {
                    model: modelStages
                    ControlBackground {
                        property var stageObj: object
                        width: optPanelWidth/3
                        height: itemHeight
                        color: {
                            if(stageModel === stageObj){
                                return GcsTheme.primaryColor
                            }else{
                                return GcsTheme.itemNormalColor
                            }
                        }
                        MouseArea{
                            anchors.left: parent.left
                            width: optPanelWidth/3-50
                            height: parent.height
                            GcsText {
                                anchors.fill: parent
                                text: stageObj.stageName
                            }
                            onClicked: {
                                resetTerrainCurve()
                                switchStageSelect(index)
                                stageModel = stageObj
                                stageModel.selectId = -1
                                stageView.currentRow = -1
                                stageCode = stageModel.stageCode

                            }
                        }
                        GcsButton {
                            width: 50
                            height: parent.height
                            anchors.right: parent.right
                            visible: stageModel === stageObj ? true : false
                            text: qsTr("新建")
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (stageObj.addStage()){
                                        resetTerrainCurve()
                                        switchStageSelect(index)
                                        stageModel = stageObj
                                        stageCode = stageModel.stageCode
                                        editorWaypoint = true
                                        updateWaypoint = true
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        GcsTreeView {
            id: stageView
            anchors.top: parent.top
            anchors.left: editmode.right
            anchors.leftMargin: 2
            height: modelStages.count *itemHeight
            width: optPanelWidth*2/3-2
            tableModel: stageModel ? stageModel.dataModel : null
            removeListener: (elem, row) => {
                stageModel.removeStage(flineStage === elem? true: false, row)
            }
            updateListener: (elem, row) => {
                if (elem.unsaved) {
                    //文件未保存，且文件名为不为空
                    if (elem.name.length > 0) {
                        stageModel.updateStageLine()
                        editorWaypoint = false
                        updateWaypoint = false
                    } else {
                        saveDlg.currStage = elem
                        saveDlg.stageModel = stageModel
                        saveDlg.open()
                    }
                } else {
                    elem.unsaved = true
                    editorWaypoint = true
                    updateWaypoint = true
                    flineStage = elem
                }
            }
            selectListener: (elem, row) => {
                stageModel.selectId = row
                flineStage = elem
                flineStage = elem
                lastSelWaypoint = -1
                currObjectItem = null
            }
        }

        GcsDivider{
            anchors.bottom: editmode.bottom
            contrast: true
            width: optPanelWidth
            spacing: 0
        }

        FmpStageDetail {
            anchors.top: stageView.bottom
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            width: optPanelWidth
        }
    }
}
