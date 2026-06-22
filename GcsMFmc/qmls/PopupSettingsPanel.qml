import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Popup {
    id: control
    focus: true
    background: ControlBackground{
        radius: 5
    }
    ColumnLayout{
        id: controlPart
        height: parent.height
        width: parent.width

        Repeater {
            model: deviceRC ? deviceRC.devices : null
            delegate: Item {
                property var deviceObj: object
                property var deviceInfo: object.deviceInfo
                property int eachWidth: (parent.width-40)/4

                height: 100
                width: parent.width
                GcsDivider {
                    orientation: Qt.Horizontal
                    borderColor: GcsTheme.itemGrayColor
                    anchors.bottom: parent.bottom
                    width: parent.width
                    spacing: 0
                }
                Grid{
                    columns: 4
                    anchors.fill: parent
                    GcsText{
                        text: deviceInfo.wXRaw
                        width: eachWidth
                        height: 30
                    }
                    GcsText{
                        text: deviceInfo.wYRaw
                        width: eachWidth
                        height: 30
                    }
                    GcsText{
                        text: deviceInfo.wZRaw
                        width: eachWidth
                        height: 30
                    }
                    GcsText{
                        text: deviceInfo.wRRaw
                        width: eachWidth
                        height: 30
                    }
                    GcsText{
                        text: deviceInfo.wURaw
                        width: eachWidth
                        height: 30
                    }
                    GcsText{
                        text: deviceInfo.wVRaw
                        width: eachWidth
                        height: 30
                    }
                    GcsText{
                        text: deviceInfo.povXRaw
                        width: eachWidth
                        height: 30
                    }
                    GcsText{
                        text: deviceInfo.povYRaw
                        width: eachWidth
                        height: 30
                    }
                    GcsText{
                        text: deviceInfo.btnState
                        width: eachWidth
                        height: 30
                    }
                    GcsText{
                        text: deviceInfo.pressedBtns
                        width: eachWidth
                        height: 30
                    }
                }
            }
        }
    }
}
