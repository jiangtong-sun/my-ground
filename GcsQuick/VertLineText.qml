import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    id: root
    property string txtLabel:""
    property string txtValue:""

    ControlBackground {
        id: rect
        width: root.width
        height: root.height/2
        anchors.top: root.top
        anchors.left: root.left

        GcsText {
            id: value
            text: txtValue
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.NoWrap
        }
    }

    GcsText {
        text: txtLabel
        horizontalAlignment:Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.NoWrap
        width: root.width
        height: root.height/2
        anchors.topMargin: 1
        anchors.top: rect.bottom
        anchors.left: root.left
    }
}
