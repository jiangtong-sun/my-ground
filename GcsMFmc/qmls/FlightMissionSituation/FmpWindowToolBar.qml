import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item{
    property string restoreText : qsTr("Restore")
    property string maximizeText : qsTr("Maximize")
    property string closeText : qsTr("Close")
    property string timeText : qsTr("00:00:00")

    property color textColor: GcsTheme.fontPrimaryColor

    property bool showTime: true
    property bool showClose: true
    property bool showMaximize: true

    property bool isRestore: false
    property bool resizable: true

    property bool isMac: GcsTools.isMacos()
    property color borerlessColor : GcsTheme.primaryColor

    property var maxClickListener: function(){
        isRestore = !isRestore;
        if (fmpwindow.visibility ===  Window.Maximized) {
            fmpwindow.showNormal()
        } else {
            fmpwindow.showMaximized()
        }
    }

    property var closeClickListener: function(){
        fmpwindow.close()
    }

    id:control

    RowLayout{
        id: standard_buttons
        height: parent.height
        anchors.right: parent.right
        spacing: 0

        GcsIconButton{
            id:btn_maximize
            Layout.preferredWidth: 60
            Layout.preferredHeight: 30
            padding: 0
            verticalPadding: 0
            horizontalPadding: 0
            iconSource: isRestore ? ttfIcons["screenShrink"] : ttfIcons["screenExpand"]
            Layout.alignment: Qt.AlignVCenter
            visible: showMaximize
            radius: 0
            iconColor: control.textColor
            text: isRestore? restoreText : maximizeText
            onClicked: maxClickListener()
        }

        GcsIconButton{
            id:btn_close
            Layout.preferredWidth: 60
            Layout.preferredHeight: 30
            padding: 0
            verticalPadding: 0
            horizontalPadding: 0
            iconSource : ttfIcons["closeBold"]
            Layout.alignment: Qt.AlignVCenter
            text: closeText
            visible: showClose
            radius: 0
            iconColor: control.textColor
            onClicked: closeClickListener()
        }

        GcsText{
            id: btn_time
            Layout.preferredWidth: 70
            Layout.preferredHeight: 30
            Layout.alignment: Qt.AlignVCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: configMgr.appRunTime

            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                property point startPos: Qt.point(0,0)
                onPressed: (mouse) => {
                    startPos = Qt.point(mouse.x, mouse.y)
                }
                onPositionChanged: (mouse) => {
                    if (pressed) {
                        fmpwindow.x += (mouse.x - startPos.x)
                        fmpwindow.y += (mouse.y - startPos.y)
                    }
                }
            }
        }
    }
}
