import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtLocation 5.15
import QtPositioning 5.15
import QtQuick.Window 2.15
import Qt.labs.platform 1.1
import GcsLineStage 1.0
import GcsQuick 1.0

Item {
    id: _root

    property bool updateWaypoint: false
    property bool updateTempWaypoint: false
    property var currVehicle: commandMgr.currentVehicle
    property var currFightLine: missionMgr.currFightLine
    property bool editorWaypoint: false
    property var flineStage: null
    property var currentTool: null
    property int lastSelWaypoint: -1
    property int fStageCode: flineStage ? flineStage.stage: 0

    function doLineAreaMap() {
        if (0 == lineAreaMode) {
            if (currentTool) {
                currentTool.closePath()
                currentTool = null
            }
        }
    }

    FileDialog {
        id: fileSaveDialog
        title: qsTr("文件存储")
        fileMode: FileDialog.SaveFile
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: {
            flineStage.saveAs(fileSaveDialog.currentFile)
        }
        onRejected: {
            fileSaveDialog.close()
        }
    }

    Component{
        id: areacomp
        MapNormArea {
        }
    }

    Component{
        id: rulercomp
        MapNormRuler {
        }
    }

    FlightMapBase {
        id: flyMap
        anchors.fill:  parent

        onMapLeftClicked: (mouse) => {
            var coordinate = flyMap.toCoordinate(Qt.point(mouse.x, mouse.y), false)

            if (updateWaypoint && flineStage && fStageCode === GcsLineStage.Templine) {
                coordinate.latitude = coordinate.latitude.toFixed(decimalPlaces)
                coordinate.longitude = coordinate.longitude.toFixed(decimalPlaces)
                coordinate.altitude = coordinate.altitude.toFixed(2)
                showMapLocation(coordinate.latitude, coordinate.longitude, coordinate.altitude);
                flineStage.insertItem(coordinate);
            }
            else if (lineAreaMode == 2) {
                if(!currentTool){
                    currentTool=rulercomp.createObject(flyMap);
                    if(currentTool) {
                        flyMap.addMapItemGroup(currentTool);
                    }
                }
                if(currentTool){
                    currentTool.appendPoint(coordinate);
                }
            } else if(lineAreaMode == 1){
                if(!currentTool){
                    currentTool=areacomp.createObject(flyMap);
                    currentTool.centerPos = coordinate;
                    if(currentTool) {
                        flyMap.addMapItemGroup(currentTool);
                    }
                }
                if(currentTool){
                    currentTool.appendPoint(coordinate);
                }
            }
        }

        onMapRightClicked: (mouse) => {
            updateTempWaypoint = false
            if (lineAreaMode > 0) {
                lineAreaMode = 0
            }
        }

        MapQuickItem {
            id: vehIcon
            z: configMgr.zOrderVehicles
            anchorPoint.x: sourceItem.width / 2
            anchorPoint.y: sourceItem.height / 2
            coordinate: currVehicle.position
            rotation: currVehicle.heading
            visible: true
            sourceItem: Image {
                source: "qrc:///res/plane-v.svg"
                fillMode: Image.PreserveAspectFit
                height: 32
                width: 32
            }
        }

        Repeater {
            model: currFightLine ? currFightLine.paths : null
            delegate: MapDotPolyline {
                mapItem: flyMap
                lineColor: object.lineColor
                z: configMgr.zOrderWaypointLines
                endCoordinate: object.epos
                beginCoordinate: object.spos
            }
        }

        Repeater {
            model: currFightLine ? currFightLine.stages : null
            delegate: MapGroupStage {
                typeStage: object.stageAbbr
                stageLine: object
                mapNode: flyMap
            }
        }
    }

    ControlBackground {
        z: configMgr.zOrderMapItems
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: optPanelWidth
        height: parent.height

        MouseArea{
            anchors.fill: parent
            onWheel: (wheel) => {wheel.accepted = true}
            FmpFLineMission {
                anchors.fill: parent
            }
        }
    }
}
