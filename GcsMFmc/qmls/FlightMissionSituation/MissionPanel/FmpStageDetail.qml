import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import GcsLineStage 1.0
import GcsQuick 1.0

Item {
    property var stagePoints: flineStage? flineStage.visualPoints : null
    property int stagePointCount: stagePoints ? stagePoints.count:0
    property int panelWidth: optPanelWidth
    property bool takeoffPoint: false

    Component {
        id: stageWaypoint
        Item {
            property var wayObj: object
            function checkeStateFunc() {
                return (lastSelWaypoint === index) ? true : false
            }

            height: itemHeight
            width: panelWidth/4
            GcsToggleButton {
                anchors.top: parent.top
                anchors.fill: parent
                checkeState: checkeStateFunc()
                text: qsTr("航点: ") + (index + 1)
                onClicked: {
                    if (!wayObj.canDrag) {
                        return
                    }
                    if (currObjectItem === wayObj) {
                        currObjectItem = null
                        lastSelWaypoint = -1
                    } else {
                        if (currObjectItem) {
                            currObjectItem.editor = false
                        }

                        if ((index === 0) && (stageCode === GcsLineStage.Curise)){
                            takeoffPoint = true
                        }
                        else {
                            takeoffPoint = false
                        }

                        currObjectItem = wayObj
                        currObjectItem.editor = true
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
        model: stagePoints
        delegate: stageWaypoint
    }

    ScrollView{
        visible: !takeoffPoint
        anchors.right: parent.right
        anchors.left: waypointView.right
        anchors.leftMargin: 5
        width: panelWidth*2/3
        height: parent.height
        clip: true

        FmpWaypointEdit{
            stageLine: flineStage
            currWaypoint: currObjectItem
            waypointTypes: flineStage && stageCode === GcsLineStage.Curise ? flineStage.stagePointType: null
            removeWaypoint: function (point) {
                flineStage.removeWaypoint(point)
            }
        }
    }

    Item{
        visible: takeoffPoint
        anchors.right: parent.right
        anchors.left: waypointView.right
        anchors.leftMargin: 5
        width: panelWidth*2/3
        height: parent.height
        FmpTakeoffWaypointEdit{
            stageLine: flineStage
            currWaypoint: currObjectItem
        }
    }
}

