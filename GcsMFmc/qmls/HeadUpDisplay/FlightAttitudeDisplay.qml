import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    property int cwidth: leftWidth
    property int cheight: hudDHeight   
    property var currVehicle: commandMgr.currentVehicle
    property var attitudeDevice: currVehicle ? currVehicle.attitudeModel : null

    id: control
    width: cwidth
    height: cheight

    Item {
        anchors.fill: parent
        anchors.margins: 10
        anchors.topMargin: 40

        GcsTableView {
            id: attitudeview
            showHeader: true
            anchors.fill: parent
            tableModel: attitudeDevice
            dataSource: attitudeDevice.rowSource
            columnSource: attitudeDevice.columnSource
            columnWidthProvider: (colu) => {
                return 80
            }
        }
    }
}
