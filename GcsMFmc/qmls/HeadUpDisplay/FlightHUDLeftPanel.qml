import QtQuick  2.15
import QtQuick.Window  2.15
import QtQuick.Controls  2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    id: root

    property var engineMode: commandMgr.currentVehicle.engineMode
    property var firstNode: engineMode ? engineMode.firstNode: null
    property var secondNode: engineMode ? engineMode.secondNode: null

    Column {
        id: leftside
        width: lrWidth
        height: firstHeight
        anchors.fill: parent
        anchors.topMargin: 5

        FlightDisplayWarn {
            partWidth: lrWidth
            partHeight: hudDHeight
        }

        FlightDisplayHistory {
            partWidth: lrWidth
            partHeight: hudDHeight
        }

        ControlAdjustPanel {
            partWidth: lrWidth
            partHeight: projectEnv === 2 ? hudDHeight*2: hudDHeight+30
            engineData: firstNode
        }

        ControlInfoSrcPanel{
            partWidth: lrWidth
            partHeight: projectEnv === 2 ? 1 : hudDHeight-30
        }
    }
}
