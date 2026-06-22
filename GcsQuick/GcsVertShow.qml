import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item{
    property string labelDisplay: ""
    property string labelValue: ""
    property color textDefColor: GcsTheme.fontPrimaryColor
    property bool textType: true
    property bool iconType: false
    property bool showIcon: false

    id: control

    Item {
        id:value
        width: control.width
        height: control.height/2
        anchors.top: parent.top
        GcsText{
            text: labelDisplay
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    Item {
        id: type
        width: control.width
        height: control.height/2
        anchors.bottom: parent.bottom

        GcsText {
            text: labelValue
            visible: textType
            anchors.fill: parent
            textColor: textDefColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Rectangle {
            visible: iconType
            color: showIcon ? GcsTheme.itemWarnColor: "transparent"
            border.color: textDefColor
            border.width: 1
            anchors.left: type.left
            anchors.top: type.top
            anchors.leftMargin: (type.width-16)/2
            anchors.topMargin: (type.height-16)/2
            radius: 10
            width: 16
            height: 16
        }
    }
}
