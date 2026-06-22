import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickTemperatureElem 1.0
import QuickTemperature 1.0
import QuickElectricElem 1.0
import QuickBatteryElem 1.0
import QuickElectricElem2 1.0
import QuickElectricElem3 1.0
import GcsQuick 1.0

ControlBackground {

    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property var batteryMode: currVehicle ? currVehicle.batteryMode : null
    property var generatorMode: currVehicle ? currVehicle.generatorMode : null
    property var eimcMode: currVehicle ? currVehicle.eimcMode : null
    property var elecSpdCtrl: currVehicle ? currVehicle.elecSpdCtrl : null
    property var warnModel: currVehicle ? currVehicle.warnModel : nul
    property var temperMode: currVehicle ? currVehicle.temperatureMode : null
    property var temperModeList: temperMode ? temperMode.dataModel : null
    property int temperCount: temperMode && (temperMode.count < 4)?4:temperMode.count
    property color textColor: GcsTheme.fontPrimaryColor
    property int eachItemWidth: electComp /9
    property int temperateWidth: ctrlWidth/temperCount

    id: electComp
    width: ctrlWidth
    height: ctrlHeight

    Component {
        id: xfelectric
        QuickElectricElem2 {
            id: elecelem2
            color: GcsTheme.primaryColor
            greenColor: GcsTheme.green
            whiteColor: GcsTheme.White
            warnColor: GcsTheme.orange
            fontColor: GcsTheme.primaryColor
            anchors.fill: parent
            generator: generatorMode
            battery: batteryMode
            elecNode: eimcMode
        }
    }

    Component {
        id: whxqelectric
        QuickElectricElem3 {
            id: elecElem3
            color: GcsTheme.primaryColor
            greenColor: GcsTheme.primaryColor
            whiteColor: GcsTheme.White
            warnColor: GcsTheme.orange
            fontColor: GcsTheme.primaryColor
            anchors.fill: parent
            generator: generatorMode
            battery: batteryMode
            elecNode: elecSpdCtrl
        }
    }

    Component {
        id: c21electric
        QuickElectricElem {
            color: GcsTheme.primaryColor
            greenColor: GcsTheme.green
            whiteColor: GcsTheme.White
            warnColor: GcsTheme.orange
            fontColor: GcsTheme.primaryColor
            anchors.fill: parent
            battery: batteryMode
            elecNode: eimcMode
        }
    }

    Component{
        id: commonshow
        GcsFlightCommonState {
            anchors.fill: parent
            ctrlWidth: electComp.ctrlWidth
            ctrlHeight: electComp.ctrlHeight
        }
    }

    Row {
        height: 110
        width: ctrlWidth
        anchors.bottom: parent.bottom
        Repeater{
            model: projectEnv === 3 ? null: temperModeList
            delegate: QuickTemperature {
                color:GcsTheme.green
                fontColor: GcsTheme.primaryColor
                warnColor: GcsTheme.orange
                width: temperateWidth
                height: 110
                dataNode: object
            }
        }
    }

    GcsLoader {
        anchors.fill: parent
        sourceComponent: {
            if(projectEnv === 1) {
                return c21electric;
            }
            if (projectEnv === 2) {
                /// 发动机 与 设备开关的时候显示
                if ((menuModelSel === 2) || (menuModelSel === 4)) {
                    return xfelectric;
                } else {
                    return commonshow;
                }
            }
            if (projectEnv === 3) {
                return whxqelectric;
            }
        }
    }
}
