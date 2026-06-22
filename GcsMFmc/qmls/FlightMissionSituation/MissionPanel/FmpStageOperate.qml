import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    property int operateHeight: 50
    property int optBtnHeight: 30
    property int optBtnWidth: 80
    property int panelWidth: optPanelWidth
    property var stagePoints: currStage.visualPoints
    property int stagePointCount: stagePoints.count

    // function getBtnName() {
    //     if (stageModel.selectId >= 0) {
    //         if (currStage.unsaved) {
    //             return qsTr("保存航段");
    //         } else {
    //             return qsTr("修改航段");
    //         }
    //     } else {
    //         if (currStage.unsaved) {
    //             return qsTr("保存航段");
    //         } else {
    //             return qsTr("新建航段");
    //         }
    //     }
    // }

    // function calBorderColor() {
    //      return editorFlag ? GcsTheme.orange : GcsTheme.itemNormalColor
    // }

    // ControlBackground {
    //     id: operate
    //     anchors.top: parent.top
    //     width: panelWidth
    //     height: operateHeight
    //     Row{
    //         anchors.fill: parent
    //         anchors.margins: 10
    //         spacing: 10
    //         GcsToggleButton {
    //             id: clear
    //             btnRadius: 5
    //             width: optBtnWidth
    //             height: optBtnHeight
    //             text: qsTr("删除航段")
    //             MouseArea{
    //                 anchors.fill: parent;
    //                 hoverEnabled: true;
    //                 onClicked: {
    //                     stageModel.removeStage()
    //                 }
    //             }
    //         }
    //         GcsToggleButton {
    //             id: adds
    //             btnRadius: 5
    //             width: optBtnWidth
    //             height: optBtnHeight
    //             checkeState: updateWaypoint
    //             text: getBtnName()
    //             MouseArea{
    //                 anchors.fill: parent;
    //                 hoverEnabled: true;
    //                 onClicked: {
    //                     if (currStage.unsaved){
    //                         if (stageModel.selectId >= 0) {
    //                             stageModel.updateStageLine()
    //                         }
    //                         else {
    //                             saveDlg.currStage = currStage
    //                             saveDlg.stageModel = stageModel
    //                             saveDlg.open()
    //                         }
    //                     } else{
    //                         //currStage.insertItem(configMgr.mapCenter)
    //                         flineStage = currStage
    //                         editorWaypoint = true
    //                         updateWaypoint = true
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }


}

