import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

ControlBackground {
    id: moveItem
    MouseArea {
        id: dragArea
        anchors.fill: parent
        drag {
            target: moveItem     // 拖拽目标
            axis: Drag.XAndYAxis // 允许水平和垂直拖拽
            minimumX: 0          // X轴最小范围
            minimumY: 0          // Y轴最小范围
            maximumX: moveItem.parent.width - moveItem.width   // X轴最大范围
            maximumY: moveItem.parent.height - moveItem.height // Y轴最大范围
        }
    }

    ControlBackground {
        width: 2
        height: parent.height
        anchors.right: parent.right
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.SizeHorCursor
            drag{ axis: Drag.XAxis }
            onMouseXChanged: {
                moveItem.width = Math.max(50, moveItem.width + mouseX)
            }
        }
    }

    ControlBackground {
        height: 2
        width: parent.width
        anchors.bottom: parent.bottom
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.SizeVerCursor
            drag{ axis: Drag.YAxis }
            onMouseYChanged: {
                moveItem.height = Math.max(50, moveItem.height + mouseY)
            }
        }
    }
}
