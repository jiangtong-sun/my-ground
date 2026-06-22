import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtLocation 5.15

Item{
    id: _root
    property var _cmap: amap
    property var _itemVisual: null

    Component {
        id: normComp
        MapPolyline {
            id: polyline
            line.width: 2
            line.color: object.lineColor
            z: configMgr.zOrderWaypointLines
            path: [ object.spos, object.epos ]
        }
    }

    Component.onCompleted: {
        _itemVisual = normComp.createObject(_cmap)
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
