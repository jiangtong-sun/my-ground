import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    id: root
    property var firstModePart: null
    property var secondModePart: null
    property var subGroupStr: null
    property int navItemHeight: 36
    property int navItemWidth: 200
    property int firstItemWidth: 200
    property int secondItemWidth: 200
    property int firstLROffset: 80
    property int secondLROffset: 80
    property int operateWidth: 100
    property int columnFirst: 1
    property int columnSecond: 1

    function setModePart(first, second, eachHeight, groupStr) {
        var devFirstCount = first?first.count: 0
        var devSecondCount = second?second.count: 0
        columnFirst = Math.ceil(devFirstCount / ((secondHeight - eachHeight)/eachHeight))
        columnSecond = Math.ceil(devSecondCount / ((secondHeight - eachHeight)/eachHeight))
        firstModePart = first
        secondModePart = second
        subGroupStr = groupStr
        navItemHeight = eachHeight
        columnFirst = columnFirst > 0 ? columnFirst : 1
        columnSecond = columnSecond > 0 ? columnSecond : 1
        firstItemWidth = (columnFirst * navItemWidth - 20)/columnFirst
        secondItemWidth = (columnSecond * navItemWidth - 20)/columnSecond
        operateWidth = window.width - lrOptWidth*2 - (columnFirst + columnSecond) * navItemWidth
        firstLROffset = (columnFirst * navItemWidth)/4
        secondLROffset = (columnSecond * navItemWidth)/4
    }


    Item {
        id: common
        anchors.left: parent.left
        width: columnFirst * navItemWidth
        height: parent.height
        GcsDivider{
            orientation: Qt.Horizontal
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 10
            width: firstLROffset
            height: navItemHeight
            spacing: 20
        }
        GcsDivider{
            orientation: Qt.Horizontal
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.rightMargin: 10
            width: firstLROffset
            height: navItemHeight
            spacing: 20
        }
        GcsText {
            height: navItemHeight
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.leftMargin: firstLROffset
            anchors.rightMargin: firstLROffset
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: subGroupStr ? subGroupStr[0] : ""
        }
        Item {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: navItemHeight
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10

            Grid{
                anchors.fill: parent
                columns: columnFirst
                spacing: 1
                Repeater {
                    model: firstModePart
                    GcsTextButton {
                        width: firstItemWidth
                        height: navItemHeight
                        labelValue: object.itemName
                        textValue: object.display
                    }
                }
            }
        }
    }

    Item {
        id: common2
        anchors.left: common.right
        width: columnSecond * navItemWidth
        height: parent.height
        GcsDivider{
            orientation: Qt.Horizontal
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 10
            height: navItemHeight
            width:secondLROffset
            spacing: 20
        }
        GcsDivider{
            orientation: Qt.Horizontal
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.rightMargin: 10
            width:secondLROffset
            height: navItemHeight
            spacing: 20
        }

        GcsText {
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.leftMargin: secondLROffset
            anchors.rightMargin: secondLROffset
            height: navItemHeight
            text: subGroupStr ? subGroupStr[1] : ""
        }

        Item {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: navItemHeight
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10

            Grid{
                anchors.fill: parent
                columns: columnSecond
                spacing: 1
                Repeater {
                    model: secondModePart
                    GcsTextButton {
                        width: secondItemWidth
                        height: navItemHeight
                        labelValue: object.itemName
                        textValue: object.display
                    }
                }
            }
        }
    }
}
