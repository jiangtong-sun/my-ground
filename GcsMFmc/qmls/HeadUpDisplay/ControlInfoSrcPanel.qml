import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickTemperature 1.0
import GcsQuick 1.0

Item {
    property int partWidth: 100
    property int partHeight: 100
    property int pitemHeight: partHeight / 2 - 10
    property int pitemWidth: partWidth / 3 - 10
    property var attiLeftShow: currVehicle ? currVehicle.attiLeftShow: null
    property var fuelTankMode: commandMgr.currentVehicle.fuelTankMode
    property var fuelDataMode: fuelTankMode ? fuelTankMode.dataModel: null

    width: partWidth
    height: partHeight

    GroupRectangle {
        id: control
        width: partWidth
        height: partHeight
        anchors.fill: parent
        penColor: "#1e90ff" //GcsTheme.dividerColor
        textColor: leftRightColor
        text: qsTr("燃油")
    }

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
                model: attiLeftShow?attiLeftShow.dataModel: null
                delegate: GcsHorizImage {
                    property var displayObj: object
                    id: horizImg
                    height: 36
                    width: partWidth - 40
                    valueUnit: object.unit
                    showImage: object.showMode
                    textDefColor:  GcsTheme.primaryColor
                    labelValue: displayObj.itemName
                    //textValue: displayObj.display
                    labelAlign: Text.AlignLeft

                    Connections {
                        target: displayObj
                        function onUpdateColorValue(value, color) {
                            if((color&0xf === 1) || (color&0xf === 2)) {
                                horizImg.drawingColor = GcsTheme.orange
                            } else {
                                horizImg.drawingColor = GcsTheme.green
                            }
                            horizImg.textValue = (color>0xf) ? qsTr("--") : value
                            horizImg.activeRate = displayObj.rateValue
                            horizImg.drawImage()
                        }
                    }
                }
            }
        }
    }
}
