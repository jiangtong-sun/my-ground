import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

ControlBackground{
    id: _root
    property int navColumn: 3
    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property int navItemHeight: 34
    property int navItemWidth: (_root.width - 20)/navColumn
    property var firstModePart:  currVehicle ? currVehicle.flightCommonA: null
    property var secondModePart:  currVehicle ? currVehicle.flightCommonB: null
    property int firstRowCounts: firstModePart?firstModePart.dataModel.count: 0
    property int secondRowCounts: secondModePart?secondModePart.dataModel.count: 0
    property int navPanelHeight: (navItemHeight+1) * (Math.ceil(firstRowCounts/3)+1)
    property int othPanelHeight: ctrlHeight - navPanelHeight

    width: ctrlWidth
    height: ctrlHeight

    GcsFlightCommonPart{
        id: part1
        anchors.top: parent.top
        ctrlWidth: _root.ctrlWidth
        panelHeight: navPanelHeight
        modelPart: firstModePart
        height: navPanelHeight
    }

    GcsFlightCommonPart{
        id: part2
        anchors.top: part1.bottom
        ctrlWidth: _root.ctrlWidth
        panelHeight: othPanelHeight
        modelPart: secondModePart
        height: othPanelHeight
    }
}
