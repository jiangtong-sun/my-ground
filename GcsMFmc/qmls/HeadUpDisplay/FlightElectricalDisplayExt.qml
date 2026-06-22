import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickBusbarElemNode 1.0
import QuickHBusbarElemNode 1.0
import GcsQuick 1.0

Item {
    property int partWidth: 100
    property int partHeight: 100
    property int itemWidth: (partWidth - 50)/2
    property int itemHeight: (partHeight - 50)/2
    property var eimcMode: commandMgr.currentVehicle.eimcMode
    property var batteryMode: commandMgr.currentVehicle.batteryMode

    width: partWidth
    height: partHeight

    GroupRectangle {
        id: control
        width: partWidth
        height: partHeight
        anchors.fill: parent
        penColor: "#1e90ff" //GcsTheme.dividerColor
        textColor: leftRightColor
        text: qsTr("EMIC & 电池")
    }

    Item {
        id: other
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.topMargin: 30
        width: partWidth - 40
        height: 70

        Column {
            anchors.fill: parent
            anchors.bottomMargin: 20
            spacing: 1
            Row {
                GcsText {
                    height: 30
                    width: (partWidth - 40)*2/5
                    color: GcsTheme.primaryColor
                    text: qsTr("")
                }
                GcsText {
                    height: 30
                    width: (partWidth - 40)/5
                    color: GcsTheme.primaryColor
                    text: qsTr("电压(V)")
                }
                GcsText {
                    height: 30
                    width: (partWidth - 40)/5
                    color: GcsTheme.primaryColor
                    text: qsTr("电流(A)")
                }
                GcsText {
                    height: 30
                    width: (partWidth - 40)/5
                    color: GcsTheme.primaryColor
                    text: qsTr("容量")
                }
            }

            Repeater {
                model: eimcMode?eimcMode.dataModel:null
                Row {
                    property var emicObj: object
                    property int aSetColor: emicObj.valtage.showColor
                    property string avaltageUnit: emicObj.valtage.unit
                    property string acurrentUnit: emicObj.current.unit
                    property string acapacityUnit: emicObj.capacity.unit
                    property string avaltageStr: emicObj.valtage.display
                    property string acurrentStr: emicObj.current.display
                    property string acapacityStr: emicObj.capacity.display
                    Connections{
                       target: emicObj.valtage
                       function onUpdateColorValue(value, icolor) {
                           avaltageStr = (icolor > 0xf) ? qsTr("--") : value + avaltageUnit
                           aSetColor = icolor
                       }
                    }
                    Connections{
                       target: emicObj.current
                       function onUpdateColorValue(value, icolor) {
                           acurrentStr = (icolor > 0xf) ? qsTr("--") : value  + acurrentUnit
                       }
                    }
                    Connections{
                       target: emicObj.capacity
                       function onUpdateColorValue(value, icolor) {
                           acapacityStr = (icolor > 0xf) ? qsTr("--") : value  + acapacityUnit
                       }
                    }
                    GcsText {
                       height: 30
                       width: (partWidth - 40)*2/5
                       text: emicObj.groupName + qsTr(":")
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

            Repeater {
                model: batteryMode?batteryMode.dataModel:null
                Row {
                    property var emicObj: object
                    property int aSetColor: emicObj.valtage.showColor
                    property string avaltageUnit: emicObj.valtage.unit
                    property string acurrentUnit: emicObj.current.unit
                    property string acapacityUnit: emicObj.capacity.unit
                    property string avaltageStr: emicObj.valtage.display
                    property string acurrentStr: emicObj.current.display
                    property string acapacityStr: emicObj.capacity.display
                    Connections{
                       target: emicObj.valtage
                       function onUpdateColorValue(value, icolor) {
                           avaltageStr = (icolor > 0xf) ? qsTr("--") : value  + emicObj.valtage.unit
                           aSetColor = icolor
                       }
                    }
                    Connections{
                       target: emicObj.current
                       function onUpdateColorValue(value, icolor) {
                           acurrentStr = (icolor > 0xf) ? qsTr("--") : value  + acurrentUnit
                       }
                    }
                    Connections{
                       target: emicObj.capacity
                       function onUpdateColorValue(value, icolor) {
                           acapacityStr = (icolor > 0xf) ? qsTr("--") : value  + acapacityUnit
                       }
                    }
                    GcsText {
                       height: 30
                       width: (partWidth - 40)*2/5
                       text: emicObj.groupName + qsTr(":")
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
}

