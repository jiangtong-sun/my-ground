import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property int itemHeight: 36
    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property int selectItem: -1
    property int selectRows: -1
    property var playColumn: ["ID", "开始", "时长", "遥测", "遥控", "媒体", "其他", "操作"]
    property var playColWidth: [1, 3, 2, 1, 1, 1, 1, 1]
    property var playColWidth2: [1, 3, 2, 1, 1, 1, 1]
    property var speedColumn: ["0.5", "1", "2", "5"]
    property real colWidth: (ctrlWidth-20)/11
    property int dataRWState: dataManager.rwState
    property string selectFile: ""

    id: control

    Connections{
        target: dataManager
        function onDataThreadExited() {
            dataManager.stopReplay()
            selectRows = -1;
        }
        function onSlideValueChanged(val) {
            slide.value = val
        }
    }

    GcsDivider {
        orientation: Qt.Horizontal
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 10
        width: parent.width/3
        spacing: 20
    }

    GcsDivider{
        orientation: Qt.Horizontal
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 10
        width: parent.width/3
        spacing: 20
    }

    GcsText{
        height: itemHeight
        anchors.right: parent.right
        anchors.rightMargin: parent.width/3
        anchors.left: parent.left
        anchors.leftMargin: parent.width/3
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: qsTr("数据记录《》数据回放")
    }

    Row {
        id: headerRow
        anchors.top: parent.top
        anchors.topMargin: itemHeight
        anchors.left: parent.left
        anchors.leftMargin: 10
        width: parent.width - 20
        height: itemHeight
        Repeater {
            model: playColumn
            GcsToggleButton {
                text: modelData
                height: itemHeight
                width: playColWidth[index] * colWidth
            }
        }
    }

    Item{
        id: playbutton
        width: itemHeight*2
        height: itemHeight*2
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        ControlBackground {
            anchors.fill: parent
            anchors.margins: 5
            GcsToggleButton {
                anchors.fill: parent
                borderColor: {
                    if(dataRWState === 1 || dataRWState === 2){
                        return GcsTheme.orange
                    }
                    return GcsTheme.primaryColor
                }
                text: {
                    if(dataRWState === 1) {
                        return qsTr("停止\n回放")
                    }
                    if(dataRWState === 2) {
                        return qsTr("停止\n记录")
                    }
                    if (selectRows >= 0){
                        return qsTr("回放\n数据")
                    }
                    return qsTr("保存\n数据")
                }
                onClicked: {
                    if (dataRWState == 2) {
                        dataManager.stopRecord()
                        return;
                    }
                    if (dataRWState == 1) {
                        dataManager.stopReplay()
                        return;
                    }

                    if (selectRows >= 0){
                        dataManager.startReplay(selectFile, selectRows)
                    } else {
                        dataManager.startRecord()
                    }
                }
            }
        }
    }


    Item {
        id: typecontrol
        height: itemHeight
        width: ctrlWidth*2/5
        anchors.right: playbutton.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: itemHeight
        Row{
            anchors.fill: parent
            GcsRadioButton{
                height: itemHeight
                width: ctrlWidth/10
                text: qsTr("遥测")
                onClicked: {
                    dataManager.hasTelemetry = !dataManager.hasTelemetry;
                }
            }
            GcsRadioButton{
                height: itemHeight
                width: ctrlWidth/10
                text: qsTr("遥控")
                onClicked: {
                    dataManager.hasCtlAdjust = !dataManager.hasCtlAdjust;
                }
            }
            GcsRadioButton{
                height: itemHeight
                width: ctrlWidth/10
                text: qsTr("媒体")
                onClicked: {
                    dataManager.hasMedia = !dataManager.hasMedia;
                }
            }
            GcsRadioButton{
                height: itemHeight
                width: ctrlWidth/10
                text: qsTr("其他")
                onClicked: {
                    dataManager.hasWeather = !dataManager.hasWeather;
                }
            }
        }
    }

    Item {
        id: speedcontrol
        height: itemHeight
        width: ctrlWidth*2/5
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: itemHeight
        anchors.leftMargin: 10
        Row{
            anchors.fill: parent
            Repeater{
                model: speedColumn
                GcsRadioButton{
                    height: itemHeight
                    width: ctrlWidth/10
                    text: modelData+"x"
                    checked: selectItem === index? true: false
                    clickListener: () => {
                        var rm = parseFloat(speedColumn[index])
                        dataManager.playSpeed = rm
                        selectItem = index
                    }
                }
            }
        }
    }

    Item {
        height: itemHeight
        anchors.left: parent.left
        anchors.right: playbutton.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 10
        GcsText {
            id: leftTm
            width: colWidth
            height: itemHeight
            anchors.left: parent.left
            text: qsTr("00:00:00")
        }
        GcsText {
            id: rightTm
            width: colWidth
            height: itemHeight
            anchors.right: parent.right
            text: qsTr("00:00:00")
        }
        Slider{
            id: slide
            from: 0
            to: 100
            value: 0
            height: itemHeight
            anchors.left: leftTm.right
            anchors.right: rightTm.left
            onValueChanged: {
                dataManager.slideValue = value
            }
        }
    }

    ListView {
        id: viewdata
        anchors.top: headerRow.bottom
        anchors.bottom: parent.bottom
        anchors.bottomMargin: itemHeight*2
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        height: control.height - 3 * itemHeight
        width: parent.width - 20
        clip: true

        model: dataManager.visualDataList
        delegate: ControlBackground{
            property var itemObject: object
            color: selectRows === index ? GcsTheme.itemGrayColor : GcsTheme.backgroundColor
            width: viewdata.width
            height: itemHeight
            MouseArea {
                anchors.fill: parent
                Row {
                    anchors.fill: parent
                    Repeater{
                        model: playColWidth2
                        GcsText {
                            height: itemHeight
                            text: itemObject.columnValue(index)
                            width: playColWidth[index] * colWidth
                        }
                    }
                    GcsButton{
                        text: qsTr("导出")
                        normalColor: selectRows === index ? GcsTheme.primaryColor: GcsTheme.backgroundColor
                        height: itemHeight
                        width: colWidth
                        onClicked: {

                        }
                    }
                }
                onDoubleClicked: {
                    selectRows = -1
                    selectFile = ""
                }
                onClicked: {
                    if (dataRWState === 0){
                        selectRows = index
                        selectFile = itemObject.columnValue(1)
                        rightTm.text = itemObject.columnValue(2)
                    }
                }
            }
        }
    }
}

