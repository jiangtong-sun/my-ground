import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

ControlBackground {
    id: footer
    property int partHeight: 40
    property int maginTop: partHeight/8
    property int widthItem: parent.width/3
    property int heightItem: maginTop * 6
    property var currVehicle: commandMgr.currentVehicle

    property string showLocation: ""
    property var posStr: currVehicle ? currVehicle.position : ""
    property var taskStr: currVehicle ? currVehicle.name : ""
    property var attStr: currVehicle ? currVehicle.name : ""

    height: partHeight

    MouseArea {
        anchors.fill: parent
        propagateComposedEvents: true
    }

    Item{
        id: loc
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: maginTop
        width: widthItem
        height: heightItem

        GcsText {
            anchors.centerIn: parent
            //text: showLocation != "" ? showLocation : posStr
        }
    }

    Item{
        id: task
        anchors.right: parent.right
        anchors.top:  parent.top
        anchors.topMargin: maginTop

        width: widthItem
        height: heightItem

        GcsText {
            anchors.centerIn: parent
            text: taskStr
        }
    }

    Item{
        id: att
        anchors.left: loc.right
        anchors.right: task.left
        anchors.top: parent.top
        anchors.topMargin: maginTop

        width: widthItem
        height: heightItem

        GcsText {
            anchors.centerIn: parent
            text: attStr
        }
    }
}
