import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Templates 2.15 as T
import GcsQuick 1.0

T.Menu {
    id: control
    implicitWidth: 100
    implicitHeight: 36
    margins: 0
    overlap: 1
    spacing: 0
    delegate: GcsMenuItem { }
    contentItem: ListView {
        implicitHeight: contentHeight
        model: control.contentModel
        clip: true
        currentIndex: control.currentIndex
        ScrollBar.vertical: GcsScrollBar{}
    }
    background: ControlBackground {
        implicitWidth: control.implicitWidth
        implicitHeight: control.implicitHeight
        radius: 5
    }
}
