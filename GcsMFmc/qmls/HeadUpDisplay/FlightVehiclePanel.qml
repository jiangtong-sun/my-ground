import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property var vehicle
    id: control

    Row {
        anchors.fill: parent
        GcsVertShow {
            textDefColor: leftRightColor
            labelDisplay: qsTr("编号")
            labelValue: vehicle.name
            width: control.width/3
            height: headHeight
            textType: true
        }
        GcsVertShow {
            textDefColor: leftRightColor
            labelDisplay: qsTr("告警")
            labelValue: vehicle.name
            width: control.width/3
            height: headHeight
            textType: false
            iconType: true
        }
        GcsVertShow {
            textDefColor: leftRightColor
            labelDisplay: qsTr("锁定")
            labelValue: vehicle.name
            width: control.width/3
            height: headHeight
            textType: false
            iconType: true
        }
    }
    GcsDivider{
        orientation: Qt.Vertical
        anchors.left: parent.left
        height: parent.height
        spacing: 0
    }
    GcsDivider{
        orientation: Qt.Vertical
        anchors.right: parent.right
        height: parent.height
        spacing: 0
    }
}
