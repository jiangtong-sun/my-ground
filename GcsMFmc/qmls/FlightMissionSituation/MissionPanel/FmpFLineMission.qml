import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import GcsLineStage 1.0
import GcsQuick 1.0

Item {
    property var iValidator: RegularExpressionValidator{regularExpression: /^[0-9.-][0-9.]{4}$/}
    property int operateHeight: 50
    property int optBtnHeight: 30
    property int optBtnWidth: 70
    property int panelWidth: 400

     //选择车辆 并且 点击 "临时航线”
    property bool templineOpt: (flineStage && fStageCode === GcsLineStage.Templine)?true:false
    function resetWaypointShow( ) {
        normalpoint.currWaypoint = null
    }

    function showWaypoint( value ) {
        normalpoint.currWaypoint = value
    }

    Item {
        id: operate
        width: panelWidth
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.right: parent.right
        height: operateHeight

        GcsToggleButton {
            id: saveUpt
            btnRadius: 5
            width: optBtnWidth
            height: optBtnHeight
            anchors.right: parent.right
            anchors.rightMargin: 5
            visible: templineOpt? true: false
            checkeState: updateWaypoint //? GcsTheme.red: GcsTheme.itemNormalColor
            text: updateWaypoint ? qsTr("保存") : qsTr("修改")
            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {
                    if (flineStage && updateWaypoint) {
                        flineStage.saveAs(selVehMission.vehicleIdx)
                        updateWaypoint = false
                    } else if(flineStage){
                        updateWaypoint = true
                    }
                }
            }
        }
        GcsToggleButton {
            id: resetBtn
            btnRadius: 5
            width: optBtnWidth
            height: optBtnHeight
            anchors.right: saveUpt.left
            anchors.rightMargin: 5
            text: qsTr("重置")
            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {
                    if (flineStage && updateWaypoint) {
                        flineStage.removeDisplayResource()
                    }
                }
            }
        }
        GcsToggleButton {
            id: uploadCtl
            btnRadius: 5
            width: optBtnWidth
            height: optBtnHeight
            anchors.right: resetBtn.left
            anchors.rightMargin: 5
            text: qsTr("上传")
            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {

                }
            }
        }

        Row{
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: uploadCtl.left
            height: optBtnHeight
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            GcsCheckBox {
                id: syncHigh
                width: 90
                height: optBtnHeight
                iconSourceNormal: ttfIcons["screenShrink"]
                iconSourceAccept: ttfIcons["screenExpand"]
                text: "同步高度"
            }
            ControlBackground{
                width: 60
                height: optBtnHeight
                visible: syncHigh.checked
                GcsTextBox{
                    validator: iValidator
                    anchors.fill: parent
                }
            }
        }
    }

    Item {
        id: showDetail
        anchors.top: parent.top
        anchors.topMargin: operateHeight
        height: parent.height - operate.height
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
                    height: itemHeight
                    width: panelWidth/4
                    checkeState: checkeStateFunc()
                    text: qsTr("航点: ") + index
                    onClicked: {
                        if (lastSelWaypoint === index) {
                            lastSelWaypoint = -1
                            resetWaypointShow(null)
                        } else {
                            showWaypoint(wayObj)
                            lastSelWaypoint = index
                        }
                    }
                }
            }
        }

        ListView{
            id: waypointView
            anchors.left: parent.left
            width: panelWidth/4
            height: parent.height
            ScrollBar.vertical: GcsScrollBar { }
            ScrollIndicator.vertical: GcsScrollIndicator {}
            boundsBehavior: Flickable.StopAtBounds
            clip: true
            focus: true
            model: flineStage ? flineStage.visualPoints : null
            delegate: flineWaypoint
        }

        ScrollView{
            id: normalArea
            anchors.right: parent.right
            anchors.left: waypointView.right
            anchors.leftMargin: 5
            height: parent.height
            clip: true
            FmpWaypointEdit{
                id: normalpoint
                stageLine: flineStage
                waypointTypes: null
            }
        }
    }
}
