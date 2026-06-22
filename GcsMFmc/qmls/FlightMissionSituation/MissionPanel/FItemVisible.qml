import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtLocation 5.15
import GcsQuick 1.0

Item{
    id: _root

    property var _cmap: null
    property bool _itemVisualShowing: false
    property var _itemVisual

    signal clicked(var seq)

    function hideItemVisuals() {
        if (_itemVisualShowing) {
            _itemVisualShowing = false
            _itemVisual.parent = null
            _cmap.removeMapItem(_itemVisual)
            _itemVisual.destroy()
        }
    }

    function showItemVisuals() {
        _itemVisual = visual.createObject(_cmap)
        _cmap.addMapItem(_itemVisual)
        _itemVisualShowing = true
    }

    Component {
        id: visual
        MapQuickItem {
            id: _itm
            visible: visibleFlightPath
            z: configMgr.zOrderMapItems
            coordinate: object.pos
            anchorPoint.x: 16
            anchorPoint.y: 16
            sourceItem: GcsFilledButton {
                width: 32
                height: 32
                text: object.seq
            }
        }
    }

    Component.onCompleted: {
        showItemVisuals()
    }

    Component.onDestruction: {
        hideItemVisuals()
    }
}
