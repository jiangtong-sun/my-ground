import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtLocation 5.15

Item{
    id: _root
    property var _cmap: amap
    property var _itemVisual: null

    Component {
        id: dotComp
        MapDotPolyline {
            id: polyline
            mapItem: _cmap
            lineColor: object.lineColor
            z: configMgr.zOrderWaypointLines
            endCoordinate: object.epos
            beginCoordinate: object.spos
        }
    }

    Component.onCompleted: {
        _itemVisual = dotComp.createObject(_cmap)
    }

    Component.onDestruction: {
        if (_itemVisual) {
            _itemVisual.parent = null
            _itemVisual.destroy()
            _itemVisual = null
        }
    }
}
