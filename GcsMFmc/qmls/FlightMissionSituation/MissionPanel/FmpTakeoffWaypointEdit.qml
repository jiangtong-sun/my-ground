import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item{
    property var currWaypoint: null
    property var stageLine: null
    property int rowItemHeight: 30

    RowLayout{
        id: detail
        height: rowItemHeight
        width: panelWidth*2/3
        GcsText {
            text: qsTr("【航点详情】")
        }
    }

    Column {
        anchors.fill: parent
        anchors.rightMargin: 20
        anchors.topMargin: rowItemHeight+10
        visible: currWaypoint? true: false
        spacing: 5
        RowLayout {
            GcsText {
                Layout.minimumWidth: panelWidth/3
                Layout.minimumHeight: rowItemHeight
                Layout.alignment: Qt.AlignRight
                horizontalAlignment: Text.AlignRight
                text: qsTr("经度（°）:")
            }
            WaypointEditing{
                id: lngInput
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3
                Layout.minimumHeight: rowItemHeight
                text: currWaypoint ? currWaypoint.lng : 0
                editorPoint: !editorWaypoint
                editFinishListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.lng = Number(lngInput.text)
                    }
                }
                commitListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.lng = Number(lngInput.text)
                    }
                }
            }
        }

        RowLayout {
            GcsText {
                Layout.minimumWidth: panelWidth/3
                Layout.minimumHeight: rowItemHeight
                Layout.alignment: Qt.AlignLeft
                horizontalAlignment: Text.AlignRight
                text: qsTr("纬度（°）:")
            }
            WaypointEditing{
                id: latInput
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3
                Layout.minimumHeight: rowItemHeight
                text: currWaypoint ? currWaypoint.lat : 0
                editorPoint: !editorWaypoint
                editFinishListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.lat = Number(latInput.text)
                    }
                }
                commitListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.lat = Number(latInput.text)
                    }
                }
            }
        }

        RowLayout {
            GcsText {
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3
                Layout.minimumHeight: rowItemHeight
                horizontalAlignment: Text.AlignRight
                text: qsTr("海拔高度（m）:")
            }
            WaypointEditing{
                id: altInput
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3
                Layout.minimumHeight: rowItemHeight
                text: currWaypoint ? currWaypoint.alt : 0
                editorPoint: !editorWaypoint
                editFinishListener: () => {
                    if (currWaypoint) {
                        currWaypoint.alt = Number(altInput.text)
                    }
                }
                commitListener: () => {
                    if (currWaypoint) {
                        currWaypoint.alt = Number(altInput.text)
                    }
                }
            }
        }

        RowLayout {
            GcsText {
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3
                Layout.minimumHeight: rowItemHeight
                horizontalAlignment: Text.AlignRight
                text: qsTr("航向（°）:")
            }
            WaypointEditing{
                id: yawInput
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3
                Layout.minimumHeight: rowItemHeight
                text: currWaypoint ? currWaypoint.yawAngle : 0
                editorPoint: !editorWaypoint
                editFinishListener: () => {
                    if (currWaypoint) {
                        currWaypoint.yawAngle = Number(yawInput.text)
                    }
                }
                commitListener: () => {
                    if (currWaypoint) {
                        currWaypoint.yawAngle = Number(yawInput.text)
                    }
                }
            }
        }

        RowLayout {
            GcsText {
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3
                Layout.minimumHeight: rowItemHeight
                horizontalAlignment: Text.AlignRight
                text: qsTr("相对高度（°）:")
            }
            WaypointEditing{
                id: tAltInput
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3
                Layout.minimumHeight: rowItemHeight
                text: currWaypoint ? currWaypoint.relativeHight : 0
                editorPoint: !editorWaypoint
                editFinishListener: () => {
                    if (currWaypoint) {
                        currWaypoint.relativeHight = Number(tAltInput.text)
                    }
                }
                commitListener: () => {
                    if (currWaypoint) {
                        currWaypoint.relativeHight = Number(tAltInput.text)
                    }
                }
            }
        }

        RowLayout {
            GcsText {
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3
                Layout.minimumHeight: rowItemHeight
                horizontalAlignment: Text.AlignRight
                text: qsTr("跑道长（m）:")
            }
            WaypointEditing{
                id: backControl
                Layout.alignment: Qt.AlignLeft
                Layout.minimumWidth: panelWidth/3
                Layout.minimumHeight: rowItemHeight
                editorPoint: !editorWaypoint
                text: currWaypoint ? currWaypoint.runwayLen : 0
                editFinishListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.runwayLen = Number(backControl.text)
                    }
                }
                commitListener: () =>{
                    if (currWaypoint) {
                        currWaypoint.runwayLen = Number(backControl.text)
                    }
                }
            }
        }
    }
}
