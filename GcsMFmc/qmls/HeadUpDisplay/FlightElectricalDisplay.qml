import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickHBusbarElemNode 1.0
import GcsQuick 1.0

Item {
    property int partWidth: 100
    property int partHeight: 100
    property int itemWidth: (partWidth - 30)
    property var eimcMode: commandMgr.currentVehicle.eimcMode
    property var batteryMode: commandMgr.currentVehicle.batteryMode
    property int emicCounts: eimcMode ? eimcMode.dataModel.count : 1
    property int batteryCounts: batteryMode ? batteryMode.dataModel.count : 1
    property int itemHeight: (partHeight - 40)/(emicCounts + batteryCounts )

    width: partWidth
    height: partHeight

    GroupRectangle {
        id: control
        width: partWidth
        height: partHeight
        anchors.fill: parent
        penColor: "#1e90ff" //GcsTheme.dividerColor
        textColor: leftRightColor
        text: qsTr("EMIC & 电池")
    }

    GridView {
        id: eimc
        clip: true
        cellWidth: itemWidth
        cellHeight: itemHeight
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.leftMargin: 10
        width: itemWidth
        height: itemHeight * emicCounts
        model: eimcMode?eimcMode.dataModel:null
        delegate: QuickHBusbarElemNode {
            property var emicObj: object
            fontColor: GcsTheme.primaryColor
            warnColor:  GcsTheme.orange
            color: GcsTheme.primaryColor
            dataNode: emicObj
            width: itemWidth
            height: itemHeight
        }
    }

    GridView {
        id: battery
        clip: true
        cellWidth: itemWidth
        cellHeight: itemHeight
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.leftMargin: 10
        width: itemWidth
        height: itemHeight * batteryCounts
        model: batteryMode?batteryMode.dataModel:null
        delegate: QuickHBusbarElemNode {
            property var batteryObj: object
            fontColor: GcsTheme.primaryColor
            color: GcsTheme.primaryColor
            warnColor:  GcsTheme.orange
            dataNode: batteryObj
            width: itemWidth
            height: itemHeight
        }
    }
}

