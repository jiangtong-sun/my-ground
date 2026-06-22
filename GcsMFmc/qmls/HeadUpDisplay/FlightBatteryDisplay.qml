import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickBatteryElemNode 1.0
import GcsQuick 1.0

Item {
    property int partWidth: 100
    property int partHeight: 100
    property int dPartHeight: partHeight-120
    property int dOtherHeight: partHeight - dPartHeight
    property var batteryMode: commandMgr.currentVehicle.batteryMode
    property var batteryDataMode: batteryMode ? batteryMode.dataModel: null

    width: partWidth
    height: partHeight

    GroupRectangle {
        id: control
        width: partWidth
        height: partHeight
        anchors.fill: parent
        penColor: "#1e90ff"
        textColor: leftRightColor
        text: qsTr("电池")

        Item {
            id: other
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.topMargin: 28
            width: partWidth - 40
            height: 70

            Column {
                anchors.fill: parent
                anchors.bottomMargin: 20
                spacing: 1
                Repeater {
                    model: batteryDataMode
                    Row {
                        property var batteryObj: object
                        property var aSetColor: batteryObj.valtage.showColor
                        property string avaltageStr: "" + batteryObj.valtage.unit
                        property string acurrentStr: "" + batteryObj.current.unit
                        property string acapacityStr: "" + batteryObj.capacity.unit
                        Connections{
                            target: batteryObj.valtage
                            function onDisplayValueChanged() {
                                avaltageStr = batteryObj.valtage.display + batteryObj.valtage.unit
                                aSetColor = batteryObj.valtage.showColor
                            }
                        }

                        Connections{
                            target: batteryObj.current
                            function onDisplayValueChanged() {
                                acurrentStr = batteryObj.current.display + batteryObj.current.unit
                            }
                        }

                        Connections{
                            target: batteryObj.capacity
                            function onDisplayValueChanged() {
                                acapacityStr = batteryObj.capacity.display + batteryObj.capacity.unit
                            }
                        }

                        GcsText {
                            height: 30
                            width: (partWidth - 40)/3
                            text: batteryObj.groupName + qsTr(":")
                            color: GcsTheme.primaryColor
                        }
                        GcsText {
                            height: 30
                            width: (partWidth - 40)/5
                            text: avaltageStr
                            color: aSetColor? GcsTheme.orange : GcsTheme.fontPrimaryColor
                        }
                        GcsText {
                            height: 30
                            width: (partWidth - 40)/5
                            text: acurrentStr
                            color: aSetColor? GcsTheme.orange : GcsTheme.fontPrimaryColor
                        }
                        GcsText {
                            height: 30
                            width: (partWidth - 40)/5
                            text: acapacityStr
                            color: aSetColor? GcsTheme.orange : GcsTheme.fontPrimaryColor
                        }
                    }
                }
            }
        }

        // Item {
        //     anchors.bottom: parent.bottom
        //     anchors.left: parent.left
        //     anchors.leftMargin: 20
        //     anchors.bottomMargin: 20
        //     height: dPartHeight
        //     width: partWidth - 40
        //     RowLayout {
        //         anchors.fill: parent
        //         anchors.bottomMargin: 20
        //         spacing: 1
        //         Repeater {
        //             model: batteryDataMode
        //             QuickBatteryElemNode{
        //                 fontColor: GcsTheme.primaryColor
        //                 color: drawLineColor
        //                 width: parent.width/batteryDataMode.count
        //                 height: dPartHeight
        //                 dataNode: object
        //             }
        //         }
        //     }
        // }
    }
}
