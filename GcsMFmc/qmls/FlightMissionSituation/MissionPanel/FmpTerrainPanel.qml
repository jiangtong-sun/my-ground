import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property int selectPanel: 0
    property string landName: "着陆" //configMgr.landConfig.landName

    Connections {
        target: missionMgr.terrainDraw
        function onDrawStageTerrain(stage) {
            if (stage === flineStage) { // 选择的航线段
                terrainItem.callRedrawTerrainStage(stage)
            }
        }
        function onResetDrawTerrain() {
            terrainItem.resetDrawTerrain()
        }
    }


    Connections {
        target: currVehicle?currVehicle.MLD: null
        function onUpdateColorValue(val, va){
            if (val === landName) {
                selectPanel = 2 // keep
                flightLand.landFlag = true
            } else {
                flightLand.landFlag = false
            }
        }
    }

    ControlBackground {
        id: barPanel
        width: 30
        height: parent.height
        anchors.left: parent.left
        Column {
            anchors.fill: parent
            GcsToggleButton {
                width: 30
                height: parent.height/3
                checkeState: selectPanel===0?true:false
                text: qsTr("飞\n参")
                onClicked: {
                    selectPanel = 0;
                }
            }
            GcsToggleButton {
                width: 30
                height: parent.height/3
                checkeState: selectPanel===1?true:false
                text: qsTr("姿\n态")
                onClicked: {
                    selectPanel = 1;
                }
            }
            GcsToggleButton {
                width: 30
                height: parent.height/3
                checkeState: selectPanel===2?true:false
                text: qsTr("下\n滑")
                onClicked: {
                    selectPanel = 2;
                }
                onDoubleClicked: {
                    flightLand.resetCurveData()
                }
            }
        }
    }

    Item{
        anchors.left: barPanel.right
        anchors.right: parent.right
        height: parent.height
        width: parent.width
        TerrainFlightParam {
            id: terrainItem
            anchors.fill: parent
            visible: selectPanel === 0?true:false
        }
        ControlBackground {
            anchors.fill: parent
            visible: selectPanel === 1?true:false
        }
        LandingFlightParam {
            id: flightLand
            anchors.fill: parent
            visible: selectPanel === 2?true:false
        }
    }
}
