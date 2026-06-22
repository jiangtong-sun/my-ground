import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickDataLinkElem 1.0
import GcsQuick 1.0

ControlBackground {
    id: base_bar
    property int vWidth: headHeight * 3/2
    property int vHeight: headHeight
    property var vehList: commandMgr.aircrafts
    property var mainLinker: commandMgr.mainLinker
    property var slaveLinker: commandMgr.slaveLinker
    property var taskInitCmd: commandMgr.taskInitItem
    property bool stayTop: false
    property string darkText : qsTr("Dark")
    property string lightText : qsTr("Light")
    property string stayTopText : qsTr("Sticky on Top")
    property string stayTopCancelText : qsTr("Sticky on Top cancelled")
    property color textColor: GcsTheme.fontPrimaryColor
    property var stayTopClickListener: function(){
        stayTop = !stayTop
        if (stayTop) {
            fmpwindow.flags = Qt.WindowStaysOnTopHint
        } else {
            fmpwindow.flags = Qt.FramelessWindowHint
        }
    }
    property var darkClickListener: function(){
        if(GcsTheme.dark){
            GcsTheme.darkMode = ThemeType.Light
        }else{
            GcsTheme.darkMode = ThemeType.Dark
        }
    }
    property var resetToolListener: function(type) {}

    Grid {
        id: linker
        anchors.right: parent.right
        height: headHeight
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

    Image {
        id: company
        width: headHeight
        height: headHeight-3
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 1
        anchors.leftMargin: 1
        source: "qrc:///res/aifly2.png"
    }

    FmpWindowToolBar{
        id: basebar
        anchors.left: company.right
        height: vHeight/2
        width: 200
    }

    RowLayout{
        id: setbar
        anchors.left: company.right
        anchors.top: basebar.bottom
        anchors.leftMargin: 10
        height: vHeight/2
        width: 200
        spacing: 10

        GcsIconButton{
            padding: 0
            Layout.minimumHeight:vHeight/2
            Layout.minimumWidth: 40
            verticalPadding: 0
            horizontalPadding: 0
            display: Button.TextOnly
            Layout.alignment: Qt.AlignVCenter
            text: "V" + configMgr.appVersion
        }

        GcsIconButton{
            id:btn_dark
            padding: 0
            Layout.minimumHeight:vHeight/2
            Layout.minimumWidth: 40
            verticalPadding: 0
            horizontalPadding: 0
            iconSource: ttfIcons["favorite"]
            Layout.alignment: Qt.AlignVCenter
            text: GcsTheme.dark ? base_bar.lightText : base_bar.darkText
            onClicked: darkClickListener()
        }

        GcsIconButton{
            id:btn_stay_top
            padding: 0
            Layout.minimumHeight:vHeight/2
            Layout.minimumWidth: 40
            verticalPadding: 0
            horizontalPadding: 0
            iconSource : ttfIcons["home"]
            Layout.alignment: Qt.AlignVCenter
            visible: true
            text: stayTop ? base_bar.stayTopCancelText : base_bar.stayTopText
            onClicked: stayTopClickListener()
        }
    }

    Row{
        id: normTool
        width: vWidth*6
        height: headHeight
        anchors.left: setbar.right
        ControlBackground {
            width: vWidth
            height: headHeight
            GcsText {
                anchors.centerIn: parent
                text: visibleFlightPath? qsTr("显示\n航线") : qsTr("隐藏\n航线")
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    visibleFlightPath = !visibleFlightPath;
                }
            }
        }
        ControlBackground {
            width: vWidth
            height: headHeight
            color: lineAreaMode==2?GcsTheme.primaryColor:GcsTheme.backgroundColor
            GcsText {
                anchors.centerIn: parent
                text: qsTr("测量\n距离")
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    resetToolListener(2)
                }
            }
        }
        ControlBackground {
            width: vWidth
            height: headHeight
            color: lineAreaMode==1?GcsTheme.primaryColor:GcsTheme.backgroundColor
            GcsText {
                anchors.centerIn: parent
                text: qsTr("测量\n面积")
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    resetToolListener(1)
                }
            }
        }
        ControlBackground {
            width: vWidth
            height: headHeight
            GcsText {
                anchors.centerIn: parent
                text: qsTr("清除\n航迹")
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    resetToolListener(-2)
                }
            }
        }
        ControlBackground {
            width: vWidth
            height: headHeight
            visible: selVehMission !== null ? true : false
            GcsText {
                anchors.centerIn: parent
                text: qsTr("飞机\n居中")
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    resetToolListener(-1)
                }
            }
        }
        Repeater {
            model: taskInitCmd?taskInitCmd.commands:null
            ControlBackground {
                property var initTask: object
                width: vWidth
                height: headHeight
                visible: selVehMission !== null ? true : false
                color: initTask.working ? GcsTheme.primaryColor: GcsTheme.backgroundColor
                GcsText {
                    anchors.centerIn: parent
                    text: initTask.vName
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        initTask.working = true
                        commandMgr.sendControlCommand(initTask)
                    }
                }
            }
        }
    }

    ControlBackground{
        id: history
        anchors.right: linker.left
        height: headHeight
        width: optPanelWidth
        GcsText {
            text: historyMessage
            textColor: GcsTheme.orange
            anchors.fill: parent
        }
    }

    GcsText{
        height: headHeight
        anchors.left: normTool.right
        anchors.right: history.left
        text: showNoticeMessage
    }
}
