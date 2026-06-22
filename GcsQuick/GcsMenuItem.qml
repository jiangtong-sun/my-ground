import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Templates 2.15 as T
import GcsQuick 1.0

T.MenuItem {
    property var selecListener: function(){ }
    id: control
    padding: 0
    spacing: 0
    implicitWidth: 100
    implicitHeight: 36
    contentItem: GcsToggleButton{
        text: control.text
        onClicked: {
            selecListener()
        }
    }
    background: Item {
        implicitWidth: 100
        implicitHeight: 36
        x: 1
        y: 1
    }
}
