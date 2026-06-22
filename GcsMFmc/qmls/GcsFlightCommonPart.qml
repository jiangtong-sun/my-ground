import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {

    property var modelPart: null
    property int navColumn: 3
    property int ctrlWidth: 400
    property int navItemHeight: 36
    property int panelHeight

    id: flight
    width: ctrlWidth
    height: panelHeight

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
        height: 38
        anchors {
            top: flight.top
            left: flight_left.right
            right: flight_right.left
            rightMargin: ctrlWidth/3
            leftMargin: ctrlWidth/3
        }

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: modelPart.mapGroup
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
                model: modelPart.dataModel
                GcsTextButton {
                    property var meta: object
                    property string nodeUnit: object.unit

                    id: oneElem2
                    width: navItemWidth
                    height: navItemHeight
                    labelValue: meta.itemName
                    btnFontSize: TextStyle.BodySmall
                    Component.onCompleted:  {
                        if (meta && meta.display && meta.itemName) {
                            oneElem2.textValue = meta.display + nodeUnit
                            oneElem2.textColor = txtColorMap[meta.showColor]
                        }
                    }

                    Connections {
                        target: meta
                        function onUpdateColorValue(value, color) {
                            oneElem2.textValue = (color>0xf) ? qsTr("--") + nodeUnit : value + nodeUnit;
                            oneElem2.textColor =  txtColorMap[color & 0xf]

                        }
                    }
                }
            }
        }
    }
}
