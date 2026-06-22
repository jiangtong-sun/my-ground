import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item{
    id: _root
    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property int navItemHeight: 36
    property int navColumn: projectEnv === 2 ? 2 : 3
    property int navItemWidth: (_root.width - 20)/navColumn
    property var firstModePart:  currVehicle ? currVehicle.mainNavBlock: null
    property var secondModePart:  currVehicle ? currVehicle.mainCommBlock: null
    property int firstRowCounts: firstModePart?firstModePart.dataModel.count: 0
    property int secondRowCounts: secondModePart?secondModePart.dataModel.count: 0
    property int navPanelHeight: (navItemHeight+1) * (Math.ceil(firstRowCounts/navColumn)+1)
    property int othPanelHeight: ctrlHeight - navPanelHeight

    property string qneqnhqfeUnit: currVehicle? currVehicle.QNE.unit: qsTr("m")
    property string qneCurrValue: qsTr("")
    property string qnhCurrValue: qsTr("")
    property string qfeCurrValue: qsTr("")

    // Connections{
    //     target: currVehicle?currVehicle.QNE: null
    //     function onUpdateColorValue(value, color) {
    //         qneCurrValue = value
    //     }
    // }

    // Connections{
    //     target: currVehicle?currVehicle.QNH: null
    //     function onUpdateColorValue(value, color) {
    //         qnhCurrValue = value
    //     }
    // }

    // Connections{
    //     target: currVehicle?currVehicle.QFE: null
    //     function onUpdateColorValue(value, color) {
    //         qfeCurrValue = value
    //     }
    // }

    width: ctrlWidth
    height: ctrlHeight

    Item {
        id: flight
        width: ctrlWidth
        height: navPanelHeight
        anchors.top: parent.top

        GcsDivider{
            id: flight_left
            orientation: Qt.Horizontal
            anchors.top: flight.top
            anchors.left: flight.left
            anchors.leftMargin: 10
            width: ctrlWidth/2-80
            contrast: true
            spacing: 20
        }

        GcsDivider{
            id: flight_right
            orientation: Qt.Horizontal
            anchors.top: flight.top
            anchors.right: flight.right
            anchors.rightMargin: 10
            width: ctrlWidth/2-80
            contrast: true
            spacing: 20
        }

        GcsText{
            height: 40
            anchors {
                top: flight.top
                left: flight_left.right
                right: flight_right.left
                rightMargin: ctrlWidth/3
                leftMargin: ctrlWidth/3
            }

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: firstModePart.mapGroup
        }

        Item {
            id: nav_view
            anchors.fill: parent
            anchors.margins: 10
            anchors.topMargin: 36

            Grid{
                anchors.fill: parent
                columns: navColumn
                spacing: 1
                Repeater {
                    model: firstModePart.dataModel
                    GcsTextButton {
                        property var metaObj: object
                        property string nodeUnit: metaObj.unit
                        property string formatStr: metaObj.formatStr

                        id: oneElem
                        width: navItemWidth
                        height: navItemHeight
                        labelValue: metaObj.itemName
                        btnFontSize: TextStyle.BodySmall

                        Component.onCompleted:  {
                            if (metaObj && metaObj.display && metaObj.itemName) {
                                oneElem.textValue = (metaObj.showColor>0xf) ? qsTr("--")  + nodeUnit:  metaObj.display + nodeUnit
                                oneElem.textColor = txtColorMap[metaObj.showColor & 0xf]
                            }
                        }

                        Connections {
                            target: metaObj
                            function onUpdateColorValueExt(value, color) {
                                oneElem.textValue = (color>0xf) ? qsTr("--")  + nodeUnit : value  + nodeUnit;
                                oneElem.textColor =  txtColorMap[color & 0xf]
                            }
                            function onUpdateColorValue(value, color) {
                                oneElem.textValue = (color>0xf) ? qsTr("--")  + nodeUnit : value  + nodeUnit;
                                oneElem.textColor =  txtColorMap[color & 0xf]
                            }
                        }
                    }
                }
            }
        }
    }

    Item {
        id: navg
        width: ctrlWidth
        height: othPanelHeight
        anchors.top: flight.bottom
        visible: secondRowCounts>0

        GcsDivider{
            id: navg_left
            orientation: Qt.Horizontal
            anchors.top: navg.top
            anchors.left: navg.left
            anchors.leftMargin: 10
            width: ctrlWidth/2-80
            contrast: true
            spacing: 20
        }

        GcsDivider{
            id: navg_right
            orientation: Qt.Horizontal
            anchors.top: navg.top
            anchors.right: navg.right
            anchors.rightMargin: 10
            width: ctrlWidth/2-80
            contrast: true
            spacing: 20
        }

        GcsText {
            height: 40
            anchors{
                top: navg.top
                left: navg_left.right
                right: navg_right.left
                rightMargin: ctrlWidth/3
                leftMargin: ctrlWidth/3
            }
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: secondModePart.mapGroup
        }

        Item {
            id: otherview
            anchors.fill: parent
            anchors.margins: 10
            anchors.topMargin: 36
            Grid{
                anchors.fill: parent
                columns: navColumn
                spacing: 1
                Repeater {
                    model: secondModePart.dataModel
                    GcsTextButton {
                        property var meta: object
                        property string nodeUnit: object.unit
                        id: oneElem2
                        width: navItemWidth
                        height: navItemHeight
                        labelValue: meta.itemName
                        textValue: meta.display
                        btnFontSize: TextStyle.BodySmall
                        Component.onCompleted:  {
                            if (meta && meta.display && meta.itemName) {
                                oneElem2.textValue = (meta.showColor>0xf) ? qsTr("--") + nodeUnit : meta.display+ nodeUnit
                                oneElem2.textColor = txtColorMap[meta.showColor & 0xf]
                            }
                        }

                        Connections {
                            target: meta
                            function onUpdateColorValue(value, color) {
                                oneElem2.textValue = (color>0xf) ? qsTr("--") + nodeUnit: value+ nodeUnit;
                                oneElem2.textColor =  txtColorMap[color & 0xf]
                            }
                        }
                    }
                }
            }
        }
    }
}
