import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

ControlBackground {
    property var tableModel
    property int cellHeight: 30
    property int cellWidth: 30
    property int currTreeRow: -1
    property bool levelShow: false
    property color activeColor: GcsTheme.itemNormalColor
    property var showStageListener: function(model, select) { }
    property var eachColumnWidth: [100, 200, 100]

    function resetSelectRow() {
        currTreeRow = -1
    }

    function forceLayoutTable(){
        table_view.forceLayout()
    }

    function checkIconColor(val) {
        return GcsTheme.dark ? GcsTheme.White : GcsTheme.Black
    }

    id: control
    borderWidth: 0
    Component.onDestruction: {
        table_view.contentY = 0
    }

    TableView{
        id:table_view
        ScrollBar.vertical: GcsScrollBar{}
        boundsBehavior: Flickable.StopAtBounds
        model: tableModel
        clip: true
        anchors.fill: parent
        columnWidthProvider: function(column) {return eachColumnWidth[column]}
        rowHeightProvider: function(row) { return control.cellHeight }
        delegate: ControlBackground {
            property int column: model.column
            property var rowModel: model.rowModel
            property string displayVal: rowModel ? rowModel.get(column) : ""
            visible: rowModel ? (!rowModel.unsaved) : false
            implicitHeight: 30
            implicitWidth: 30
            Item{
                anchors.fill: parent
                GcsLoader {
                    sourceComponent: {
                        if(displayVal === qsTr("Action")) {
                            return firstelem
                        } else {
                            return otherelem
                        }
                    }
                }
            }
            Component{
                id:firstelem
                GcsToggleButton {
                    text: qsTr("选择")
                    width: 80
                    height: cellHeight-10
                    anchors.top: parent.top
                    anchors.topMargin: 5
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    onClicked: {
                        showStageListener(rowModel, !rowModel.checked)
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
