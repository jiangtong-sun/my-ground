import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property int panelOffy: projectEnv === 2 ? 50: 0

    id: root
    Column {
        id: leftside
        width: lrWidth
        height: firstHeight
        anchors.fill: parent
        anchors.topMargin: 5

        FlightMissionDetail {
            partWidth: lrWidth
            partHeight: hudDHeight-panelOffy
        }

        FlightAttitudeDetail {
            partWidth: lrWidth
            partHeight: hudDHeight+panelOffy
        }

        FlightElectricalDisplay {
            partWidth: lrWidth
            partHeight: hudDHeight
        }

        FlightFuelDisplay {
            partWidth: lrWidth
            partHeight: hudDHeight
        }
    }
}
