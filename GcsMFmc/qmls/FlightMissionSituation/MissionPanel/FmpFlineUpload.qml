import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Popup {
    id: switchpoint
    property var curStageLine: null
    property int waypointIdx: -1

    width: 150
    height: 36
    modal: true
    focus: true
    background: ControlBackground {
        anchors.fill: parent
        radius: 8
        GcsFilledButton {
            anchors.fill: parent
            text: qsTr("切换航点")
            onClicked: {
                if (selVehMission) {
                    selVehMission.switchWaypoint(curStageLine, waypointIdx)
                }
                switchpoint.close()
            }
        }
    }
}

