import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Popup {
    id: root
    x: parent.width/2 - root.width/2
    y: parent.height/2 - root.height/2

    property var stageModel
    property var currStage
    property point clickPoint: "0,0"
    property string warnBorderColor: "#F0A020"
    property string headerText: qsTr("保存航段信息")

    width: 300
    height: 150
    modal: true
    focus: true

    background: ControlBackground {
        anchors.fill: parent
        radius: 8

        GcsButton{
            id: header
            height: 30
            width: parent.width
            text: headerText
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                onPressed: {
                    clickPoint  = Qt.point(mouse.x, mouse.y)
                }
                onPositionChanged: {
                    var offset = Qt.point(mouse.x - clickPoint.x, mouse.y - clickPoint.y)
                    setDlgPoint(offset.x, offset.y)
                }
            }
        }

        ColumnLayout {
            id: column
            height: 40
            anchors.margins: 15
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: header.bottom
            spacing: 10

            Row {
                height: 30
                width: parent.width
                GcsText{
                    text: "航段名称："
                    width: parent.width/3
                    height: parent.height
                }
                ControlBackground{
                    id: nameArea
                    width: parent.width*2/3
                    height: parent.height
                    GcsTextBox{
                        id: nameAreaInput
                        anchors.fill: parent
                        text: currStage ? currStage.name : ""
                    }
                }
            }
        }

        GcsToggleButton {
            anchors.topMargin: 20
            anchors.leftMargin: 60
            anchors.left: parent.left
            anchors.top: column.bottom
            width: 60
            height: 30
            flat: true
            highlighted: true
            text: qsTr("确定")
            onClicked: {
                if (nameAreaInput.text.length > 0){
                    if (stageModel.saveStageLine(nameAreaInput.text)){
                        toastShow(qsTr("保存航线段成功"), 2000)
                        resetFlineSelect()
                        root.close()
                    } else {
                        headerText = qsTr("航段名冲突")
                        header.normalColor = warnBorderColor
                    }
                }
                else {
                    if (nameAreaInput.length <= 0){
                        nameArea.borderColor = warnBorderColor
                    }
                }
            }
        }

        GcsToggleButton {
            anchors.topMargin: 20
            anchors.rightMargin: 60
            anchors.right: parent.right
            anchors.top: column.bottom
            width: 60
            height: 30
            flat: true
            highlighted: true
            text: qsTr("取消")
            onClicked: {
                root.close()
            }
        }
    }

    function setDlgPoint(dlgX ,dlgY)
    {
        if(root.x + dlgX < 0)
        {
            root.x = 0
        }
        else if(root.x + dlgX > root.parent.width - root.width)
        {
            root.x = root.parent.width - root.width
        }
        else
        {
            root.x = root.x + dlgX
        }
        if(root.y + dlgY < 0)
        {
            root.y = 0
        }
        else if(root.y + dlgY > root.parent.height - root.height)
        {
            root.y = root.parent.height - root.height
        }
        else
        {
            root.y = root.y + dlgY
        }
    }
}
