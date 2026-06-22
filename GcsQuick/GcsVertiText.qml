import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

ControlBackground{
    property var enumListStr
    property string labelValue: ""
    property int itemRawVal: -1
    property int enumCount: enumListStr.length
    property real enumItemWidth: ((control.width-20)/enumCount)-2

    id: control

    Item {
        id:value
        width: control.width-20
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: 4

        Row {
            anchors.fill: parent
            spacing: 4
            Repeater {
                model: enumListStr
                // GcsToggleButton{
                //     width: enumItemWidth
                //     height: control.height/2
                //     text: enumListStr[index]
                //     checkeState: (itemRawVal==index)?true:false
                // }
                Rectangle {
                    width: enumItemWidth
                    height: control.height/2
                    border.width: 1
                    border.color: (itemRawVal==index)? "#1e90ff" : GcsTheme.dividerColor
                    color: (itemRawVal==index)? "transparent" : "transparent"
                    GcsText{
                        text: enumListStr[index]
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }

    Item {
        id: type
        width: control.width
        height: control.height/2
        anchors.bottom: parent.bottom

        GcsText {
            text: labelValue
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}
