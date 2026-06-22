import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Rectangle {
    id: base_bar
    color: backgroundColor
    property alias notice: noticeTxt.text
    property int vWidth: (parent.width - headHeight * 2 - 10 )/12 - 5
    property int vHeight: headHeight - 10
    property bool showClose: true
    property bool isRestore: false

    property var stayTopClickListener: function(){
        stayTop = !stayTop
        if (stayTop) {
            window.flags = Qt.WindowStaysOnTopHint
        } else {
            window.flags = Qt.FramelessWindowHint
        }
    }

    property var darkClickListener: function(){
        if(GcsTheme.dark){
            GcsTheme.darkMode = ThemeType.Light
        }else{
            GcsTheme.darkMode = ThemeType.Dark
        }
    }

    property var maxClickListener: function(){
        isRestore = !isRestore;
        if (fmcwindow.visibility ===  Window.Maximized) {
            fmcwindow.showNormal()
        } else {
            fmcwindow.showMaximized()
        }
    }

    property var settingClickListener: function(){

    }

    property var closeClickListener: function(){
        fmcwindow.close()
    }

    Item{
        id: statebar
        anchors.right: parent.right
        anchors.top: parent.top
        height: headHeight
        width: headHeight

        GridLayout{
            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.topMargin: 2
            anchors.bottomMargin: 2
            rowSpacing: 2
            columns: 1

            GcsIconButton{
                id:btn_maximize
                Layout.minimumHeight: 28
                Layout.minimumWidth: headHeight
                verticalPadding: 0
                horizontalPadding: 0
                iconSource: isRestore ? ttfIcons["screenShrink"] : ttfIcons["screenExpand"]
                Layout.alignment: Qt.AlignVCenter
                onClicked: maxClickListener()
            }

            GcsIconButton{
                id:btn_close
                Layout.minimumHeight: 28
                Layout.minimumWidth: headHeight
                verticalPadding: 0
                horizontalPadding: 0
                iconSource : ttfIcons["closeBold"]
                Layout.alignment: Qt.AlignVCenter
                onClicked: closeClickListener()
            }
        }
    }

    Item{
        id: setbar
        anchors.left: parent.left
        anchors.top: parent.top
        height: headHeight
        width: headHeight

        GridLayout{
            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.topMargin: 2
            anchors.bottomMargin: 2
            rowSpacing: 2
            columns: 1
            GcsIconButton{
                padding: 0
                Layout.minimumHeight: 28
                Layout.minimumWidth: headHeight
                verticalPadding: 0
                horizontalPadding: 0
                iconSource: ttfIcons["setting"]
                Layout.alignment: Qt.AlignVCenter
                onClicked: settingClickListener()
            }
            GcsIconButton{
                id:btn_dark
                padding: 0
                Layout.minimumHeight: 28
                Layout.minimumWidth: headHeight
                verticalPadding: 0
                horizontalPadding: 0
                iconSource: ttfIcons["favorite"]
                Layout.alignment: Qt.AlignVCenter
                onClicked: darkClickListener()
            }
        }
    }

    Item {
        id: showNotice
        width: 400
        height: headHeight
        anchors.top: parent.top
        anchors.right: statebar.left
        GcsText {
            id: noticeTxt
            anchors.fill: parent
            textColor: GcsTheme.orange
        }

        MouseArea{
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            property point startPos: Qt.point(0,0)
            onPressed: (mouse) => {
                startPos = Qt.point(mouse.x, mouse.y)
            }
            onPositionChanged: (mouse) => {
                if (pressed) {
                    fmcwindow.x += (mouse.x - startPos.x)
                    fmcwindow.y += (mouse.y - startPos.y)
                }
            }
        }
    }

    Item {
        id: head_view
        height: headHeight
        anchors.left: setbar.right
        anchors.right: showNotice.left
        Row{
            anchors.fill: parent
            anchors.margins: 5
            spacing: 5
            Repeater {
                model: currVehicle ? currVehicle.deviceList: null
                delegate: GcsToggleButton {
                    width: vWidth
                    height: vHeight
                    text: object.rawGroupName
                    checkeState: index === menuModelSel ? true: false
                    onClicked: {
                        menuModelSel = index
                        leftDeviceGroup = object
                        if (leftDeviceGroup) {
                            leftDeviceGroup.selectIndex = 0
                            var selectNode = leftDeviceGroup.selectNode
                            var mapGroupName = selectNode.mapGroup
                            commandMgr.setDataFrame(mapGroupName)

                            vechicleRC.commands.updateSysCmdGroup(mapGroupName)
                            rightMenuGroupSel = 0
                            leftMenuGroupSel = 0
                            selectCmdGroup = 0
                        }
                    }
                }
            }
        }
    }
}
