import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property int partWidth: 100
    property int partHeight: 100
    property var currVehicle: commandMgr.currentVehicle
    property var levelAWarn: currVehicle ? currVehicle.warnLevelA.warnGroup : null
    property var levelBWarn: currVehicle ? currVehicle.warnLevelB.warnGroup : null
    property var levelCWarn: currVehicle ? currVehicle.warnLevelC.warnGroup : null

    width: partWidth
    height: partHeight

    GroupRectangle {
        id: control
        width: partWidth
        height: partHeight
        anchors.fill: parent
        penColor: "#1e90ff" //GcsTheme.dividerColor
        textColor: leftRightColor
        text: qsTr("告警")
    }

    Item{
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 20
        anchors.topMargin: 28

        ScrollView {
            anchors.fill: parent
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            clip: true
            Column {
                Repeater{
                    model: levelAWarn
                    delegate: GcsNoticeButton {
                        id: noticeABtn
                        property var warninfoa: object
                        display: Button.TextBesideIcon
                        height: 36
                        width: partWidth-40
                        text: warninfoa?warninfoa.name:qsTr("")
                        iconState: warninfoa?warninfoa.show:false
                        textColor: GcsTheme.red
                        iconColor: GcsTheme.red
                        onClicked: {
                            warninfoa.manual = true;
                        }
                    }
                }
                Repeater{
                    model: levelBWarn
                    delegate: GcsNoticeButton {
                        id: noticeBBtn
                        property var warninfob: object
                        display: Button.TextBesideIcon
                        height: 36
                        width: partWidth-40
                        text: warninfob?warninfob.name:qsTr("")
                        iconState: warninfob?warninfob.show:false
                        textColor: GcsTheme.orange
                        iconColor: GcsTheme.orange
                        onClicked: {
                            warninfob.manual = true;
                        }
                    }
                }
                Repeater{
                    model: levelCWarn
                    delegate: GcsNoticeButton {
                        id: noticeCBtn
                        property var warninfoc: object
                        display: Button.TextBesideIcon
                        height: 36
                        width: partWidth-40
                        text: warninfoc?warninfoc.name:qsTr("")
                        iconState: warninfoc?warninfoc.show:false
                        textColor: GcsTheme.yellow
                        iconColor: GcsTheme.yellow
                        onClicked: {
                            warninfoc.manual = true;
                        }
                    }
                }
            }
        }

        // ListView{
        //     id: aWarnView
        //     clip: true
        //     model: levelAWarn
        //     anchors.fill: parent
        //     delegate: GcsNoticeButton {
        //         property var warninfo: object
        //         id: noticeABtn
        //         height: 36
        //         width: partWidth-40
        //         display: Button.TextBesideIcon
        //         text: warninfo?warninfo.name:qsTr("")
        //         iconState: warninfo?warninfo.show:false
        //         textColor: GcsTheme.primaryColor
        //         iconColor: GcsTheme.red
        //     }
        // }
    }
}
