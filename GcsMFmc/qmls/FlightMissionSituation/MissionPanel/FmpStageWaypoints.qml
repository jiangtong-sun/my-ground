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
    property bool isTakeoffPoint: false

    Component {
        id: stageWaypoint
        Row {
            property var wayObj: object
            function checkeStateFunc() {
                return (lastSelWaypoint == index) ? true : false
            }

            GcsButton {
                height: itemHeight
                width: panelWidth/10
                text: index+1 //wayObj.seq
                onClicked: {
                    if (currObjectItem === wayObj) {
                        currObjectItem = null
                        lastSelWaypoint = -1
                    } else {
                        if ((index == 0) && (stageCode === GcsLineStage.Curise)){
                            isTakeoffPoint = true
                        } else {
                            isTakeoffPoint = false
                        }
                        currObjectItem = wayObj
                        lastSelWaypoint = index
                    }
                }
            }

            GcsButton {
                height: itemHeight
                width: panelWidth/3-10
                text: wayObj.lngStr
            }

            GcsButton {
                height: itemHeight
                width: panelWidth/3-10
                text: wayObj.latStr
            }

            GcsButton {
                height: itemHeight
                width: panelWidth/6+10
                text: wayObj.altStr
            }
            GcsButton {
                width: 32
                height: itemHeight
                text: wayObj.pointType
            }
        }
    }

    ListView{
        id: waypointView
        anchors.left: parent.left
        width: panelWidth
        height: parent.height
        ScrollBar.vertical: GcsScrollBar { }
        ScrollIndicator.vertical: GcsScrollIndicator {}
        boundsBehavior: Flickable.StopAtBounds
        clip: true
        focus: true
        model: stagePoints
        delegate: stageWaypoint
    }
}

