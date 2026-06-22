import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    id: control
    property var lastAdjustCmd

    // Item {
    //     anchors.fill: parent
    //     anchors.leftMargin: 30
    //     anchors.rightMargin: 30
    //     anchors.topMargin: itemHeight
    //     anchors.bottomMargin: 60
    //     GridLayout{
    //         anchors.fill: parent
    //         columns: ctlColumn
    //         columnSpacing: 20
    //         rowSpacing: 5
    //         Repeater {
    //             model: currControl
    //             GcsToggleButton {
    //                 Layout.minimumWidth: cmdWidth
    //                 Layout.minimumHeight: itemHeight
    //                 checkeState: object.working
    //                 text: object.name
    //                 onClicked: {
    //                     if(lastAdjustCmd !== object && lastAdjustCmd) {
    //                         lastAdjustCmd.working = false
    //                     }
    //                     var setv = object.working
    //                     if (setv) {
    //                         object.working = false
    //                         lastAdjustCmd = null
    //                     } else {
    //                         object.working = true
    //                         lastAdjustCmd = object
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }


}
