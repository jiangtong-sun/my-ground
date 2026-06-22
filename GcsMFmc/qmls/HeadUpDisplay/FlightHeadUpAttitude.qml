import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

ControlBackground{
    property int hudDHeight: firstHeight/3
    property int hudUHeight: firstHeight/6
    property int itemWidth: (lrWidth-80)/3
    property var currVehicle: commandMgr.currentVehicle

    color: backgroundColor
    borderColor: GcsTheme.dividerColor
    borderWidth: 1

    Column {
        id: leftside
        width: lrWidth
        height: firstHeight
        anchors.left: parent.left

        Item {
            height: hudDHeight
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.rightMargin: itemWidth
            anchors.leftMargin: itemWidth

            ColumnLayout {
                anchors.fill: parent

                GcsHorizText {
                    width: itemWidth
                    labelValue: qsTr("")
                    textValue: qsTr("")
                }

                GcsHorizText {
                    width: itemWidth
                    labelValue: currVehicle.LateralMode.itemName
                    textValue: currVehicle.LateralMode.display
                }
                GcsHorizText {
                    width: itemWidth
                    labelValue: currVehicle.LongitudinalMode.itemName
                    textValue: currVehicle.LongitudinalMode.display
                }
                GcsHorizText {
                    width: itemWidth
                    labelValue: currVehicle.NavigationMode.itemName
                    textValue: currVehicle.NavigationMode.display
                }

                GcsHorizText {
                    width: itemWidth
                    labelValue: qsTr("")
                    textValue: qsTr("")
                }

                GcsHorizText {
                    width: itemWidth
                    labelValue: currVehicle.DeclineRatio.itemName
                    textValue: currVehicle.DeclineRatio.display
                }
            }
        }

        Item {
            height: hudUHeight
            width: lrWidth

            RowLayout{
                height: hudUHeight
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 30
                anchors.leftMargin: 30
                anchors.top: parent.top

                VertLineText {
                    height: 60
                    width: (lrWidth-40)/3
                    txtLabel: currVehicle.TrueAirspeed.itemName
                    txtValue: currVehicle.TrueAirspeed.display
                }

                VertLineText {
                    height: 60
                    width: (lrWidth-40)/3
                    txtLabel: currVehicle.IndicateSpeed.itemName
                    txtValue: currVehicle.IndicateSpeed.display
                }

                VertLineText {
                    height: 60
                    width: (lrWidth-40)/3
                    txtLabel: currVehicle.GroundSpeed.itemName
                    txtValue: currVehicle.GroundSpeed.display
                }
            }
        }

        Item {
            height: hudUHeight
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.leftMargin: 30
        }

        FlightAttitudeDisplay {
            height: hudDHeight
            width: lrWidth
        }
    }

    Column {
        id: rightside
        width: lrWidth
        height: firstHeight
        anchors.right: parent.right


        Item {
            height: hudDHeight
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.rightMargin: itemWidth
            anchors.leftMargin: itemWidth

            ColumnLayout{
                anchors.fill: parent

                GcsHorizText {
                    width: lrWidth-30
                    labelValue: qsTr("")
                    textValue: qsTr("")
                }

                GcsHorizText {
                    width: lrWidth-30
                    labelValue: currVehicle.PlaneAddress.itemName
                    textValue: currVehicle.PlaneAddress.display
                }

                GcsHorizText {
                    width: lrWidth-30
                    labelValue: currVehicle.DTG.itemName
                    textValue: currVehicle.DTG.display
                }

                GcsHorizText {
                    width: lrWidth-30
                    labelValue: qsTr("")
                    textValue: qsTr("")
                }

                GcsHorizText {
                    width: lrWidth-30
                    labelValue: currVehicle.WindSpeed.itemName
                    textValue: currVehicle.WindSpeed.display
                }

                GcsHorizText {
                    width: lrWidth-30
                    labelValue: currVehicle.WindDirection.itemName
                    textValue: currVehicle.WindDirection.display
                }
            }
        }

        Item {
            height: hudUHeight
            width: lrWidth
            RowLayout{
                height: hudUHeight
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 30
                anchors.leftMargin: 30
                anchors.top: parent.top

                VertLineText {
                    height: 60
                    width: itemWidth
                    txtLabel: currVehicle.Elevation.itemName
                    txtValue: currVehicle.Elevation.display
                }

                VertLineText {
                    height: 60
                    width: itemWidth
                    txtLabel: currVehicle.RadioAltimeter.itemName
                    txtValue: currVehicle.RadioAltimeter.display
                }

                VertLineText {
                    height: 60
                    width: itemWidth
                    txtLabel: currVehicle.QNE.itemName
                    txtValue: currVehicle.QNE.display
                }
            }
        }

        Item {
            height: hudUHeight
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.leftMargin: 30
        }

        FlightFlapDisplay {
            ctlHeight: hudDHeight
            ctlWidth: lrWidth
        }
    }

    FlightHUDCenterPanel{
        id: center
        anchors.left: leftside.right
        anchors.right: rightside.left
        height: firstHeight
    }
}


