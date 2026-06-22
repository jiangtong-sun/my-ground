import QtQuick 2.15
import QtQuick.Window 2.15
import GcsQuick 1.0

Window {
    id: window
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    property bool isRestore: false
    color: "black"
    Grid{
        anchors.fill: parent
        columns: 10
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["add"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["addCircle"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["arrowRightCircle"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["browse"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["arrowDoubleLeft"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["arrowLeftCircle"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["close"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["complete"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["copy"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["dataView"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["download"]
        }


        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["exports"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["edit"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["error"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["favorite"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["fileDelete"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["home"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["history"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["fileOpen"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["screenShrink"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["screenExpand"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["map"]
        }

        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["modular"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["notification"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["play"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["navigation"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["move"]
        }

        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["pin"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["share"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["success"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["setting"]
        }
        GcsIconButton{
            width: 30
            height: 30
            iconSource: ttfIcons["text"]
        }
    }

}
