import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    id: adjust

    // GridLayout{
    //     anchors.fill: parent
    //     columns: 2
    //     Repeater {
    //         model: currAdjust
    //         Item{
    //             width: adjustWidth
    //             height: adjustHeight
    //             ControlBackground{
    //                 id: inputCtrl
    //                 width: adjustWidth/2
    //                 height: adjustHeight
    //                 GcsTextBox {
    //                     anchors.fill: parent
    //                     validator: iValidator
    //                     text: object.defValue
    //                     id: devtypeinput
    //                 }
    //             }
    //             GcsToggleButton {
    //                 width: adjustWidth/2
    //                 height: adjustHeight
    //                 checkeState: object.working
    //                 anchors.left: inputCtrl.right
    //                 text: object.name
    //                 onClicked: {
    //                     if (devtypeinput.text.trim().length <= 0){
    //                         devtypeinput.focus = true
    //                     }
    //                     else {
    //                         if(lastAdjustCmd) {
    //                             lastAdjustCmd.working = false
    //                         }
    //                         devtypeinput.focus = false
    //                         object.working = true
    //                         lastAdjustCmd = object
    //                         commandMgr.sendDeviceSetCommand(object, devtypeinput.text.trim());
    //                     }
    //                 }
    //                 onDoubleClicked: {
    //                     devtypeinput.focus = false
    //                     object.working = false
    //                     lastAdjustCmd = null
    //                 }
    //             }
    //         }
    //     }
    // }
}
