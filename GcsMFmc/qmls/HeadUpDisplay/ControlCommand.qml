import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

ControlBackground {
    id: cmd_adjust

    property var currCommand: commandMgr.currCommand
    property int maxWords: currCommand.maxWords
    property int countCmd: currCommand.size
    property int ctlHeight: Math.ceil(countCmd/4+1)*60
    property int minWidth: Math.floor(gridpanel.width/4)-10

    GroupRectangle {
        id: control
        width: parent.width
        height: ctlHeight
        anchors.top: parent.top
        penColor: GcsTheme.dividerColor

        GcsText{
            height: 40
            anchors{
                top: control.top
                right: control.right
                left: control.left
            }
            anchors.rightMargin: parent.width/3
            anchors.leftMargin: parent.width/3
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: "Control"
        }

        Item {
            id: gridpanel
            anchors.fill: parent
            anchors.margins: 20
            anchors.topMargin: 40

            GridLayout{
                anchors.fill: parent
                columns: 4
                Repeater {
                    model: currCommand.commands
                    GcsToggleButton {
                        Layout.minimumWidth: minWidth
                        Layout.minimumHeight: 40
                        text: object.name
                    }
                }
            }
        }
    }

    GroupRectangle {
        id: adjust
        width: parent.width
        height: parent.height-ctlHeight
        anchors.top: control.bottom
        penColor: GcsTheme.dividerColor

        GcsText {
            height: 40
            anchors {
                top: adjust.top
                right: adjust.right
                left: adjust.left
            }
            anchors.rightMargin: parent.width/3
            anchors.leftMargin: parent.width/3
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: "Adjust"
        }
    }
}
