import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    id: root
    property var firstModePart: null
    property var secondModePart: null
    property int navItemHeight: 36
    property int navItemWidth: 150

    function setModePart(first, second) {
        firstModePart = first
        secondModePart = second
    }

    Item {
        id: srcAck
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.rightMargin: 20
        height: 100
        width: 40
        GcsFilledButton {
            anchors.fill: parent
            text: qsTr("发\n送\n确\n认")
            onClicked: {

            }
        }
    }

    Row {
        id: common
        anchors.left: parent.left
        width: leftWidth + middleWidth
        height: parent.height
        spacing: 5

        Grid{
            width: columnFirst * navItemWidth
            height: parent.height
            columns: columnFirst
            spacing: 1
            Repeater {
                model: firstModePart
                GcsTextButton {
                    width: navItemWidth
                    height: navItemHeight
                    labelValue: object.name
                    textValue: object.display
                }
            }
        }

        Grid{
            width: columnSecond * navItemWidth
            height: parent.height
            columns: columnSecond
            spacing: 1
            Repeater {
                model: secondModePart
                GcsTextButton {
                    width: navItemWidth
                    height: navItemHeight
                    labelValue: object.name
                    textValue: object.display
                }
            }
        }
    }

    GcsEngineOperateCmd {
        id: operate
        width: rightWidth
        height: root.height
        ctrlWidth: rightWidth
        ctrlHeight: root.height
        anchors.left: common.right
        anchors.top: root.top
    }
}
