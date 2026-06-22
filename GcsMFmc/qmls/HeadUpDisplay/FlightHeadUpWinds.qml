import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    id: roots
    property var flightStates: commandMgr.currentVehicle.flightStates
    property int eachItemWidth: roots.width - itemWidth - itemWidth/2
    property int eachItemHigh: 30

    Column{
        anchors.leftMargin: itemWidth+itemWidth/2
        anchors.topMargin: itemWidth/2
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: parent.left

        Repeater {
            model: flightStates ? flightStates.dataModel: null
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
