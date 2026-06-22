import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import GcsLineOperate 1.0
import "MissionPanel"
import GcsQuick 1.0

Window {
    readonly property int decimalPlaces: 7
    property var currVehicle: null
    property var selectCurrFLine: null
    property var selVehMission: null
    property var flineDetailStage: null
    property int lineAreaMode: 0
    property bool removeFlightPath: false
    property bool visibleFlightPath: false
    property string showNoticeMessage: ""
    property string historyMessage: ""
    property int winHeight: configMgr.fmpLocate.height
    property int winWidth: configMgr.fmpLocate.width
    property int optPanelWidth: 400
    property int headHeight: 60

    id: fmpwindow
    visible: true
    title: qsTr("GCSFMP")
    flags: Qt.FramelessWindowHint|Qt.Window|configMgr.staysOnTopHint //|Qt.WindowStaysOnTopHint
    screen: Qt.application.screens[configMgr.fmpLocate.screenId]
    x: screen.virtualX + configMgr.fmpLocate.xpos+1
    y: screen.virtualY + configMgr.fmpLocate.ypos
    width: winWidth-1
    height: winHeight

    color: GcsTheme.backgroundColor

    function showMapLocation( lat, lng, alt) {
        var showLocation = "";
        showLocation += "纬度：" + lat
        showLocation += " 经度：" + lng
        showLocation += " 高度：" + alt
        showNoticeMessage = showLocation
    }

    function toastShow(message, showTime) {
        historyMessage = message
    }

    FileDialog {
        id: saveDialog
        title: qsTr("选择文件保存路径")
        folder: shortcuts.home
        selectExisting: false
        nameFilters: [qsTr("文本文件 (*.txt)"), qsTr("所有文件 (*)")]
        onAccepted: {
            let path =  saveDialog.fileUrl.toString().replace(/^(file:\/{3})/,"");
            if (selectCurrFLine) {
                var res2 = selectCurrFLine.exportFlightLine(path);
                if (GcsLineOperate.MissionExportSuccess !== res2) {
                    toastShow(qsTr("任务文件导出失败"), 2000)
                }
            }
            else if (selVehMission) {
                var res = selVehMission.exportFlightLine(path);
                if (GcsLineOperate.MissionExportSuccess !== res) {
                    toastShow(qsTr("任务文件导出失败"), 2000)
                }
            }
        }
    }

    FileDialog {
        id: loadDialog
        title: qsTr("选择要加载的文件")
        folder: shortcuts.home
        nameFilters: [qsTr("文本文件 (*.txt)"), qsTr("所有文件 (*)")]
        onAccepted: {
            let path =  loadDialog.fileUrl.toString().replace(/^(file:\/{3})/,"");
            if (selectCurrFLine) {
                var res2 = selectCurrFLine.loadFlightLine(path, true) ;
                if (GcsLineOperate.MissionLoadSuccess !== res2) {
                    toastShow(qsTr("加载任务文件失败"), 2000)
                }
            }
            else if (selVehMission) {
                var res = selVehMission.loadFlightLineFromFile(path)
                if (GcsLineOperate.MissionLoadSuccess !== res) {
                    toastShow(qsTr("加载任务文件失败"), 2000)
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("选择要加载的文件")
        folder: shortcuts.home
        nameFilters: [qsTr("文本文件 (*.txt)"), qsTr("所有文件 (*)")]
        onAccepted: {
            var res = flineDetailStage.loadStageFromFile(fileDialog.fileUrl)
            if (GcsLineOperate.TaskOperationSuccess !== res) {
                toastShow(qsTr("加载任务文件失败"), 2000)
            }
        }
    }

    Connections {
        target: selVehMission
        function onNotifyOperateResult(msg) {
            historyMessage = msg
        }
    }

    Component.onCompleted: {
        commandMgr.switchVehicel(0)
        currVehicle = commandMgr.currVehicle()
    }

    Component.onDestruction: {
        ;//
    }

    FmpHeaderPanel {
        id: toolbar
        height: headHeight
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        resetToolListener: (value) => {
           mainDisplay.resetLineAreaMap(value)
        }
    }

    FmpPlanDisplay {
        id: mainDisplay
        anchors.top: toolbar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }

    ControlBackground {
        id: mapOptItem
        width: headHeight
        height: headHeight
        radius: headHeight/2
        anchors.left: parent.left
        anchors.top: toolbar.bottom
        GcsText {
            anchors.centerIn: parent
            text: qsTr("地图")
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                mapselect.open()
            }
        }
    }

    Popup {
        id: mapselect
        y: toolbar.height
        x: headHeight
        width: 276
        height: 148
        padding: 1
        contentItem: Item{
            ControlBackground {
                id: imgw
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 10
                width: 128
                height: 128
                Image {
                    anchors.fill: parent
                    source: "qrc:///res/map-satellite.jpg"
                }
                MouseArea {
                    anchors.fill: parent
                    propagateComposedEvents: true
                    acceptedButtons: Qt.LeftButton
                    onClicked: {
                        configMgr.resetMapFactory(true)
                    }
                }
            }
            ControlBackground {
                id: vehw
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 10
                width: 128
                height: 128
                Image {
                    anchors.fill: parent
                    source: "qrc:///res/map-route.jpg"
                }
                MouseArea {
                    anchors.fill: parent
                    propagateComposedEvents: true
                    acceptedButtons: Qt.LeftButton
                    onClicked: {
                        configMgr.resetMapFactory(false)
                    }
                }
            }
        }
    }
}
