import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

ControlBackground {
    property var tableModel
    property int cellHeight: 30
    property int currTreeRow: -1
    property bool levelShow: false
    property color activeColor: GcsTheme.itemNormalColor
    property var selectStageListener : function(elem, row) {}
    property var removeListener : function(elem, row){ }
    property var updateListener : function(elem, row){ }
    property var columnProvider: function(column) {}

    function forceLayoutTable(){
        table_view.forceLayout()
    }

    function updateRowSelect() {
        currTreeRow = -1
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
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        columnWidthProvider: columnProvider
        rowHeightProvider: function(row) { return control.cellHeight }
        delegate: MouseArea {
            property int curRow: model.row
            property int column: model.column
            property var rowModel: model.rowModel
            property var unsaved: rowModel ? rowModel.unsaved : false
            property string displayVal: rowModel ? rowModel.get(column) : ""

            id: itemtable
            hoverEnabled: true
            implicitHeight: 30
            implicitWidth: 30
            acceptedButtons: Qt.RightButton|Qt.LeftButton
            GcsSimpleMenu {
                id: missionMenu
                implicitHeight: 70
                GcsMenuItem {
                    text: qsTr("文件加载")
                    selecListener: ()=> {
                        loadDialog.open()
                        missionMenu.close()
                    }
                }
                GcsMenuItem {
                    text: qsTr("航线另存")
                    selecListener: ()=> {
                        saveDialog.open()
                        missionMenu.close()
                    }
                }
            }

            onClicked: (mouse)=>{
                if (mouse.button === Qt.RightButton) {
                    selectCurrFLine = rowModel
                    missionMenu.popup()
                } else {
                    currTreeRow = curRow
                    selectStageListener(rowModel, curRow)
                    // if (event) { // mouse event
                    //     event.accepted = true
                    // }
                }
            }

            ControlBackground{
                anchors.fill: parent
                color: currTreeRow === curRow?GcsTheme.primaryColor:"transparent"
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
                id:firstelem
                Item{
                    height: cellHeight
                    width: columnProvider(column)
                    GcsIconButton{
                        height: cellHeight
                        anchors.fill: parent
                        text: String(displayVal)
                        anchors.centerIn: parent
                        verticalPadding: 1
                        horizontalPadding: 1
                        iconSize: 18
                        iconSource: {
                            if(rowModel && rowModel.checked) {
                                return ttfIcons["success"]
                            }
                            return ttfIcons["modular"]
                        }
                        iconColor: {
                            if (rowModel && rowModel.checked) {
                               return GcsTheme.orange
                            }
                            return GcsTheme.dark ? GcsTheme.White : GcsTheme.Black
                        }
                        onClicked: {
                            var state = rowModel.checked
                            rowModel.checked = !state
                        }
                    }
                }
            }
            Component{
                id:comaction
                Item {
                    height: cellHeight
                    width: columnProvider(column)
                    GcsToggleButton{
                        width: 60
                        height: cellHeight
                        anchors.top: parent.top
                        anchors.left: parent.left
                        visible: rowModel.name.length >0 ? true: false
                        text: qsTr("删除")
                        onClicked: {
                            removeListener(rowModel, curRow)
                        }
                    }
                    GcsToggleButton{
                        width: 60
                        height: cellHeight
                        anchors.top: parent.top
                        anchors.right: parent.right
                        text: unsaved ? qsTr("保存") : qsTr("修改")
                        normalColor: rowModel.unsaved ? GcsTheme.red : GcsTheme.itemNormalColor
                        onClicked: {
                            currTreeRow = curRow
                            updateListener(rowModel, curRow)
                        }
                    }
                }
            }
            Component{
                id: otherelem
                GcsText {
                    width: columnProvider(column)
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
