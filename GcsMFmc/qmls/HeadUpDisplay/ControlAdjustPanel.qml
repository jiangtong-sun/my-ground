import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickEngineElemNode 1.0
import QuickEngineElemNode2 1.0
import QuickEngineElemNode3 1.0
import GcsQuick 1.0

Item {
    property int partWidth: 100
    property int partHeight: 100
    property var engineData: null

    width: partWidth
    height: partHeight
    GroupRectangle {
        id: control
        width: partWidth
        height: partHeight
        anchors.fill: parent
        penColor: "#1e90ff"
        textColor: leftRightColor
        text: qsTr("发动机")
    }
    Component{
        id: c21engine
        QuickEngineElemNode{
            id: engine
            setColor: GcsTheme.fontPrimaryColor
            fontColor: GcsTheme.primaryColor
            color: GcsTheme.primaryColor
            warnColor: GcsTheme.orange
            anchors.fill: parent
            dataNode: engineData
        }
    }
    Component{
        id: xfengine
        QuickEngineElemNode3{
            id: engine
            setColor: GcsTheme.fontPrimaryColor
            fontColor: GcsTheme.primaryColor
            color: GcsTheme.primaryColor
            redColor: GcsTheme.red
            greenColor: GcsTheme.green
            warnColor: GcsTheme.orange
            anchors.fill: parent
            dataNode: engineData
        }
    }

    Component{
        id: whxqengine
        QuickEngineElemNode2{
            id: engine
            setColor: GcsTheme.fontPrimaryColor
            fontColor: GcsTheme.primaryColor
            color: GcsTheme.primaryColor
            warnColor: GcsTheme.orange
            anchors.fill: parent
            dataNode: engineData
        }
    }

    Item{
        anchors.margins: 10
        anchors.topMargin: 28
        height: partHeight - 20
        width: partWidth - 20
        GcsLoader{
            anchors.fill: parent
            sourceComponent: {
                if(projectEnv === 1) {
                    return c21engine;
                }
                if (projectEnv === 2) {
                    return xfengine;
                }
                if (projectEnv === 3) {
                    return whxqengine;
                }
            }
        }
    }
}
