import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15
import GcsQuick 1.0

MapItemGroup{
    id: control

    property alias centerPos: areaitem.coordinate
    property bool showArea: false
    property double areaValue: 0

    MapQuickItem {
        id: areaitem
        visible: showArea
        anchorPoint.x: sourceItem.width / 2
        anchorPoint.y: sourceItem.height / 2
        sourceItem: ControlBackground{
            height: 32
            width: 100
            GcsText {
                anchors.verticalCenter: parent.verticalCenter
                text: control.areaValue + " m^2"
                padding: 10
            }
        }
    }

    MapPolygon{
        color: selAreaColor;
        border.width: 0
        opacity: 0.5
        path: item_line.path
    }

    MapPolyline{
        id: item_line
        line.width: 1
        line.color: selBtnColor
    }

    MapItemView{
        id: item_view
        add: Transition {}
        remove: Transition {}
        model: ListModel{
            id: item_model
        }
        delegate: MapQuickItem{
            id: ietm_delegate
            sourceItem: ControlBackground {
                color: GcsTheme.primaryColor
                width: 14
                height: 14
                radius: 7
            }
            coordinate{
                latitude: latitudeval
                longitude: longitudeval
            }
            anchorPoint: Qt.point(sourceItem.width/2, sourceItem.height/2)
        }
    }

    function appendPoint(coord){
        item_model.append({"latitudeval":coord.latitude,"longitudeval":coord.longitude})
        item_line.addCoordinate(coord)
        if(item_line.pathLength()<3){
            return;
        }

        showArea = true
        var latitude = 0
        var longitude = 0
        control.areaValue = getPolygonArea(item_line.path)
        for(let i = item_line.pathLength() - 1; i >= 0; i-- ){
            latitude += item_line.path[i].latitude
            longitude += item_line.path[i].longitude
        }

        centerPos.latitude = latitude/item_line.pathLength()
        centerPos.longitude = longitude/item_line.pathLength()
    }

    function closePath(){
        item_line.path=[]
        item_model.clear()
        areaitem.destroy()
    }

    function getPolygonArea(path){
        let area_count = 0
        let path_len = path.length
        if(path_len < 3) {
            return area_count
        }

        let data_list=[]
        let sJ = 6378137
        let Hq = 0.017453292519943295
        let c = sJ * Hq

        for(let i = 0;i < path_len-1; i++){
            let h = path[i]
            let k = path[i+1]
            let u = h.longitude*c*Math.cos(h.latitude*Hq)
            let hhh = h.latitude*c
            let v = k.longitude*c*Math.cos(k.latitude*Hq)
            area_count += (u*k.latitude*c-v*hhh)
        }

        let eee = path[path_len-1].longitude*c*Math.cos(path[path_len-1].latitude*Hq)
        let k = path[0].longitude*c*Math.cos(path[0].latitude*Hq)
        let g2 = path[path_len-1].latitude*c

        area_count += eee*path[0].latitude*c-k*g2

        return Math.round(Math.abs(area_count)/2)
    }
}
