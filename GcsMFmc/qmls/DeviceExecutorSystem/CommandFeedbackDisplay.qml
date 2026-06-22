import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickDataLinkElem 1.0
import QuickActuatorElem 1.0
import QuickExecutorElem 1.0
import QuickEngineElem 1.0
import ChannelSelection 1.0
import GcsQuick 1.0


Item{
    id: cmdstate

    // property var navigationMode: currVehicle ? currVehicle.NavigationMode: null
    // property var navItemName: navigationMode ? navigationMode.itemName : ""
    // property var navItemDisplay: navigationMode ? navigationMode.display : ""
    property color cmdFontColor: GcsTheme.green
    property string commandRequst: ""
    property string commandResponse: ""

    Connections {
        target:vehfeedback
        function onCurrCommandChanged(request, response, fontcolor) {
            cmdFontColor = fontcolor === 1 ? GcsTheme.green : GcsTheme.red
            commandResponse = response
            commandRequst = request
        }
    }

    Column{
        anchors.fill: parent
        Grid {
            columns: 2
            height: 100
            width: parent.width
            GcsText {
                height: 40
                width: 80
                text: qsTr("指令发送")
                textColor: GcsTheme.fontPrimaryColor
            }
            GcsText {
                height: 40
                width: parent.width-80
                defFontSize: TextStyle.BodySmall
                textColor: cmdFontColor
                text: commandRequst
            }
            GcsText {
                height: 40
                width: 80
                text: qsTr("指令接受")
                textColor: GcsTheme.fontPrimaryColor
            }
            GcsText {
                height: 40
                width: parent.width-80
                defFontSize: TextStyle.BodySmall
                textColor: cmdFontColor
                text: commandResponse
            }
        }
    }
}
