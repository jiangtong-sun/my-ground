import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickExecutorElem 1.0
import QuickActuatorElem 1.0
import GcsQuick 1.0

Item{
    property int hudDHeight: firstHeight/3
    property int hudUHeight: hudDHeight*2
    property var currVehicle: commandMgr.currentVehicle
    property real rotateDirect: configMgr.hudReferLevel

    Component {
        id: xfhudcenter
        XFFlightHUDCenterPanel{
        }
    }


    Item {
        anchors.fill: parent
        anchors.leftMargin: middleWidth/5
        anchors.rightMargin: middleWidth/5
        width: middleWidth*3/5
        height: firstHeight
        GcsLoader {
            anchors.fill: parent
            sourceComponent: {
                return xfhudcenter;
            }
        }
    }

    Item {
        id: leftside
        width: middleWidth/3
        height: firstHeight
        anchors.left: parent.left
        FlightHeadUpMode {
            height: hudUHeight
            width: leftWidth
            anchors.top: parent.top
            anchors.left: parent.left
        }
    }

    Item {
        id: rightside
        width: middleWidth/3+30
        height: firstHeight
        anchors.right: parent.right

        FlightHeadUpWinds {
            height: hudUHeight
            width: rightWidth
            anchors.top: parent.top
            anchors.right: parent.right
        }

        Component{
            id: xfvehFlap
            XFFlightFlapDisplay {
            }
        }

        Item {
            height: hudDHeight
            width: middleWidth/3+30
            anchors.bottom: parent.bottom
            GcsLoader {
                anchors.fill: parent
                sourceComponent: {
                    return xfvehFlap
                }
            }
        }
    }
}


