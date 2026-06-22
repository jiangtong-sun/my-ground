import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property var trueAirspeed: currVehicle.TrueAirspeed
    property var indicateSpeed: currVehicle.IndicateSpeed
    property var groundSpeed: currVehicle.GroundSpeed

    RowLayout{
        height: hudUHeight
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.rightMargin: 30
        anchors.leftMargin: 30
        anchors.top: parent.top

        VertLineText {
            height: 60
            width: (lrWidth-40)/4
            txtLabel: trueAirspeed.itemName
            txtValue: trueAirspeed.display
        }

        VertLineText {
            height: 60
            width: (lrWidth-40)/4
            txtLabel: indicateSpeed.itemName
            txtValue: indicateSpeed.display
        }

        VertLineText {
            height: 60
            width: (lrWidth-40)/4
            txtLabel: groundSpeed.itemName
            txtValue: groundSpeed.display
        }
    }
}
