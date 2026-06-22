import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

import "AirTrafficControlSystem"
import "DataLinkerSystem"
import "DeviceExecutorSystem"
import "EnginePowerSystem"
import "FlightManageSystem"
import "NavigationSensor"

Item {
    id: root
    anchors.fill: parent

    GcsFlightWarnDisplay{
        id: warnPart
        ctrlWidth: leftWidth
        ctrlHeight: parent.height
        anchors.left: parent.left
        width: leftWidth
        height: parent.height
    }

    GcsFlightNormalDisplay{
        id: normalDisplay
        ctrlWidth: middleWidth
        ctrlHeight: parent.height
        anchors.left: warnPart.right
        width: middleWidth
        height: parent.height
    }

    function loadXFProjEnv() {
        if (leftMenuGroupSel == 0) {
            if (rightMenuGroupSel === 0) {
                return commonctrl;
            }
            if (rightMenuGroupSel === 1) {
                return commandadjust;
            }
            if (rightMenuGroupSel === 2) {
                return controlextent;
            }
        }
        if ((leftMenuGroupSel == 1)) {
            return xfsimpleadjust;
        }
        if ((leftMenuGroupSel == 2)) {
            return xfinfosourceoptpanel;
        }
        return datamgroptpanel;
    }

    Item {
        anchors.left: normalDisplay.right
        anchors.right: parent.right
        height: parent.height
        width: rightWidth
        GcsLoader {
            anchors.fill: parent
            sourceComponent: {
                return loadXFProjEnv()
            }
            onLoaded: {
                item.focus = true
                item.forceActiveFocus()
            }
        }
    }

    Component{
        id: commonctrl
        GcsControlCommonCmds {
            height: parent.height
            width: rightWidth
            ctrlWidth: rightWidth
            ctrlHeight: parent.height
        }
    }

    Component{
        id: commandadjust
        GcsControlAdjustCmds {
            height: parent.height
            width: rightWidth
            ctrlWidth: rightWidth
            ctrlHeight: parent.height
        }
    }

    Component{
        id: controlextent
        GcsControlExtentCmds{
            height: parent.height
            width: rightWidth
            ctrlWidth: rightWidth
            ctrlHeight: parent.height
        }
    }

    Component{
        id: xfsimpleadjust
        XFDeviceAdjustPanel{
            height: parent.height
            width: rightWidth
            ctrlWidth: rightWidth
            ctrlHeight: parent.height
        }
    }

    Component{
        id: xffiberoptpanel
        XFFiberAdjustPanel {
            height: parent.height
            width: rightWidth
            ctrlWidth: rightWidth
            ctrlHeight: parent.height
            Layout.alignment: Qt.AlignCenter
        }
    }

    Component{
        id: xfinfosourceoptpanel
        XFInfoSourceAdjustPanel {
            height: parent.height
            width: rightWidth
            ctrlWidth: rightWidth
            ctrlHeight: parent.height
            Layout.alignment: Qt.AlignCenter
        }
    }

    Component{
        id: xfnavdevoptpanel
        XFNavDeviceSetPanel {
            height: parent.height
            width: rightWidth
            ctrlWidth: rightWidth
            ctrlHeight: parent.height
            Layout.alignment: Qt.AlignCenter
        }
    }

    Component{
        id: datamgroptpanel
        GcsControlPowerCmds {
            height: parent.height
            width: rightWidth
            ctrlWidth: rightWidth
            ctrlHeight: parent.height
        }
    }
}
