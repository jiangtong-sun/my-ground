import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QmlQCustomPlot 1.0
import FlightLanding 1.0
import GcsQuick 1.0

Item {
    id: control
    anchors.fill: parent

    property real lateral: 0
    property real distance: 0
    property bool landFlag: false

    // remove curve data
    function resetCurveData() {
        landingCom.clearCurveData("2");
        lateralCmp.clearCurveData("1");
    }

    Connections {
        target: currVehicle?currVehicle.DTG: null
        function onUpdateColorValue(val, va){
            distance = parseFloat(val === "NA" ? "0" : val)
        }
    }

    Connections {
        target: currVehicle?currVehicle.LateralOffset: null
        function onUpdateColorValue(val, va){
            lateral = parseFloat(val === "NA" ? "0" : val)
            if (landFlag && distance > 0) {
                lateralCmp.addLateralPosValue("1", distance, lateral)
            }
        }
    }

    Connections {
        target: currVehicle?currVehicle.HeightOffset: null
        function onUpdateColorValue(val, va){
            var high = parseFloat(val === "NA" ? "0" : val)
            if (landFlag && distance > 0) {
                landingCom.addGraphHighValue("2", distance, high)
            }
        }
    }

    Item {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 10
        height: parent.height
        width: (parent.width-30)/3
        SimplePlot {
            id: lateralCmp
            rotation: -90
            anchors.centerIn: parent
            backgroundColor: "whitesmoke"
            width: parent.height-6
            height: parent.height*2
            Component.onCompleted: {
                yAxis.setRange(-30, 30)
                xAxis.setRange(0, 100)
                xAxis.transparentAxis()
                addGraph("1", "green")
                addGraph("2", "darksalmon")
                addGraph("3", "darksalmon")
                drawSpecialLine(22.5, "3");
                drawSpecialLine(-22.5, "2");
            }
        }
    }

    Item {
        anchors.left: parent.left
        anchors.top: parent.top
        height: parent.height-3
        width: (parent.width-30)*2/3
        SimplePlot {
            id: landingCom
            anchors.fill: parent
            anchors.topMargin: 3
            backgroundColor: "whitesmoke"
            Component.onCompleted: {
                yAxis.setRange(0, 400)
                xAxis.setRange(0, 10000)
                addGraph("1", "red")
                addGraph("2", "green")
                addLandSetpoints("1", configMgr.landConfig)
            }
        }
    }
}
