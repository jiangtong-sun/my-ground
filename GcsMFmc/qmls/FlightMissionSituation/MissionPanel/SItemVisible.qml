import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtLocation 5.15
import GcsQuick 1.0

Item {
    id: _root
    property bool editor: object.editor
    property int itemHeight: 30
    property int itemWidth: 480
    property var pItem: null
    property var vItem: null

    Component {
        id: visual
        Item {
            id: aelem
            height: itemHeight + (editor ? itemExpandH: 0)
            width: itemWidth

            GcsButton {
                anchors.top: parent.top
                height: itemHeight
                width: itemWidth
                text: "航点: " + (index  + 1)
            }

            MouseArea{
                anchors.fill: parent;
                onClicked: {
                    if (currObjectItem === object) {
                        editor = !editor
                        loaderEditorFlag = !loaderEditorFlag
                    } else {
                        if (currObjectItem) {
                            currObjectItem.editor = false
                        }

                        currObjectItem = object
                        currObjectItem.editor = true

                        if (lastSelWaypoint >= 0 && lastSelWaypoint < index){
                            detailPanelX = aelem.x
                            detailPanelY = aelem.y+itemHeight - itemExpandH
                        } else {
                            detailPanelX = aelem.x
                            detailPanelY = aelem.y+itemHeight
                        }

                        lastSelWaypoint = index
                        loaderEditorFlag = true
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        vItem = visual.createObject(pItem)
    }

    Component.onDestruction: {
        if (vItem) {
            vItem.parent = null
            vItem.destroy()
            vItem = null
        }
    }
}
