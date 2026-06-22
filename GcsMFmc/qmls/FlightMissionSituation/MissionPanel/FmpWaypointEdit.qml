import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import GcsWaypointMode 1.0
import GcsLineStage 1.0
import GcsQuick 1.0

Item{

    property var stageLine: null
    property int rowItemHeight: 30
    property var currWaypoint: null
    property var waypointTypes: null
    property int waypointMode: currWaypoint ? currWaypoint.pointType: -1
    property int labelWidth: panelWidth/3-20

    property var removeWaypoint: function(waypoint){ }

    function clampSlideState(mode) {
        if (mode === GcsWaypointMode.TurnRoundPoint) {
            return true;
        }
        if (stageLine && (stageLine.stage === GcsLineStage.Curise)) {
            return true
        }
        return false
    }

    function turnModeState(mode) {
        if (stageLine && (stageLine.stage === GcsLineStage.Curise ||
                stageLine.stage === GcsLineStage.Templine)) {
            return true
        }
        return false
    }

    function nextWaypointState(mode) {
        if (stageLine && (stageLine.stage === GcsLineStage.Curise)) {
            return true
        }
        return false
    }

    function speedState() {
        //stageLine.stage === GcsLineStage.Ergency
        if (stageLine && (stageLine.stage === GcsLineStage.Curise)) {
            return true
        }
        return false
    }

    Item{
        id: detail
        height: rowItemHeight
        width: parent.width
        GcsText {
            id: detailPoint
            text: qsTr("【航点详情】")
            height: rowItemHeight
            width: 120
        }
        GcsToggleSwitch {
            id: syncUpt
            width: 80
            text: qsTr("同步")
            height: rowItemHeight
            anchors.left: detailPoint.right
        }
        GcsIconButton{
            padding: 0
            verticalPadding: 0
            horizontalPadding: 0
            anchors.left: syncUpt.right
            anchors.leftMargin: 20
            height: rowItemHeight
            width: 30
            visible: {
                if(editorWaypoint && stageLine){
                    return true
                }
                return false
            }
            iconSource: ttfIcons["closeBold"]
            Layout.alignment: Qt.AlignVCenter
            onClicked: {
                removeWaypoint(currWaypoint)
            }
        }
    }

    Column {
        anchors.fill: parent
        anchors.rightMargin: 20
        anchors.topMargin: rowItemHeight+10
        visible: currWaypoint? true: false
        spacing: 5
        RowLayout {
            GcsText {
                Layout.minimumWidth: labelWidth
                Layout.minimumHeight: rowItemHeight
                Layout.alignment: Qt.AlignRight
                horizontalAlignment: Text.AlignRight
                text: qsTr("经度（°）:")
            }
            WaypointEditing{
                id: lngInput
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3+20
                Layout.minimumHeight: rowItemHeight
                text: currWaypoint ? currWaypoint.lng : 0
                editorPoint: !editorWaypoint
                editFinishListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.lng = Number(lngInput.text)
                    }
                }
                commitListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.lng = Number(lngInput.text)
                    }
                }
            }
        }

        RowLayout {
            GcsText {
                Layout.minimumWidth: labelWidth
                Layout.minimumHeight: rowItemHeight
                Layout.alignment: Qt.AlignRight
                horizontalAlignment: Text.AlignRight
                text: qsTr("纬度（°）:")
            }
            WaypointEditing{
                id: latInput
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3+20
                Layout.minimumHeight: rowItemHeight
                text: currWaypoint ? currWaypoint.lat : 0
                editorPoint: !editorWaypoint
                editFinishListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.lat = Number(latInput.text)
                    }
                }
                commitListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.lat = Number(latInput.text)
                    }
                }
            }
        }

        RowLayout {
            GcsText {
                Layout.alignment: Qt.AlignRight
                Layout.minimumWidth: labelWidth
                Layout.minimumHeight: rowItemHeight
                horizontalAlignment: Text.AlignRight
                text: qsTr("高度（m）:")
            }
            WaypointEditing{
                id: altInput
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3+20
                Layout.minimumHeight: rowItemHeight
                text: currWaypoint ? currWaypoint.alt : 0
                editorPoint: !editorWaypoint
                editFinishListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.alt = Number(altInput.text)
                        if (syncUpt.checked && stageLine) {
                            stageLine.syncStateF(4, currWaypoint.alt);
                        }
                    }
                }
                commitListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.alt = Number(altInput.text)
                        if (syncUpt.checked && stageLine) {
                            stageLine.syncStateF(4, currWaypoint.alt);
                        }
                    }
                }
            }
        }

        RowLayout {
            visible: speedState()
            GcsText {
                Layout.minimumWidth: labelWidth
                Layout.minimumHeight: rowItemHeight
                horizontalAlignment: Text.AlignRight
                Layout.alignment: Qt.AlignRight
                text: qsTr("速度（m/s）:")
            }
            WaypointEditing{
                id: speedInput
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3+20
                Layout.minimumHeight: rowItemHeight
                text: currWaypoint ? currWaypoint.speed : 0
                editorPoint: !editorWaypoint
                editFinishListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.speed = Number(speedInput.text)
                        if (syncUpt.checked && stageLine) {
                            stageLine.syncStateF(3, currWaypoint.speed);
                        }
                    }
                }
                commitListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.speed = Number(speedInput.text)
                        if (syncUpt.checked && stageLine) {
                            stageLine.syncStateF(3, currWaypoint.speed);
                        }
                    }
                }
            }
        }

        RowLayout {
            visible: currWaypoint && currWaypoint.turnMode === 1 ? true: false
            GcsText {
                Layout.minimumWidth: labelWidth
                Layout.minimumHeight: rowItemHeight
                horizontalAlignment: Text.AlignRight
                Layout.alignment: Qt.AlignRight
                text: qsTr("过点半径（m）:")
            }
            WaypointEditing{
                id: radiusInput
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3+20
                Layout.minimumHeight: rowItemHeight
                text: currWaypoint ? currWaypoint.turnRadius : 0
                editorPoint: !editorWaypoint
                editFinishListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.turnRadius = Number(radiusInput.text)
                        if (syncUpt.checked && stageLine) {
                            stageLine.syncStateF(2, currWaypoint.turnRadius);
                        }
                    }
                }
                commitListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.turnRadius = Number(radiusInput.text)
                        if (syncUpt.checked && stageLine) {
                            stageLine.syncStateF(2, currWaypoint.turnRadius);
                        }
                    }
                }
            }
        }

        RowLayout {
            GcsText {
                horizontalAlignment: Text.AlignLeft
                text: qsTr("高度类型:")
            }
            GroupBox {
                background: Item{ height: rowItemHeight}
                Layout.minimumHeight: rowItemHeight
                height: rowItemHeight
                anchors.margins: 1
                padding: 1
                RowLayout {
                    anchors.fill: parent
                    GcsRadioButton {
                        checked:  currWaypoint && currWaypoint.heightMode === 0 ? true : false
                        disabled: !editorWaypoint
                        text: qsTr("标准")
                        clickListener: () => {
                            currWaypoint.heightMode = 0
                            if (syncUpt.checked && stageLine) {
                                stageLine.syncStateI(1, 0);
                            }
                        }
                    }
                    GcsRadioButton {
                        checked:  currWaypoint && currWaypoint.heightMode === 1 ? true : false
                        disabled: !editorWaypoint
                        text: qsTr("海拔")
                        clickListener: () => {
                            currWaypoint.heightMode = 1
                            if (syncUpt.checked && stageLine) {
                                stageLine.syncStateI(1, 1);
                            }
                        }
                    }
                    GcsRadioButton {
                        checked:  currWaypoint && currWaypoint.heightMode === 2 ? true : false
                        disabled: !editorWaypoint
                        text: qsTr("场高")
                        clickListener: () => {
                            currWaypoint.heightMode = 2
                            if (syncUpt.checked && stageLine) {
                                stageLine.syncStateI(1, 2);
                            }
                        }
                    }
                }
            }
        }

        RowLayout {
            visible: clampSlideState(waypointMode)
            GcsText {
                horizontalAlignment: Text.AlignLeft
                text: qsTr("爬升下滑:")
            }
            GroupBox {
                background: Item{ height: rowItemHeight}
                Layout.minimumHeight: rowItemHeight
                height: rowItemHeight
                anchors.margins: 1
                padding: 1
                RowLayout {
                    anchors.fill: parent
                    GcsRadioButton {
                        checked:  currWaypoint && currWaypoint.clampSlide === 0 ? true : false
                        disabled: !editorWaypoint
                        text: qsTr("直线")
                        clickListener: () => {
                            currWaypoint.clampSlide = 0
                        }
                    }
                    GcsRadioButton {
                        checked:  currWaypoint && currWaypoint.clampSlide === 1 ? true : false
                        disabled: !editorWaypoint
                        text: qsTr("左旋")
                        clickListener: () => {
                            currWaypoint.clampSlide = 1
                        }
                    }
                    GcsRadioButton {
                        checked:  currWaypoint && currWaypoint.clampSlide === 2 ? true : false
                        disabled: !editorWaypoint
                        text: qsTr("右旋")
                        clickListener: () => {
                            currWaypoint.clampSlide = 2
                        }
                    }
                }
            }
        }

        RowLayout {
            visible: turnModeState(waypointMode)
            GcsText {
                horizontalAlignment: Text.AlignLeft
                text: qsTr("转弯方式:")
            }
            GroupBox {
                background: Item{ height: rowItemHeight}
                Layout.minimumHeight: rowItemHeight
                height: rowItemHeight
                anchors.margins: 1
                padding: 1
                RowLayout {
                    anchors.fill: parent
                    GcsRadioButton {
                        checked:  currWaypoint && currWaypoint.turnMode === 0 ? true : false
                        disabled: !editorWaypoint
                        text: qsTr("默认")
                        clickListener: () => {
                            currWaypoint.turnMode = 0
                        }
                    }
                    GcsRadioButton {
                        checked:  currWaypoint && currWaypoint.turnMode === 1 ? true : false
                        disabled: !editorWaypoint
                        text: qsTr("过点")
                        clickListener: () => {
                            currWaypoint.turnMode = 1
                        }
                    }
                }
            }
        }

        RowLayout {
            visible: nextWaypointState(waypointMode)
            GcsText {
                Layout.alignment: Qt.AlignRight
                text: qsTr("离开方式:")
            }
            GroupBox {
                background: Item{ height: rowItemHeight}
                Layout.minimumHeight: rowItemHeight
                height: rowItemHeight
                anchors.margins: 1
                padding: 1
                RowLayout {
                    anchors.fill: parent
                    GcsRadioButton {
                        checked:  currWaypoint && currWaypoint.nextMode === 0 ? true : false
                        disabled: !editorWaypoint
                        text: qsTr("最近")
                        clickListener: () => {
                            currWaypoint.nextMode = 0
                        }
                    }
                    GcsRadioButton {
                        checked:  currWaypoint && currWaypoint.nextMode === 1 ? true : false
                        disabled: !editorWaypoint
                        text: qsTr("中线")
                        clickListener: () => {
                            currWaypoint.nextMode = 1
                        }
                    }
                }
            }
        }
    }
}

