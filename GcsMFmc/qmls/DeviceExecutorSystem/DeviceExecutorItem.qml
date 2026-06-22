import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

GcsHorizText {
    property var itemComp: null
    property string units:itemComp?itemComp.unit:""
    id: item
    leftWidth: item.width/2+10
    textDefColor:GcsTheme.primaryColor
    labelValue: itemComp?itemComp.itemName:qsTr("")
    Connections {
        target: itemComp
        function onUpdateColorValue(value, color) {
            item.textValue = (color > 0xf) ? qsTr("--") + units: value+ units;
            item.valueDefColor =  txtColorMap[color & 0xf]
        }
    }
}
