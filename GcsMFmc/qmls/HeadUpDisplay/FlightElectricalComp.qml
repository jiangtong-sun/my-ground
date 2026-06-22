import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickBusbarElemNode 1.0
import QuickHBusbarElemNode 1.0
import GcsQuick 1.0

Component {
    property var inputParam: null
    property var eimcMode: commandMgr.currentVehicle.eimcMode
    property var batteryMode: commandMgr.currentVehicle.batteryMode

    GroupRectangle {
        id: control
        width: partWidth
        height: partHeight
        anchors.fill: parent
        penColor: "#1e90ff" //GcsTheme.dividerColor
        textColor: leftRightColor
        text: qsTr("EMIC & 电池")

        Item {
            id: other
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.topMargin: 28
            anchors.bottomMargin: 20
            height: partHeight - 50
            width: partWidth - 50
            GridLayout{
                rows: 2
                columns: 2
                anchors.fill: parent
                Repeater {
                    model: eimcMode?eimcMode.dataModel:null
                    delegate: Item {
                        property var emicData: object
                        Layout.alignment: Qt.AlignHCenter
                        width: itemWidth
                        height: itemHeight
                        Component{
                            id: xfBusbar
                            QuickBusbarElemNode{
                                id: xfBusbarNode
                                anchors.fill: parent
                                warnColor: GcsTheme.orange
                                fontColor: GcsTheme.primaryColor
                                whiteColor: GcsTheme.White
                                color: GcsTheme.green
                                dataNode: emicData
                            }
                        }
                        Component{
                            id: whxqBusbar
                            QuickHBusbarElemNode{
                                id: whxqBusbarNode
                                anchors.fill: parent
                                warnColor: GcsTheme.orange
                                fontColor: GcsTheme.primaryColor
                                whiteColor: GcsTheme.White
                                color: GcsTheme.green
                                dataNode: emicData
                            }
                        }
                        Component{
                            id: c21Busbar
                            QuickBusbarElemNode {
                                id: c21BusbarNode
                                anchors.fill: parent
                                warnColor: GcsTheme.orange
                                fontColor: GcsTheme.primaryColor
                                whiteColor: GcsTheme.White
                                color: GcsTheme.green
                                dataNode: emicData
                            }
                        }

                        GcsLoader{
                            anchors.fill: parent
                            sourceComponent: {
                                inputParam = emicData
                                if (projectEnv === 1) {
                                    return c21Busbar;
                                }
                                if (projectEnv === 2) {
                                    return xfBusbar;
                                }
                                if (projectEnv === 3) {
                                    return whxqBusbar;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

