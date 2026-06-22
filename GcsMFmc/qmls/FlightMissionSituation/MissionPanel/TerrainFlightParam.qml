import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import CustomPlotItem 1.0
import GcsQuick 1.0

CustomPlotItem {
    id: plotItem

    function callRedrawTerrainStage(stage) {
        plotItem.redrawTerrainStage(stage)
    }

    function callRedrawTerrainLine(stage) {
        plotItem.redrawTerrainLine(stage)
    }

    Component.onCompleted: {
        initCustomPlot()
    }
}
