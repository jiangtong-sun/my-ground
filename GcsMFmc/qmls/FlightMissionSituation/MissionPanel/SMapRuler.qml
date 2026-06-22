import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15
import GcsQuick 1.0

MapItemGroup{
    id: control

    MapPolyline {
        id: item_line
        line.width: 2
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
            id: itmdelegate
            sourceItem: ControlBackground {
                color: GcsTheme.primaryColor
                width: 14
                height: 14
                radius: 7
                ControlBackground{
                    anchors.left: parent.right
                    anchors.top: parent.bottom
                    width: item_text.width+5+5+14+5
                    height: item_text.height+10
                    GcsText {
                        x: 5
                        id: item_text
                        anchors.verticalCenter: parent.verticalCenter
                        text: index <= 0
                              ?("起点")
                              :(Math.round(itmdelegate.coordinate.distanceTo(item_line.coordinateAt(index-1))) +" m")
                    }
                    Item{
                        width: 14
                        height: 14
                        anchors.rightMargin: 5
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        GcsText {
                            color: selBtnColor
                            anchors.centerIn: parent
                            text: qsTr("+")
                            rotation: 45
                        }
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                item_line.removeCoordinate(index)
                                item_model.remove(index)
                            }
                        }
                    }
                }
            }
            coordinate{
                latitude: latval
                longitude: lngval
            }
            anchorPoint: Qt.point(sourceItem.width/2, sourceItem.height/2)
        }
    }

    function appendPoint(coord){
        item_model.append({
                "latval":coord.latitude,
                "lngval":coord.longitude});
        item_line.addCoordinate(coord);
    }

    function closePath(){
        item_line.path = []
        item_model.clear()
    }
}
