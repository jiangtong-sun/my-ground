import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtLocation 5.15
import GcsQuick 1.0

Item {
    id: root

    property bool down: false;
    property int selectIndex: 0
    property alias model: _listView.model
    property alias pressed: mouseArea.pressed
    property var selectListener : function(idx){ }

    GcsButton {
        id: contentItemId
        width: root.width;
        height: root.height
        text: model[selectIndex]
        txtHorzAlign: Text.AlignLeft
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onReleased: root.down = !root.down
        }
    }

    Item {
        width: root.width;
        height: root.height
        Item {
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 2
            anchors.right: parent.right
            anchors.rightMargin: 5
            clip: true
            width: 2*height;
            height: 7
            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: -parent.height/2
                width: Math.sqrt(parent.width*parent.width*2)/2;
                color: root.down ? "white" : "#4cbeff"
                height: width
                rotation: 45
            }
        }
    }

    Popup {
        y: contentItemId.height
        visible: root.down
        width: root.width
        height: root.count < 3 ? root.count * root.height : root.height * 3
        padding: 1

        ListView {
            id: _listView
            anchors.fill: parent
            clip: true

            delegate: GcsButton {
                txtHorzAlign: Text.AlignLeft
                width: root.width
                height: root.height
                text: modelData
                MouseArea {
                    id: delegateMouseArea
                    property bool isEnter: false
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: isEnter = true
                    onExited: isEnter = false
                    onClicked: {
                        root.down = false
                        root.selectIndex = index
                        selectListener(index)
                    }
                }
            }
        }
    }
}
