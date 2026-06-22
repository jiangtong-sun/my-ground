import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property int partWidth: 100
    property int partHeight: 100
    property var currVehicle: commandMgr.currentVehicle
    property var feedbackModel: currVehicle ? currVehicle.feedback : null

    width: partWidth
    height: partHeight

    GroupRectangle {
        id: control
        width: partWidth
        height: partHeight
        anchors.fill: parent
        penColor: "#1e90ff" //GcsTheme.dividerColor
        textColor: leftRightColor
        text: qsTr("指令回报")
    }

    Item{
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 20
        anchors.topMargin: 28

        GcsTableView {
            id:feed_tableview
            anchors.fill: parent
            tableModel: feedbackModel
            dataSource: feedbackModel.rowSource
            columnSource: feedbackModel.columnSource
            columnWidthProvider: function(column) {
                var widthSet = [80, 180]
                return widthSet[column]
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

