import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    height: hudUHeight
    width: lrWidth
    property var elevation: currVehicle.Elevation
    property var radioAltimeter: currVehicle.RadioAltimeter
    property var qNE: currVehicle.QNE

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
            txtLabel: elevation.itemName
            txtValue: elevation.display
        }

        VertLineText {
            height: 60
            width: (lrWidth-40)/4
            txtLabel: radioAltimeter.itemName
            txtValue: radioAltimeter.display
        }

        VertLineText {
            height: 60
            width: (lrWidth-40)/4
            txtLabel: qNE.itemName
            txtValue: qNE.display
        }
    }
}
