import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property var tableModel
    property int depthPadding: 40
    property int cellHeight: 30
    property int cellWidth: 120
    property int currentRow: -1
    property color frameColor: GcsTheme.backgroundColor
    property color activeColor: GcsTheme.itemNormalColor
    property var selectListener : function(elem, row){ }
    property var removeListener : function(elem, row){ }
    property var updateListener : function(elem, row){ }

    function updateRowSelect() {
        currentRow = -1
    }

    id: control
    //color: frameColor
    Component.onDestruction: {
        table_view.contentY = 0
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: {
            if (mouse.button === Qt.RightButton){
                currentRow = -1
                selectListener(null, -1)
            }
            mouse.accepted = true
        }
    }

    TableView{
        id:table_view
        ScrollBar.vertical: GcsScrollBar{}
        boundsBehavior: Flickable.StopAtBounds
        model: tableModel
        clip: true
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        columnWidthProvider: function(column) { return column === 0? 40 : 110; }
        rowHeightProvider: function(row) { return control.cellHeight }

        delegate: MouseArea{
            property int column: model.column
            property var rowModel: model.rowModel
            property var unsaved: rowModel.unsaved
            property string displayVal: rowModel ? rowModel.get(column) : ""

            id: item_table
            hoverEnabled: true
            implicitHeight: 30
            implicitWidth: 30

            onClicked:
                (event)=>{
                    currentRow = model.row
                    selectListener(rowModel, model.row)
                    event.accepted = true
                }

            ControlBackground {
                anchors.fill: parent
                color: currentRow === model.row?GcsTheme.primaryColor:"transparent"
                GcsLoader {
                    sourceComponent: {
                        if(displayVal === qsTr("Action")) {
                            return comaction
                        } else {
                            return otherelem
                        }
                    }
                }
            }

            Component{
                id:comaction
                Item {
                    width: 110
                    height: cellHeight
                    GcsToggleButton{
                        width: 55
                        height: cellHeight
                        anchors.top: parent.top
                        anchors.left: parent.left
                        text: qsTr("删除")
                        onClicked: {
                            removeListener(rowModel, model.row)
                        }
                    }
                    GcsToggleButton{
                        width: 55
                        height: cellHeight
                        anchors.top: parent.top
                        anchors.right: parent.right
                        text: unsaved ? qsTr("保存") : qsTr("修改")
                        normalColor: unsaved ? GcsTheme.red : GcsTheme.itemNormalColor
                        onClicked: {
                            updateListener(rowModel, model.row)
                        }
                    }
                }
            }

            Component{
                id: otherelem
                GcsText {
                    height: cellHeight
                    text: String(displayVal)
                    elide: Text.ElideRight
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    wrapMode: Text.WrapAnywhere
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
