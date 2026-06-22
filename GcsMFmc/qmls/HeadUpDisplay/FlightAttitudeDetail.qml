import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickBusbarElemNode 1.0
import GcsQuick 1.0

Item {
    property int partWidth: 100
    property int partHeight: 100
    property int itemWidth: (partWidth - 50)/9
    property int itemHeight: 28
    property var currVehicle: commandMgr.currentVehicle
    property var fmodelStates: currVehicle ? currVehicle.fmodelStates : null
    property var navigationState: currVehicle ? currVehicle.navigationState: null
    property var fmodelCount: fmodelStates ? fmodelStates.dataModel.count: 1
    property var navStateCount: navigationState ? navigationState.dataModel.count: 1

    width: partWidth
    height: partHeight

    Component{
        id: altitudeComp
        GroupRectangle {
            id: control
            anchors.fill: parent
            penColor: "#1e90ff"
            textColor: leftRightColor
            text: qsTr("姿态")
            Item {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.topMargin: 20
                height: partHeight - 40
                width: partWidth - 40

                Row {
                    height: itemHeight
                    width: parent.width
                    GcsText {
                        height: itemHeight
                        width: itemWidth*3
                        text: qsTr("")
                    }
                    GcsText {
                        height: itemHeight
                        width: itemWidth*2
                        textColor: GcsTheme.primaryColor
                        text: qsTr("主惯")
                    }
                    GcsText {
                        height: itemHeight
                        width: itemWidth*2
                        textColor: GcsTheme.primaryColor
                        text: qsTr("备惯")
                    }
                }

                Column {
                    anchors.topMargin: itemHeight
                    anchors.fill: parent
                    Repeater {
                        model: navigationState ? navigationState.dataModel: null
                        delegate: Row {
                            height: itemHeight
                            property var showItem: object
                            property var showName: showItem.name
                            property var showMain: showItem.main
                            property var showSlave: showItem.slave
                            Connections {
                                target: showMain
                                function onUpdateColorValue(value, icolor) {
                                    mainItem.text = (icolor > 0xf) ? qsTr("--") : value;
                                    //slaveItem.textColor = txtColorMap[icolor]
                                }
                            }
                            Connections {
                                target: showSlave
                                function onUpdateColorValue(value, icolor) {
                                    slaveItem.text = (icolor > 0xf) ? qsTr("--") : value;
                                    //slaveItem.textColor = txtColorMap[icolor]
                                }
                            }
                            GcsText {
                                height: itemHeight
                                width: itemWidth*3
                                textColor: GcsTheme.primaryColor
                                text: showName.itemName
                            }
                            GcsText {
                                id: mainItem
                                height: itemHeight
                                width: itemWidth*2
                                textColor: GcsTheme.fontPrimaryColor
                                text: showMain.display
                            }
                            GcsText {
                                id: slaveItem
                                height: itemHeight
                                width: itemWidth*2
                                textColor: GcsTheme.fontPrimaryColor
                                text: showMain.display
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: modestateComp
        GroupRectangle {
            id: control
            width: partWidth
            height: partHeight
            anchors.fill: parent
            penColor: "#1e90ff"
            text: fmodelStates ? fmodelStates.mapGroup : qsTr("模态/状态")
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
                        model: fmodelStates ? fmodelStates.dataModel: null
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
                            height: itemHeight
                            width: partWidth - 40
                            textDefColor: GcsTheme.primaryColor
                            labelValue: showItem.itemName
                            labelAlign: Text.AlignLeft
                            valueAlign: Text.AlignLeft
                        }
                    }
                }
            }
        }
    }

    Item{
        anchors.fill: parent
        GcsLoader{
            anchors.fill: parent
            sourceComponent: {
                if(projectEnv === 1) {
                    itemHeight = (parent.height-40)/(navStateCount+1)
                    return altitudeComp;
                }
                if (projectEnv === 2) {
                    itemHeight = (parent.height-40)/fmodelCount
                    return modestateComp;
                }
                if (projectEnv === 3) {
                    itemHeight = (parent.height-40)/(navStateCount+1)
                    return altitudeComp;
                }
            }
        }
    }
}

