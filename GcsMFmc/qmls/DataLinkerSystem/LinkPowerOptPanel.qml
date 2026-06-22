import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    id: control

    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property var lastSelectEelm: null
    property int deviceItemWidth: 120
    property var dataLinkSource: vechicleRC.commands.dataLinkSource

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
                commandMgr.sendSimpleCommand(lastSelectEelm);
            }
        }
    }

    Item {
        id: srcSet
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: srcAck.left
        anchors.rightMargin: 10
        anchors.topMargin: 10
        width: deviceItemWidth *3

        Column{
            anchors.fill: parent
            spacing: 10
            Repeater {
                model: dataLinkSource.adjustCmds
                ControlBackground {
                    property var infoObj: object
                    property var infoSrc: infoObj.enumValStr
                    property int counts: infoSrc.length
                    width: parent.width
                    height: navItemHeight
                    Row {
                        anchors.fill: parent
                        GcsToggleButton {
                            height: navItemHeight
                            width: deviceItemWidth
                            anchors.top: parent.top
                            text: infoObj.name
                            checkeState: infoObj.working?true:false
                        }
                        Repeater{
                            model: infoSrc
                            GcsToggleSwitch {
                                text: modelData
                                height: navItemHeight
                                width: deviceItemWidth
                                checked: (infoObj.selectId === index) ? true: false
                                clickListener: () => {
                                    if (lastSelectEelm) {
                                        lastSelectEelm.working = false
                                        lastSelectEelm.selectId = -1
                                    }
                                    infoObj.working = true
                                    infoObj.selectId = index
                                    lastSelectEelm = infoObj
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
