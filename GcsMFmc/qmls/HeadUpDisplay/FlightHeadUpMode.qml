import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    id: roots
    property var vehicleMode: commandMgr.currentVehicle.vehicleMode
    property int eachItemWidth: roots.width
    property int eachItemHigh: 30

    Column{
        anchors.margins: itemWidth/2
        anchors.top: parent.top
        anchors.right: parent.right
        Repeater {
            model: vehicleMode ? vehicleMode.dataModel: null
            GcsHorizText {
                property var showItem: object
                Connections {
                    target: showItem
                    function onUpdateColorValue(value, icolor) {
                        //eachItem.valueDefColor = txtColorMap[icolor]
                        eachItem.textValue = (icolor > 0xf) ? qsTr("--") : value
                    }
                }
                id: eachItem
                height: eachItemHigh
                width: eachItemWidth
                labelValue: showItem.itemName
            }
        }
    }
}
