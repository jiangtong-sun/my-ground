import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

ControlBackground {
    id: control
    property var lastAdjustCmd
    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property int opticalItemH: 32
    property var lastSelectEelm: null
    property int opticalIWidth: (ctrlWidth - 90)/3
    property int opticalMWidth: opticalIWidth * 2
    property var currAdjust: vechicleRC.commands.engineSource

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

    Item {
        id: srcSet
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: srcAck.left
        anchors.rightMargin: 10
        anchors.topMargin: 10
        anchors.bottomMargin: 20
        anchors.leftMargin: 20

        Column{
            anchors.fill: parent
            spacing: 1
            Repeater {
                model: currAdjust.adjustCmds
                Row{
                    property var parentObject: object
                    property var enumValStr: object.enumValStr
                    property int rowSrc: Math.ceil(enumValStr.length/2)
                    property int fectIndex: index
                    spacing: 1
                    GcsButton {
                        text: parentObject.name
                        width: opticalIWidth
                        height: opticalItemH
                    }
                    Grid {
                        columns: 2
                        height: opticalItemH * rowSrc + rowSrc*2
                        width: opticalMWidth
                        Repeater{
                            model: enumValStr
                            GcsToggleButton {
                                height: opticalItemH
                                width: opticalMWidth/2
                                text: modelData
                                checkeState: (parentObject.selectId === index) ? true: false
                                onClicked: {
                                    if (lastSelectEelm != parentObject && lastSelectEelm) {
                                        lastSelectEelm.selectId = -1
                                    }
                                    parentObject.selectId = index
                                    lastSelectEelm = parentObject
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
