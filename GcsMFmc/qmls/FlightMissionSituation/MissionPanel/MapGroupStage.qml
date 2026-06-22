import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15
import GcsLineStage 1.0
import GcsQuick 1.0

MapItemGroup{
    id: control

    property var mapNode: null
    property var stageLine: null
    property bool nodeVisible: true
    property var takeoffPoints: null
    property string typeStage: ""

    function getNormalColor(neareast, index) {
        if (neareast) {
            return GcsTheme.blue
        }

        if (stageLine === flineStage) {
            if (lastSelWaypoint === index) {
                return GcsTheme.green
            }

            return GcsTheme.orange
        }

        return GcsTheme.primaryColor
    }

    function getLineColor() {
        if (stageLine && stageLine === flineStage) {
            return GcsTheme.orange
        }
        if (stageLine) {
            return stageLine.color
        }
        return GcsTheme.frameColor
    }

    Connections {
        target: takeoffPoints
        function onNextPosChanged(nextPos) {
            takeoffLine.replaceCoordinate(1, nextPos)
        }
    }

    MapPolyline {
        id: takeoffLine
        visible: editorWaypoint
        line.color: GcsTheme.green
        line.width: 4
    }

    MapPolyline {
        id: polyline
        line.width: 2
        line.color: getLineColor()
        visible: nodeVisible
        path: stageLine? stageLine.dispayPoints : []
    }

    MapItemView{
        id: item_view
        visible: nodeVisible
        model: stageLine ? stageLine.visualPoints : null
        delegate: MapQuickItem {
            property var objPoint: object
            property bool pointNear: objPoint ? objPoint.neareast: false
            property string pointType: objPoint ? objPoint.typeName + (index + 1) : qsTr("")

            id: flinePoint
            z: lastSelWaypoint === index ? 0xffff: configMgr.zOrderMapItems
            coordinate: objPoint ? objPoint.pos : mapNode.center
            sourceItem: GcsFilledButton {
                normalColor: getNormalColor(pointNear, index)
                width: 24
                height: 24
                radiusVal: 8
                pixelSize: 10
                text: pointType
                x: -10
                y: -10

                MouseArea{
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton|Qt.LeftButton
                    drag.target: editorWaypoint ? flinePoint : null
                    onReleased: (mouse) => {
                        if (editorWaypoint && stageLine === flineStage) {
                            objPoint.updatePosition(flinePoint.coordinate)
                        } else {
                            flinePoint.coordinate = objPoint.pos
                        }
                    }
                    onClicked: {
                        if (mouse.button === Qt.RightButton && selVehMission){
                            var mousePos = mapToItem(planpanel, mouseX, mouseY)
                            uptwaypoint.curStageLine = stageLine
                            uptwaypoint.waypointIdx = index
                            uptwaypoint.x = mousePos.x
                            uptwaypoint.y = mousePos.y
                            uptwaypoint.open()
                        }
                    }
                    onPositionChanged: (mouse) => {
                        if (editorWaypoint && stageLine === flineStage) {
                            objPoint.updatePosition(flinePoint.coordinate)
                        }
                    }
                }
            }

            Component.onCompleted: {
                if ((stageLine.stage === GcsLineStage.Curise)){
                    if ((objPoint.pointType === 1)){
                        takeoffPoints = objPoint
                        takeoffLine.path = []
                        takeoffLine.addCoordinate(objPoint.pos)
                        takeoffLine.addCoordinate(objPoint.nextPos)
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        mapNode.addMapItemGroup(control)
    }

    Component.onDestruction: {
        mapNode.removeMapItemGroup(control)
    }
}
