import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QQuickVideoView 1.0
import GcsStationRole 1.0
import GcsQuick 1.0

Window {
    property string frontName: "16px bold Arial"
    property url appIcon: "qrc:///res/favicon.ico"
    property int winHeight: configMgr.hudLocate.height
    property int winWidth: configMgr.hudLocate.width
    property int headHeight: 60
    property int leftWidth: winWidth/6
    property int rightWidth: winWidth/6
    property int middleWidth: winWidth-leftWidth-rightWidth
    property int firstHeight: hudwindow.height-headHeight-2
    property int lrWidth: leftWidth < 192 ? 192 : leftWidth
    property int hudDHeight: firstHeight/4
    property int hudUHeight: firstHeight/3
    property int itemWidth: (lrWidth-80)/3
    property var currVehicle:  null
    property color backgroundColor: GcsTheme.backgroundColor
    property color drawLineColor: GcsTheme.fontPrimaryColor
    property color leftRightColor: GcsColors.SFont.dark
    property var mainLinker: commandMgr.mainLinker
    property var slaveLinker: commandMgr.slaveLinker
    property int stationRole: commandMgr.stationRole

    id: hudwindow
    visible: true
    title: qsTr("GCSHUD")
    flags: Qt.FramelessWindowHint|Qt.Window|configMgr.staysOnTopHint //|Qt.WindowStaysOnTopHint
    screen: Qt.application.screens[configMgr.hudLocate.screenId]
    x: screen.virtualX + configMgr.hudLocate.xpos+1
    y: screen.virtualY + configMgr.hudLocate.ypos
    width: winWidth-1
    height: winHeight
    color: backgroundColor

    Component.onCompleted: {
        commandMgr.switchVehicel(0)// vehicle 0
        currVehicle = commandMgr.currVehicle()
        if (stationRole === GcsStationRole.ControlRole) {
            videoView.openStream(true)
        }
    }

    onStationRoleChanged: {
        var enable = stationRole === GcsStationRole.ControlRole ? true: false;
        videoView.replayStream(true, enable);
    }

    Item {
        id: vidoeComp
        anchors.fill: parent
        anchors.topMargin: headHeight
        anchors.leftMargin: leftWidth
        anchors.rightMargin: rightWidth

//        QQuickVideoView {
//            id: videoView
//            width: parent.width
//            height: parent.height
//            url: configMgr.videoTestFile
//            Component.onCompleted: {
//                videoView.setRWTimeout(configMgr.videoRWTimeout)
//            }
//            Component.onDestruction: {
//                videoView.closeStream()
//            }
//            Connections {
//                target: videoView
//                function onVidoeStateChanged(curVisible) {
//                    vidoeComp.visible = curVisible
//                }
//            }
//        }
    }

    FlightHeaderPanel {
        id: toolbar
        height: headHeight
        width: parent.width
        anchors.top: parent.top
        videoReplayListener: ()=>{
            videoView.replayStream()
        }
    }

    Item {
        id: footbar
        height: 1
        width: parent.width
        anchors.bottom: parent.bottom
        GcsDivider {
            anchors.bottom: footbar.top
            width: parent.width
            contrast: true
            spacing: 0
        }
    }

    FlightHUDLeftPanel {
        id: leftopt
        width: leftWidth
        height: winHeight - headHeight
        anchors.top: toolbar.bottom
        anchors.bottom: footbar.bottom
        anchors.left: parent.left
    }

    FlightHUDRightPanel{
        id: rightopt
        width: rightWidth
        height: winHeight - headHeight
        anchors.top: toolbar.bottom
        anchors.right: parent.right
        anchors.bottom: footbar.bottom
    }

    FlightMainDisplay {
        id: mainPart
        anchors {
            left: leftopt.right
            right: rightopt.left
            top: toolbar.bottom
            bottom: footbar.top
        }
        width:  middleWidth
        height: winHeight - headHeight
    }
}
