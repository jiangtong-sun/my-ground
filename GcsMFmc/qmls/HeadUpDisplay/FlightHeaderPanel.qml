import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickDataLinkElem 1.0
import GcsQuick 1.0

Item {
    id: base_bar

    property int switchFont: 0
    property int vWidth: headHeight * 3
    property int vHeight: headHeight
    property bool stayTop: false
    property bool isRestore: false
    property string darkText : qsTr("Dark")
    property string lightText : qsTr("FontColor")
    property string stayTopText : qsTr("Sticky on Top")
    property string stayTopCancelText : qsTr("Sticky on Top cancelled")
    property string restoreText : qsTr("Restore")
    property string maximizeText : qsTr("Maximize")
    property string closeText : qsTr("Close")
    property color textColor: GcsTheme.fontPrimaryColor
    property var vehicleList: commandMgr.aircrafts
    property var navigationMode: currVehicle ? currVehicle.NavigationMode: null
    property var navItemName: navigationMode ? navigationMode.itemName : ""
    property var navItemDisplay: navigationMode ? navigationMode.display : ""
    property var videoReplayListener: function() {}
    property var stayTopClickListener: function(){
        stayTop = !stayTop
        if (stayTop) {
            hudwindow.flags = Qt.WindowStaysOnTopHint
        } else {
            hudwindow.flags = Qt.FramelessWindowHint
        }
    }
    property var maxClickListener: function(){
        isRestore = !isRestore;
        if (hudwindow.visibility ===  Window.Maximized) {
            hudwindow.showNormal()
        } else {
            hudwindow.showMaximized()
        }
    }
    property var closeClickListener: function(){
        hudwindow.close()
    }
    property var darkClickListener: function(){
        switchFont = (switchFont+1)%4
        if(switchFont == 0) {
            GcsTheme.fontPrimaryColor = GcsTheme.blue
        } else if(switchFont == 1) {
            GcsTheme.fontPrimaryColor = GcsTheme.green
        } else if(switchFont == 2) {
            GcsTheme.fontPrimaryColor = GcsTheme.White
        } else if(switchFont == 3) {
            GcsTheme.fontPrimaryColor = GcsTheme.green
        }
    }

    Grid {
        id: linkLU
        anchors.right: parent.right
        height: vHeight
        width: 200
        columns: 2

        QuickDataLinkElem {
            id: mainElem
            color: GcsTheme.green
            dataNode: mainLinker
            linkName: mainLinker.name
            width: 100
            height: vHeight
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    mainLinker.working = true
                    slaveLinker.working = false
                }
            }
        }

        QuickDataLinkElem {
            id: slaveElem
            color: GcsTheme.green
            dataNode: slaveLinker
            linkName: slaveLinker.name
            width: 100
            height: vHeight
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    mainLinker.working = false
                    slaveLinker.working = true
                }
            }
        }
    }

    Grid{
        id: setbar
        anchors.left: parent.left
        height: vHeight
        width: 240
        columns: 3

        GcsIconButton{
            id:btn_maximize
            height: vHeight/2
            width: 80
            padding: 0
            verticalPadding: 0
            horizontalPadding: 0
            iconSource: isRestore ? ttfIcons["screenShrink"] : ttfIcons["screenExpand"]
            text: isRestore? base_bar.restoreText : base_bar.maximizeText
            onClicked: maxClickListener()
        }

        GcsIconButton{
            id:btn_close
            height: vHeight/2
            width: 80
            padding: 0
            verticalPadding: 0
            horizontalPadding: 0
            iconSource : ttfIcons["closeBold"]
            text: base_bar.closeText
            onClicked: closeClickListener()
        }

        GcsIconButton{
            id: btn_time
            height: vHeight/2
            width: 80
            text: configMgr.appRunTime
            display: Button.TextOnly
            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                property point startPos: Qt.point(0,0)
                onPressed: (mouse) => {
                    startPos = Qt.point(mouse.x, mouse.y)
                }
                onPositionChanged: (mouse) => {
                    if (pressed) {
                        hudwindow.x += (mouse.x - startPos.x)
                        hudwindow.y += (mouse.y - startPos.y)
                    }
                }
            }
        }

        GcsIconButton{
            id: btn_setting
            padding: 0
            height: vHeight/2
            width: 80
            verticalPadding: 0
            horizontalPadding: 0
            display: Button.TextOnly
            text: "V" + configMgr.appVersion
        }

        GcsIconButton{
            id:btn_dark
            padding: 0
            height: vHeight/2
            width: 80
            verticalPadding: 0
            horizontalPadding: 0
            iconSource: ttfIcons["favorite"]
            text: GcsTheme.dark ? base_bar.lightText : base_bar.darkText
            onClicked: darkClickListener()
        }

        GcsIconButton{
            id:btn_stay_top
            padding: 0
            height: vHeight/2
            width: 80
            verticalPadding: 0
            horizontalPadding: 0
            iconSource : ttfIcons["home"]
            text: stayTop ? base_bar.stayTopCancelText : base_bar.stayTopText
            onClicked: stayTopClickListener()
        }
    }

    GcsStationPanel{
        id: station
        anchors.right: linkLU.left
        height: vHeight
        width: 110
    }

    ColumnLayout {
        id: nametest
        anchors.left: setbar.right
        anchors.leftMargin: 10
        width: leftWidth - 260
        height: vHeight
        GcsButton{
            normalColor: "transparent"
            text: qsTr("RM")
            onClicked: {
                commandMgr.exportRMData()
            }
        }
        GcsButton{
            normalColor: "transparent"
            text: qsTr("RC")
            onClicked: {
                commandMgr.exportRCData()
            }
        }
    }

    Item {
        id: joystickPart
        width: projectEnv === 3?400:2
        visible: projectEnv === 3?true:false
        anchors.left: nametest.right
        height: vHeight
        Row {
            anchors.fill: parent
            Item {
                height: vHeight
                width: 100
                GcsText{
                    anchors.fill: parent
                    text: qsTr("摇杆\n设置")
                }
            }
            Item {
                height: vHeight
                width: 100
                GcsRadioButton {
                    checked: !configMgr.verticalMode
                    anchors.top: parent.top
                    height: vHeight/2
                    width: 100
                    text: qsTr("高度")
                    clickListener: ()=>{
                        configMgr.verticalMode = !configMgr.verticalMode
                    }
                }
                GcsRadioButton {
                    checked: configMgr.verticalMode
                    anchors.bottom: parent.bottom
                    height: vHeight/2
                    width: 100
                    text: qsTr("下沉率")
                    clickListener: ()=>{
                        configMgr.verticalMode = !configMgr.verticalMode
                    }
                }
            }

            Item {
                height: vHeight
                width: 100
                GcsRadioButton {
                    checked: !configMgr.longitudeMode
                    anchors.top: parent.top
                    height: vHeight/2
                    width: 100
                    text: qsTr("X位置")
                    clickListener: ()=>{
                        configMgr.longitudeMode = !configMgr.longitudeMode
                    }
                }
                GcsRadioButton {
                    checked: configMgr.longitudeMode
                    anchors.bottom: parent.bottom
                    height: vHeight/2
                    width: 100
                    text: qsTr("X速度")
                    clickListener: ()=>{
                        configMgr.longitudeMode = !configMgr.longitudeMode
                    }
                }
            }

            Item {
                height: vHeight
                width: 100
                GcsRadioButton {
                    checked: !configMgr.latitudeMode
                    anchors.top: parent.top
                    height: vHeight/2
                    width: 100
                    text: qsTr("Y位置")
                    clickListener: ()=>{
                        configMgr.latitudeMode = !configMgr.latitudeMode
                    }
                }
                GcsRadioButton {
                    checked: configMgr.latitudeMode
                    anchors.bottom: parent.bottom
                    height: vHeight/2
                    width: 100
                    text: qsTr("Y速度")
                    clickListener: ()=>{
                        configMgr.latitudeMode = !configMgr.latitudeMode
                    }
                }
            }
        }

        GcsDivider{
            orientation: Qt.Vertical
            anchors.left: parent.left
            height: parent.height
            spacing: 0
        }
    }

    Row{
        id: head_view
        anchors.left: joystickPart.right
        anchors.right: station.left
        Repeater {
            model: vehicleList
            FlightVehiclePanel {
                vehicle: object
                width: vWidth
                height: vHeight
            }
        }

        ControlBackground {
            id: navPart
            height: vHeight
            width: vWidth
            GcsText {
                id: partName
                height: vHeight/2
                width: vWidth
                text: navItemName
            }
            GcsText {
                text: navItemDisplay
                textColor: GcsTheme.green
                anchors.top: partName.bottom
                height: vHeight/2
                width: vWidth
            }
            GcsDivider{
                orientation: Qt.Vertical
                anchors.right: parent.right
                height: parent.height
                spacing: 0
            }
        }
    }

    GcsDivider{
        anchors.bottom: parent.bottom
        width: parent.width
        spacing: 0
    }
}
