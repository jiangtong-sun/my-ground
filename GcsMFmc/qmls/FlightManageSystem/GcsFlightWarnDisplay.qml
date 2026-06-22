import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item{
    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property var detailWarnGroup: null
    property var currVehicle: commandMgr.currentVehicle
    property var levelAWarn: currVehicle ? currVehicle.warnLevelA.warnGroup : null
    property var levelBWarn: currVehicle ? currVehicle.warnLevelB.warnGroup : null
    property var levelCWarn: currVehicle ? currVehicle.warnLevelC.warnGroup : null
    property var feedbackModel: currVehicle ? currVehicle.feedback : null
    property int navItemWidth: ctrlWidth / 3
    property int warnItemWidth: ctrlWidth*3/10
    property int navItemHight: 36

    width: ctrlWidth
    height: ctrlHeight

    Item {
        id: warndetailA
        height: ctrlHeight
        width: navItemWidth
        anchors.left: parent.left
        anchors.top: parent.top

        GcsDivider{
            id: warn_left
            orientation: Qt.Horizontal
            anchors.top: warndetailA.top
            anchors.left: warndetailA.left
            anchors.leftMargin: 10
            width: parent.width/4
            contrast: true
            spacing: 20
        }

        GcsDivider{
            id: warn_right
            orientation: Qt.Horizontal
            anchors.top: warndetailA.top
            anchors.right: warndetailA.right
            anchors.rightMargin: 10
            width: parent.width/4
            contrast: true
            spacing: 20
        }
        GcsButton{
            height: navItemHight
            anchors.top: warndetailA.top
            anchors.left: warn_left.right
            anchors.right: warn_right.left
            normalColor: "transparent"
            text: qsTr("一级告警")
            // onClicked: {
            //     levelAWarn.clear()
            // }
        }
        Item {
            anchors.fill: parent
            anchors.margins: 10
            anchors.topMargin: navItemHight
            ListView{
                id: aWarnView
                clip: true
                model: levelAWarn
                anchors.fill: parent
                delegate: GcsNoticeButton {
                    id: noticeABtn
                    property var warninfo: object
                    display: Button.TextBesideIcon
                    height: navItemHight
                    width: navItemWidth
                    text: warninfo?warninfo.name:qsTr("")
                    iconState: warninfo?warninfo.show:false
                    textColor: GcsTheme.red
                    iconColor: GcsTheme.red
                    onClicked: {
                        warninfo.manual = true;
                    }
                }
            }
        }
    }

    Item {
        id: warndetailB
        height: ctrlHeight
        width: navItemWidth
        anchors.left: warndetailA.right
        anchors.top: parent.top

        GcsDivider{
            id: left
            orientation: Qt.Horizontal
            anchors.top: warndetailB.top
            anchors.left: warndetailB.left
            anchors.leftMargin: 10
            width: parent.width/4
            contrast: true
            spacing: 20
        }

        GcsDivider{
            id: right
            orientation: Qt.Horizontal
            anchors.top: warndetailB.top
            anchors.right: warndetailB.right
            anchors.rightMargin: 10
            width: parent.width/4
            contrast: true
            spacing: 20
        }

        GcsButton{
            height: navItemHight
            anchors.top: warndetailB.top
            anchors.left: left.right
            anchors.right: right.left
            normalColor: "transparent"
            text: qsTr("二级告警")
            // onClicked: {
            //     levelBWarn.clear()
            // }
        }
        ScrollView {
            anchors.fill: parent
            anchors.margins: 10
            anchors.topMargin: navItemHight
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            clip: true
            Column {
                Repeater{
                    model: levelBWarn
                    delegate: GcsNoticeButton {
                        id: noticeBBtn
                        property var warninfo: object
                        display: Button.TextBesideIcon
                        height: navItemHight
                        width: navItemWidth
                        text: warninfo?warninfo.name:qsTr("")
                        iconState: warninfo?warninfo.show:false
                        textColor: GcsTheme.orange
                        iconColor: GcsTheme.orange
                        onClicked: {
                            warninfo.manual = true;
                        }
                    }
                }
                Repeater{
                    model: levelCWarn
                    delegate: GcsNoticeButton {
                        id: noticeCBtn
                        property var warninfo: object
                        display: Button.TextBesideIcon
                        height: navItemHight
                        width: navItemWidth
                        text: warninfo?warninfo.name:qsTr("")
                        iconState: warninfo?warninfo.show:false
                        textColor: GcsTheme.yellow
                        iconColor: GcsTheme.yellow
                        onClicked: {
                            warninfo.manual = true;
                        }
                    }
                }
            }
        }
    }

    Item {
        id: back
        height: ctrlHeight
        width: navItemWidth
        anchors.left: warndetailB.right
        GcsDivider{
            id: back_left
            orientation: Qt.Horizontal
            anchors.top: back.top
            anchors.left: back.left
            anchors.leftMargin: 10
            width: parent.width/4
            contrast: true
            spacing: 20
        }
        GcsDivider{
            id: back_right
            orientation: Qt.Horizontal
            anchors.top: back.top
            anchors.right: back.right
            anchors.rightMargin: 10
            width: parent.width/4
            contrast: true
            spacing: 20
        }
        GcsButton {
            height: navItemHight
            anchors.top: back.top
            anchors.left: back_left.right
            anchors.right: back_right.left
            normalColor: "transparent"
            text: qsTr("指令回报")
            onClicked: {
                feedbackModel.clear()
            }
        }

        Item {
            id: feed_view
            anchors.fill: parent
            anchors.margins: 10
            anchors.topMargin: navItemHight
            GcsTableView {
                id:feed_tableview
                anchors.fill: parent
                tableModel: feedbackModel
                dataSource: feedbackModel.rowSource
                columnSource: feedbackModel.columnSource
                columnWidthProvider: function(column) {
                    var widthSet = [70, 180]
                    return widthSet[column]
                }
                Component.onCompleted: {
                    feed_tableview.setVerticalBarPos();
                }
            }
            Connections {
                target: commandMgr
                function onVerticalBarPos() {
                    feed_tableview.setVerticalBarPos();
                }
            }
        }
    }
}
