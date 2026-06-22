import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property int partWidth: 100
    property int partHeight: 100
    property var missionMode: commandMgr.currentVehicle.missionMode
    property var itemCounts: missionMode ? missionMode.dataModel.count: 1
    property int eachHeight: (partHeight - 54)/itemCounts

    width: partWidth
    height: partHeight

    GroupRectangle {
        id: control
        width: partWidth
        height: partHeight
        anchors.fill: parent
        penColor: "#1e90ff" //GcsTheme.dividerColor
        text: missionMode ? missionMode.mapGroup : qsTr("飞行任务")
        textColor: leftRightColor

        Item {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.topMargin: 28
            height: partHeight - 40
            width: partWidth - 40
            Column {
                anchors.fill: parent
                anchors.bottomMargin: 20
                Repeater {
                    model: missionMode ? missionMode.dataModel: null
                    GcsHorizText {
                        property var showItem: object
                        Connections {
                            target: showItem
                            function onUpdateColorValue(value, icolor) {
                                eachItem.textValue = (icolor > 0xf) ? qsTr("--") : value
                                eachItem.valueDefColor = txtColorMap[icolor&0xf]
                            }
                        }
                        id: eachItem
                        height: eachHeight
                        width: partWidth - 40
                        textDefColor: GcsTheme.primaryColor
                        labelValue: showItem.itemName
                        //textValue: object.display
                        labelAlign: Text.AlignLeft
                        valueAlign: Text.AlignLeft
                    }
                }
            }
        }
    }
}
