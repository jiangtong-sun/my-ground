import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import GcsLineStage 1.0
import GcsQuick 1.0

Item {
    property var iValidator: RegularExpressionValidator{regularExpression: /^[+-]?(\d+\.?\d*|\.\d+)$/}
    property int operateHeight: 40
    property int optBtnHeight: 30
    property int optBtnWidth: 70
    property int panelWidth: 400

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
            id: exportUpt
            btnRadius: 5
            width: optBtnWidth
            height: optBtnHeight
            anchors.right: parent.right
            anchors.rightMargin: 5
            text: flineStage&&flineStage.canRMWaypoint ? qsTr("保存") : qsTr("修改")
            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {
                    if (updateWaypoint) {
                        updateWaypoint = false
                        editorWaypoint = false
                    } else {
                        updateWaypoint = true
                        editorWaypoint = true
                    }
                }
            }
        }

        GcsToggleButton {
            id: resetBtn
            btnRadius: 5
            width: optBtnWidth
            height: optBtnHeight
            anchors.right: exportUpt.left
            anchors.rightMargin: 5
            text: qsTr("移除")
            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {
                    if (currFlineStage) {
                        lastSelWaypoint = -1
                        resetWaypointShow()
                        currFlineStage.removeStage(flineStage)
                    }
                }
            }
        }
    }

    Item {
        id: showDetail
        anchors.top: operate.bottom
        anchors.bottom: parent.bottom
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
                    text: qsTr("航点: ") + (index + 1)
                    onClicked: {
                        if (lastSelWaypoint === index) {
                            lastSelWaypoint = -1
                            resetWaypointShow()
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
            clip: true
            focus: true
            anchors.left: parent.left
            width: panelWidth/4
            height: parent.height
            ScrollBar.vertical: GcsScrollBar { }
            ScrollIndicator.vertical: GcsScrollIndicator {}
            boundsBehavior: Flickable.StopAtBounds
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
