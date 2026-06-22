import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

ControlBackground {
    property int ctrlWidth: 400
    property int ctrlHeight: 400
    property color cmdFontColor: GcsTheme.green
    property int eachItemWidth: ctrlWidth/7
    property var engineMode: currVehicle ? currVehicle.engineMode : null
    property var engineFirst: engineMode ? engineMode.firstNode : null
    property var vehfeedback: currVehicle ? currVehicle.feedback: null
    property var attiLeftShow: currVehicle ? currVehicle.attiLeftShow: null
    property var attiCenterShow: currVehicle ? currVehicle.attiCenterShow: null
    property var attiRightShow: currVehicle ? currVehicle.attiRightShow: null
    property var flapRundder: currVehicle?currVehicle.flapRunnder:null
    property var uptDataSrc: flapRundder?flapRundder.deftElemNode:null

    width: ctrlWidth
    height: ctrlHeight

    Item {
        id: actuator
        anchors.fill: parent
        anchors.topMargin: 100
        anchors.bottomMargin: 10
        // 西飞飞机模型俯视角面
        XFDevActuatorDisplay {
            anchors.fill: parent
        }
    }

    Item {
        id: engineDisp
        anchors.top: parent.top
        anchors.left: parent.left
        width: eachItemWidth*2
        height: 200
        Column{
            // 西飞发动机引擎信息
            anchors.fill: parent
            anchors.leftMargin: 2
            DeviceExecutorItem {
                height: 25
                width: eachItemWidth*2
                itemComp: engineFirst.engineLevel
            }
            DeviceExecutorItem {
                height: 25
                width: eachItemWidth*2
                itemComp: engineFirst.throttle
            }
            DeviceExecutorItem {
                height: 25
                width: eachItemWidth*2
                itemComp: engineFirst.inletTotalTmp
            }
            DeviceExecutorItem {
                height: 25
                width: eachItemWidth*2
                itemComp: engineFirst.exhuastTmp
            }
            DeviceExecutorItem {
                height: 25
                width: eachItemWidth*2
                itemComp: engineFirst.oilPressure
            }
            DeviceExecutorItem {
                height: 25
                width: eachItemWidth*2
                itemComp: engineFirst.oilTmp
            }
            DeviceExecutorItem {
                height: 25
                width: eachItemWidth*2
                itemComp: engineFirst.lowFuelWarn
            }
            DeviceExecutorItem {
                height: 25
                width: eachItemWidth*2
                itemComp: engineFirst.pitchControl
            }
        }
    }

    // 链路设备信息展示
    LinkerDeviceDisplay {
        id: linker
        height: 100
        width: eachItemWidth*2
        itemWidth: eachItemWidth
        anchors.top: parent.top
        anchors.right: parent.right
    }

    // 舵机数据驱动来源切换
    GcsButton{
        id: comcan
        anchors.top: linker.bottom
        anchors.right: parent.right
        text: uptDataSrc? uptDataSrc.groupName: qsTr("");
        normalColor: "transparent"
        anchors.rightMargin: 10
        width: 50
        height: 50
        onClicked: {
            if (flapRundder){
                var cur = flapRundder.selectNode
                flapRundder.selectNode = cur+1
            }
        }
    }

    // 遥测数据测试
    UnittestParamDisplay{
        anchors.rightMargin: 10
        anchors.top: comcan.bottom
        anchors.right: parent.right
        width: 110
        height: 60
    }

    // 反馈消息数据显示
    CommandFeedbackDisplay{
        id: cmdstate
        anchors.top: parent.top
        anchors.right: linker.left
        anchors.left: engineDisp.right
    }

    GcsDivider {
        orientation: Qt.Vertical
        anchors.right: cmdstate.left
        height: 90
        spacing: 0
    }

    GcsDivider {
        orientation: Qt.Vertical
        anchors.left: cmdstate.right
        height: 90
        spacing: 0
    }

    Item {
        height: 190
        width: eachItemWidth*2
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        Column{
            anchors.fill: parent
            anchors.margins: 1
            Repeater {
                model: attiCenterShow?attiCenterShow.dataModel: null
                delegate: GcsHorizText {
                    property var displayObj: object
                    Connections {
                        target: displayObj
                        function onUpdateColorValue(value, iColor) {
                            horizImg.textValue = (iColor > 0xf) ? qsTr("--") : value;
                            horizImg.valueDefColor =  txtColorMap[iColor & 0xff]
                        }
                    }
                    id: horizImg
                    height: 25
                    width: parent.width
                    leftWidth: parent.width/2-15
                    textDefColor: GcsTheme.primaryColor
                    labelValue: displayObj.itemName
                }
            }
        }
    }

    Item {
        height: 190
        width: eachItemWidth*2
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        Column{
            anchors.fill: parent
            anchors.margins: 1
            Repeater {
                model: attiRightShow?attiRightShow.dataModel: null
                delegate: GcsHorizText {
                    property var displayObj: object
                    Connections {
                        target: displayObj
                        function onUpdateColorValue(value, color) {
                            horiz.textValue = (color > 0xf) ? qsTr("--") : value;
                            horiz.valueDefColor =  txtColorMap[color & 0xf]
                        }
                    }
                    id: horiz
                    height: 25
                    width: parent.width
                    textDefColor: GcsTheme.primaryColor
                    labelValue: displayObj.itemName
                }
            }
        }
    }
}
