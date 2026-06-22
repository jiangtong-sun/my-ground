import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import GcsQuick 1.0

Item{
    id: _root

    property var _cmap: null
    property var _itemVisual: null
    property var waypointObj: object

    function itemBackground() {
        return (visualDrag === index || lastSelWaypoint == index)
                ? GcsTheme.orange
                : GcsTheme.primaryColor
    }

    Component {
        id: visual
        MapQuickItem {
            id: _itm
            anchorPoint.x: 16
            anchorPoint.y: 16
            coordinate: object.pos
            z: configMgr.zOrderMapItems
            sourceItem: ControlBackground {
                borderColor: GcsTheme.orange
                color: itemBackground();
                width: 32
                height: 32
                radius: 8
                GcsText{
                    anchors.centerIn: parent
                    text: index
                }
                MouseArea {
                    enabled: currStage.unsaved
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    drag.target: waypointObj.canDrag ? _itm : null
                    anchors.fill: parent
                    onReleased: (mouse) => {
                        waypointObj.updatePosition(_itm.coordinate)
                        if (lastSelWaypoint != index) {
                            currObjectItem = null
                        }
                        visualDrag = -1
                    }
                    onPressed: {
                        if (waypointObj.canDrag) {
                            visualDrag = index
                        }
                    }
                    onPositionChanged: (mouse) => {
                        currObjectItem = waypointObj
                        waypointObj.updatePosition(_itm.coordinate)
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        _itemVisual = visual.createObject(_cmap)
        _cmap.addMapItem(_itemVisual)
    }

    Component.onDestruction: {
        if (_itemVisual) {
            _itemVisual.parent = null
            _cmap.removeMapItem(_itemVisual)
            _itemVisual.destroy()
            _itemVisual = null
        }
    }
}
