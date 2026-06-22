import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15

Map {
    id: _map
    opacity: 0.99
    zoomLevel: 15
    plugin: Plugin { name: "cdmcoc" }

    property int showUptFreq: 0
    signal mapLeftClicked(var position)
    signal mapRightClicked(var position)
    signal showMapPosition(var position)

    Component.onCompleted: {
        _map.center = configMgr.mapCenter
    }

    Connections {
        target: configMgr
        function onMapProviderChanged(value) {
            updateActiveMapType(value)
        }
        function onMapPositionChanged(value) {
            _map.center = configMgr.mapCenter
        }
    }

    onZoomLevelChanged: {
        if(_map.zoomLevel > 18){
            _map.zoomLevel = 18
        }
    }

    onCenterChanged: {
        configMgr.mapCenter = _map.center
    }

    function updateActiveMapType(type) {
        for (var i = 0; i < _map.supportedMapTypes.length; i++) {
            if(type === _map.supportedMapTypes[i].name) {
                _map.activeMapType = _map.supportedMapTypes[i]
                break;
            }
        }
    }

    onMapReadyChanged: {
        if (_map.mapReady) {
            console.log("active map: ", _map.activeMapType)
            for (var i = 0; i < _map.supportedMapTypes.length; i++) {
                console.log(_map.supportedMapTypes[i])
            }
        }
    }

    MouseArea{
        hoverEnabled: true
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPositionChanged: {
            if (showUptFreq++ % 5 === 0){
                var coord = _map.toCoordinate(Qt.point(mouseX, mouseY));
                showMapPosition(coord);
            }
        }
        onClicked: (mouse) => {
            if (mouse.button === Qt.RightButton){
                mapRightClicked(mouse)
            } else {
                mapLeftClicked(mouse)
            }
        }
    }

    WheelHandler {
        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
        rotationScale:  1 / 120
        property: "zoomLevel"
    }

    TapHandler {
        acceptedButtons: Qt.LeftButton
        onTapped: (eventPoint) => mapLeftClicked(eventPoint.position)
    }

    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: (eventPoint) => mapRightClicked(eventPoint.position)
    }
}
