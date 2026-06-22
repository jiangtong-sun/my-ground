import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickSimpleElemNode 1.0
import QuickTemperature 1.0
import GcsQuick 1.0

Item {
    property int partWidth: 100
    property int partHeight: 100
    property int pitemHeight: partHeight / 2 - 10
    property int pitemWidth: partWidth / columnCount - 10
    property var temperMode: currVehicle ? currVehicle.temperatureMode : null
    property int temperCount: temperMode && (temperMode.count < 4)?4:temperMode.count
    property int columnCount: Math.ceil(temperCount/2)

    width: partWidth
    height: partHeight
    GroupRectangle {
        id: control
        width: partWidth
        height: partHeight
        anchors.fill: parent
        penColor: "#1e90ff" //GcsTheme.dividerColor
        textColor: leftRightColor
        text: qsTr("温度")
        Grid{
            anchors.fill: parent
            anchors.margins: 10
            columns: columnCount
            Repeater{
                model: temperMode ? temperMode.dataModel: null
                delegate: QuickTemperature {
                    color:GcsTheme.green
                    fontColor: GcsTheme.primaryColor
                    warnColor: GcsTheme.orange
                    width: pitemWidth
                    height: pitemHeight
                    dataNode: object
                }
            }
        }
    }
}
