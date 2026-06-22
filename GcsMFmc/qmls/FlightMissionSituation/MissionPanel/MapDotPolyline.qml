import QtQuick 2.15
import QtQuick.Controls 2.15

import QtLocation 5.15
import QtPositioning 5.15

Item {
    id:mapDashLine
    anchors.fill: parent

    property var mapItem: null
    property color lineColor: "crimson"
    property var beginCoordinate: QtPositioning.coordinate()
    property var endCoordinate: QtPositioning.coordinate()
    property var lineDash: [4,4]
    property int lineWidth: 2

    Canvas{
        id:myCanvas
        anchors.fill: parent
        onPaint: {

            if(!mapDashLine.beginCoordinate.isValid ||
                    !mapDashLine.endCoordinate.isValid) {
                return
            }

            var ctx = getContext("2d")
            ctx.clearRect(0,0,myCanvas.width,myCanvas.height)
            ctx.strokeStyle = mapDashLine.lineColor
            ctx.lineWidth = mapDashLine.lineWidth
            ctx.setLineDash(mapDashLine.lineDash)

            ctx.beginPath()
            var beginPos = mapItem.fromCoordinate(mapDashLine.beginCoordinate,false)
            ctx.moveTo(beginPos.x,beginPos.y)
            var endPos = mapItem.fromCoordinate(mapDashLine.endCoordinate,false)
            ctx.lineTo(endPos.x,endPos.y)
            ctx.stroke()
            ctx.save()

            ctx.restore()
        }
    }

    onBeginCoordinateChanged: {
        update()
    }

    onEndCoordinateChanged: {
        update()
    }

    onLineDashChanged: {
        update()
    }

    onLineColorChanged: {
        update()
    }

    onLineWidthChanged: {
        update()
    }

    Connections{
        target: mapItem

        function onZoomLevelChanged(){
            update()
        }

        function onVisibleRegionChanged(){
            update()
        }
    }

    function update(){
        myCanvas.requestPaint()
    }
}
