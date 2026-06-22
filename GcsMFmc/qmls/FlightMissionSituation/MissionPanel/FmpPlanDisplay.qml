import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15
import GcsLineStage 1.0
import GcsQuick 1.0

Item {
    id: planpanel

    property var gisTool: null
    property var gisSetpoint: null
    property var flineStage: null
    property int selectOperate: 0
    property int operateHeight: 40
    property int fStageCode: -1
    property int visualDrag: -1
    property bool updateWaypoint: false
    property bool editorWaypoint: false
    property int lastSelWaypoint: -1
    property int showStageDetail: 0
    property var currObjectItem: null
    property var vehMissions: missionMgr.vehMissions
    property var vehicleList: commandMgr.aircrafts
    property var stageName: missionMgr.stageName
    property var modelStages: missionMgr.stageModels
    property var flineModel: missionMgr.flineModel
    property int itemWidth: optPanelWidth/stageName.length
    property var setDestPoint: null
    property var currFlineStage: null
    property var stageModel: null
    property int stageCode: -1
    property int lineWaypoint: 0

    property var ergencyMinRadis: [1040, 1200,
        1760, 1760, 2200, 2800, 2640, 1200, 1040,
        1760, 1760, 2200, 2800, 2640, 1200, 1040,
        1760, 1200, 1200 ]
    property var ergencyMaxRadis: [4200, 2640,
        2640, 3000, 4200, 4200, 3960, 3600, 1800,
        2640, 3000, 4200, 4200, 3960, 3600, 4200,
        3480, 3480, 3480]
    property int vehErgencyMinRadis: 1040
    property int vehErgencyMaxRadis: 4200

    function resetFlineSelect() {
        if (currObjectItem != null) {
            currObjectItem = null
        }
        editorWaypoint = false
        updateWaypoint = false
    }

    function switchStageSelect(index) {
        if (currObjectItem != null) {
            currObjectItem = null
        }
        editorWaypoint = false
        updateWaypoint = false
    }

    function resetTerrainCurve() {
        if (missionMgr.terrainDraw) {
            missionMgr.terrainDraw.resetTerrainCurve()
        }
        lastSelWaypoint = -1
        currObjectItem = null
        currFlineStage = null
        flineStage = null
    }

    function mapCenter() {
        var coordinate = editorMap.center
        coordinate.latitude  = coordinate.latitude.toFixed(decimalPlaces)
        coordinate.longitude = coordinate.longitude.toFixed(decimalPlaces)
        coordinate.altitude  = coordinate.altitude.toFixed(decimalPlaces)
        return coordinate
    }

    function resetLineAreaMap(value) {
        /// 选中的车辆居中显示
        if (value === -1 && selVehMission){
            var veh = selVehMission.currVehicle
            editorMap.center = veh.position
            return
        }
        // 清除车辆的航迹信息
        if (value === -2){
            let vehCnt = vehiclePolyline.count
            for (var x = 0; x < vehCnt; x++) {
                var item = vehiclePolyline.itemAt(x)
                console.log("===>vehilce ", item)
                item.path = []
            }
        }
        // 关闭tool条
        if (gisTool) {
            gisTool.closePath()
            gisTool = null
        }
        lineAreaMode = value
        updateWaypoint = false
    }

    Connections {
        target: currVehicle?currVehicle.LineStage: null
        function onUpdateColorValue(val, va){
            var cur = parseInt(val === "NA" ? "0" : val)
            if (cur !== lineWaypoint) {
                if (cur > ergencyMinRadis.length) {
                    cur = 0;
                }
                lineWaypoint = cur
                vehErgencyMinRadis = ergencyMinRadis[cur]
                vehErgencyMaxRadis = ergencyMaxRadis[cur]
            }
        }
    }

    Component.onCompleted: {
        missionMgr.loadAll()
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

    Component{
        id: stagecomp
        MapGroupStage {
            stageLine: object
            mapNode: editorMap
        }
    }

    Component{
        id: setpointComp
        MapItemGroup {
            function appendPoint(coord){
                setpointOpt.append({"latvalue":coord.latitude, "lngvalue":coord.longitude});
                if(setpointOpt.count > 1) {
                    setpointOpt.remove(0);
                }
            }
            MapItemView {
                add: Transition {}
                remove: Transition {}
                model: ListModel {
                    id: setpointOpt
                }
                delegate: MapQuickItem{
                    anchorPoint.x: 10
                    anchorPoint.y: 10
                    coordinate: QtPositioning.coordinate(model.latvalue, model.lngvalue)
                    sourceItem: ControlBackground {
                        color: GcsTheme.orange
                        borderColor: GcsTheme.orange
                        radius: 10
                        height: 20
                        width: 20
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.RightButton
                            onClicked: (mouse) => {
                                console.log("===>RightButton")
                            }
                        }
                    }
                }
            }
        }
    }

    FlightMapBase {
        id: editorMap
        anchors.fill:  parent
        z: configMgr.zOrderBaseMap

        onShowMapPosition: (coord) => {
            showMapLocation(coord.latitude.toFixed(decimalPlaces),
                            coord.longitude.toFixed(decimalPlaces),
                            coord.altitude.toFixed(decimalPlaces));
        }

        onMapLeftClicked: (mouse) => {
            var coordinate = editorMap.toCoordinate(Qt.point(mouse.x, mouse.y), false)
            if (updateWaypoint) {
                if (flineStage) {
                    flineStage.insertItem(coordinate);
                } else {
                    stageModel.insertWaypoint(coordinate);
                }
            }
            else if (lineAreaMode == 2) {
                if(!gisTool){
                    gisTool=rulercomp.createObject(editorMap);
                    if(gisTool) {
                        editorMap.addMapItemGroup(gisTool);
                    }
                }
                if(gisTool){
                    gisTool.appendPoint(coordinate);
                }
            }
            else if(lineAreaMode == 1){
                if(!gisTool){
                    gisTool=areacomp.createObject(editorMap);
                    gisTool.centerPos = coordinate;
                    if(gisTool) {
                        editorMap.addMapItemGroup(gisTool);
                    }
                }
                if(gisTool){
                    gisTool.appendPoint(coordinate);
                }
            }
            else if(selectOperate === 0) {
                if (!gisSetpoint) {
                    gisSetpoint = setpointComp.createObject(editorMap);
                    if(gisSetpoint) {
                        editorMap.addMapItemGroup(gisSetpoint);
                    }
                }
                if (gisSetpoint) {
                    gisSetpoint.appendPoint(coordinate);
                }
                setDestPoint = coordinate
            }

            showMapLocation(coordinate.latitude.toFixed(decimalPlaces),
                  coordinate.longitude.toFixed(decimalPlaces),
                  coordinate.altitude.toFixed(decimalPlaces));
        }

        onMapRightClicked: (mouse) => {
            resetLineAreaMap(0)
        }

        MapQuickItem {
            z: configMgr.zOrderVehicles
            coordinate: setDestPoint?setDestPoint:editorMap.center
            anchorPoint.x: 2
            anchorPoint.y: 2
            sourceItem: ControlBackground {
                color: GcsTheme.green
                borderColor: GcsTheme.green
                radius: 2
                height: 4
                width: 4
            }
        }

        MapQuickItem {
            z: configMgr.zOrderVehicles
            coordinate: configMgr.gcsCenter
            anchorPoint.x: sourceItem.width / 2
            anchorPoint.y: sourceItem.height / 2
            sourceItem: Image {
                source: "qrc:///res/center.png"
                fillMode: Image.PreserveAspectFit
                height: 32
                width: 32
            }
        }

        Repeater{
            model: vehMissions
            delegate: MapCircle {
                property var vehItem: object.currVehicle
                id: vehMinDis
                radius: vehErgencyMinRadis
                color: "transparent"
                center: vehItem.position
                border.color: GcsTheme.green
                border.width: 2
            }
        }

        Repeater{
            model: vehMissions
            delegate: MapCircle {
                property var vehItem: object.currVehicle
                id: vehMaxDis
                radius: vehErgencyMaxRadis
                color: "transparent"
                center: vehItem.position
                border.color: GcsTheme.green
                border.width: 2
            }
        }

        /// 多个车辆的图标信息
        Repeater{
            model: vehMissions
            delegate: MapQuickItem {
                property var vehSession: object
                property var vehItem: object.currVehicle
                property int heading: vehItem.heading

                id: quickVechicle
                z: configMgr.zOrderVehicles
                anchorPoint.x: sourceItem.width / 2
                anchorPoint.y: sourceItem.height / 2
                rotation: heading

                sourceItem: Image {
                    source: "qrc:///res/plane-v.svg"
                    fillMode: Image.PreserveAspectFit
                    height: 32
                    width: 32
                }

                Connections {
                    target: vehItem
                    function onPositionChanged(vehPos, va){
                        quickVechicle.coordinate = vehPos
                    }
                }

                Component.onCompleted: {
                    quickVechicle.coordinate = vehItem.position
                }
            }
        }

        /// 多个车辆的飞行轨迹信息
        Repeater {
            id: vehiclePolyline
            model: vehicleList
            delegate: MapPolyline {
                property var vehObject: object
                id: polyline
                line.width: 2
                line.color: GcsTheme.green
                z: configMgr.zOrderWaypointLines
                Connections {
                    target: vehObject
                    function onTrajectoryChanged(coord) {
                        polyline.addCoordinate(coord)
                        if (polyline.pathLength() > 5000) {
                            polyline.removeCoordinate(0)
                        }
                    }
                }
            }
        }

        //编辑航线段的信息显示
        MapGroupStage {
            id: stageMapNode
            stageLine: flineStage
            typeStage: flineStage ? flineStage.stageAbbr : ""
            nodeVisible: flineStage ? true : false
            mapNode: editorMap
        }

        Repeater {
            model: selectCurrFLine ? selectCurrFLine.stagePath : null
            delegate: MapDotPolyline {
                mapItem: editorMap
                lineColor: object.lineColor
                z: configMgr.zOrderWaypointLines
                endCoordinate: object.epos
                beginCoordinate: object.spos
            }
        }

        // Repeater {
        //     model: currFlineStage ? currFlineStage.stagesMode  : null
        //     delegate: MapGroupStage {
        //         typeStage: object.stageAbbr
        //         stageLine: object
        //         mapNode: editorMap
        //     }
        // }

        Repeater {
            model: selectCurrFLine ? selectCurrFLine.gmapStages : null
            delegate: MapItemGroup {
                property var taskFlineObj: object
                property bool taskFObjVisbile: taskFlineObj ? taskFlineObj.visible : false
                Repeater {
                    model: taskFlineObj ? taskFlineObj.stagesMode : null
                    delegate: MapGroupStage {
                        property var taskStages: object
                        property bool taskSObjVisbile: taskStages ? taskStages.distState : false
                        nodeVisible: taskFObjVisbile && taskSObjVisbile
                        typeStage: taskStages ? taskStages.stageAbbr : null
                        stageLine: taskStages
                        mapNode: editorMap
                    }
                }
            }
        }
    }

    ControlBackground {
        id: operatePanel
        z: configMgr.zOrderMapItems
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: optPanelWidth
        height: parent.height

        Row{
            anchors.top: parent.top
            width: optPanelWidth
            height: operateHeight
            GcsToggleButton{
                height:operateHeight
                width: optPanelWidth/3
                text: qsTr("【飞行】")
                checkeState: selectOperate===0?true:false
                onClicked: {
                    selectCurrFLine = null
                    resetTerrainCurve()
                    selectOperate = 0;
                }
            }
            GcsToggleButton{
                height:operateHeight
                width: optPanelWidth/3
                text: qsTr("【编辑】")
                checkeState: selectOperate===1?true:false
                onClicked: {
                    selectOperate = 1
                    showStageDetail = 0
                    selVehMission = null
                    selectCurrFLine = null
                    resetTerrainCurve()
                    fstagePanel.updateRowSelect()
                    ftaskPanel.resetWaypointEdit()
                }
            }
            GcsToggleButton{
                height:operateHeight
                width: optPanelWidth/3
                text: qsTr("【任务】")
                checkeState: selectOperate===2?true:false
                onClicked: {
                    selectOperate = 2
                    showStageDetail = 0
                    selVehMission = null
                    selectCurrFLine = null
                    resetTerrainCurve()
                    flinePanel.updateRowSelect()
                    ftaskPanel.resetWaypointEdit()
                }
            }
        }

        MouseArea{
            hoverEnabled: true
            preventStealing: true
            anchors.fill: parent
            anchors.topMargin: operateHeight
            onWheel: (wheel) => {wheel.accepted = true}
            onPositionChanged: (wheel) => { wheel.accepted = true }
            FmpFLinePanel {
                id: ftaskPanel
                visible: selectOperate === 0?true:false
                anchors.fill: parent
            }
            FmpFStagePanel {
                id: fstagePanel
                visible: selectOperate === 1?true:false
                anchors.fill: parent
            }
            FmpFTaskPanel {
                id: flinePanel
                visible: selectOperate === 2?true:false
                anchors.fill: parent
            }
        }
    }

    Item {
        height: 200
        anchors.left: parent.left
        anchors.right: operatePanel.left
        anchors.bottom: parent.bottom
        z: configMgr.zOrderMapItems
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            preventStealing: true
            onWheel: (wheel) => {wheel.accepted = true}
            onPositionChanged: (wheel) => { wheel.accepted = true }
            FmpTerrainPanel {
                anchors.fill: parent
            }
        }
    }

    FmpStagePopup {
        id: saveDlg
    }

    FmpFlinePopup{
        id: flineDlg
    }

    FmpFlineUpload {
        id: uptwaypoint
    }
}
