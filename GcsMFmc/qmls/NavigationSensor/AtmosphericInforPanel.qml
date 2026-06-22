import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    id: root
    property var dataModePart: null
    property var subGroupStr: null
    property int navItemHeight: 36
    property int navItemWidth: 180

    function setModePart(statusMode, countGroupElem, eachHeight, groupStr) {
        var count = 0
        for (var ixm in countGroupElem) {
            var tmp = countGroupElem[ixm]
            count += Math.ceil(tmp / ((secondHeight - eachHeight)/eachHeight))
        }
        count = count > 0? count: 1;
        dataModePart = statusMode
        subGroupStr = groupStr
        navItemHeight = eachHeight
        navItemWidth = (root.width-2)/count
        navItemWidth = navItemWidth>300?300:navItemWidth
    }

    Row{
        anchors.fill: parent
        Repeater{
            model: dataModePart
            delegate: Item {
                property var dataMode: object
                property var devFirstCount: dataMode.count
                property int columnFirst: Math.ceil(devFirstCount / ((secondHeight - navItemHeight)/navItemHeight))
                property int firstItemWidth: (columnFirst * navItemWidth - 20)/columnFirst
                property int firstLROffset: (columnFirst * navItemWidth)/4

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
                    text: subGroupStr[index]
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
                            model: dataMode
                            GcsTextButton {
                                property var displayNode: object
                                property string nodeUnit: object.unit

                                id: dataelem
                                width: firstItemWidth
                                height: navItemHeight
                                labelValue: displayNode.itemName
                                btnFontSize: TextStyle.BodySmall
                                Component.onCompleted:  {
                                    if (displayNode && displayNode.display) {
                                        var mm =  ((displayNode.showColor > 0xf)) ? qsTr("--") : displayNode.display
                                        dataelem.textValue = mm + nodeUnit
                                        dataelem.textColor = txtColorMap[displayNode.showColor & 0xf]
                                    }
                                }

                                Connections{
                                    target: displayNode
                                    function onUpdateColorValue(valueStr, iColor){
                                        dataelem.textValue = (iColor > 0xf) ? qsTr("--") + nodeUnit : valueStr + nodeUnit;
                                        dataelem.textColor = txtColorMap[iColor & 0xf]
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
