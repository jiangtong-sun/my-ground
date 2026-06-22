import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

ControlBackground {
    property var tableModel
    property var selectRowModel
    property int cellHeight: 30
    property int cellWidth: control.width/3
    property int currTreeRow: -1
    property bool actionShow: false
    property color activeColor: GcsTheme.itemNormalColor
    property var selectListener : function(type, elem){ }

    function forceLayoutTable(){
        table_view.forceLayout()
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

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        columnWidthProvider: function(column) { return cellWidth;}
        rowHeightProvider: function(row) { return control.cellHeight }

        delegate: MouseArea {
            property int column: model.column
            property var rowModel: model.rowModel
            property int depthVal: rowModel?rowModel.depth: 1
            property string displayVal: rowModel ? rowModel.get(column) : ""

            id: item_table
            hoverEnabled: true
            implicitHeight: 30
            implicitWidth: 30

            onDoubleClicked: {
                if(rowModel.isExpanded){
                    tableModel.collapse(row)
                } else {
                    tableModel.expand(row)
                }
                table_view.forceLayout()
            }

            onClicked:
                (event)=>{
                    currTreeRow = model.row
                    event.accepted = true
                    if (depthVal > 1) {
                        selectListener(rowModel.checked, rowModel)
                    }
                }

            Rectangle{
                anchors.fill: parent
                color:{
                    if(currTreeRow === model.row){
                        return control.activeColor
                    }
                    return control.color
                }
                GcsLoader {
                    sourceComponent: {
                        if(column === 0) {
                            return firstelem
                        } else if(displayVal === qsTr("Action")) {
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
                    height: cellHeight
                    width: cellWidth
                }
            }

            Component{
                id:firstelem
                Item {
                    width: cellWidth
                    height: cellHeight
                    GcsIconButton{
                        id: leftBtnIcon
                        visible: depthVal == 1
                        height: cellHeight
                        width: cellWidth
                        contentItem:GcsIcon{
                            iconSource: ttfIcons["arrowRightFilling"]
                            iconColor: GcsTheme.primaryColor
                            anchors.centerIn: parent
                            iconSize: 18
                        }
                    }
                    GcsIconButton {
                        visible: depthVal > 1
                        display: Button.TextOnly
                        text: String(displayVal)
                        height: cellHeight
                        width: cellWidth
                        iconSize: 18
                    }
                }
            }

            Component{
                id: otherelem
                GcsText {
                    id: dragItem
                    width: cellWidth
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
