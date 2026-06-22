import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    property bool showHeader: false
    property var dataSource
    property var columnSource
    property int rowHeight: 30
    property int rowHoverIndex: -1
    property alias tableModel: table_view.model
    property color columnColor: GcsTheme.primaryColor
    property color selectedColor: GcsTools.withOpacity(GcsTheme.frameColor,0.7)
    property var columnWidthProvider: function(column) {return 100 }

    function setVerticalBarPos() {
        tableVBar.setPosition(1-tableVBar.size)
    }

    id:control

    Row {
        id: header
        visible: showHeader
        height: rowHeight
        width: parent.width
        Repeater {
            model: tableModel.columnSource
            delegate: GcsText{
                horizontalAlignment: Text.AlignLeft
                width: columnWidthProvider(index)
                text: modelData
            }
        }
    }

    Component{
        id:com_table_delegate
        MouseArea{
            property var elemState: model.elemState
            property var display : model.elemNode
            hoverEnabled: true
            implicitHeight: rowHeight
            onEntered: {
                rowHoverIndex = row
            }
            onExited: {
                rowHoverIndex = -1
            }
            Rectangle{
                anchors.fill: parent
                color: rowHoverIndex === row ? selectedColor: "transparent"
                GcsText {
                    id:item_text
                    anchors.fill: parent
                    text: String(display)
                    elide: Text.ElideRight
                    textColor: elemState?GcsTheme.orange:GcsTheme.fontPrimaryColor
                    wrapMode: Text.WrapAnywhere
                    horizontalAlignment: Text.AlignLeft
                }
            }
        }
    }

    TableView {
        id:table_view
        anchors.fill: parent
        anchors.topMargin: showHeader ? rowHeight : 0
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: GcsScrollBar {
            id: tableVBar
            active: true
        }
        columnWidthProvider: control.columnWidthProvider
        rowHeightProvider: function(row) { return rowHeight; }
        model: tableModel
        clip: true

        delegate: com_table_delegate
    }
}
