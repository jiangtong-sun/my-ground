import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

ControlBackground {
    id: base_bar
    property var vehList: commandMgr.aircrafts
    property bool stayTop: false
    property string darkText : qsTr("Dark")
    property string lightText : qsTr("Light")
    property string stayTopText : qsTr("Sticky on Top")
    property string stayTopCancelText : qsTr("Sticky on Top cancelled")
    property color textColor: GcsTheme.fontPrimaryColor

    property int vWidth: (parent.width - 320)/vehList.count
    property int vHeight: headHeight

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

    FmpHeaderLinker {
        id: hlinker
        anchors.right: parent.right
        linkWidth: 160
        linkHeight: vHeight
        height: vHeight
        width: 160
    }

    FmpWindowToolBar{
        id: basebar
        anchors.left: parent.left
        height: vHeight/2
        width: 160
    }

    RowLayout{
        id: setbar
        anchors.left: parent.left
        anchors.top: basebar.bottom
        anchors.leftMargin: 10
        height: vHeight/2
        width: 160
        spacing: 0

        // GcsIconButton{
        //     padding: 0
        //     display: Button.TextOnly
        //     Layout.alignment: Qt.AlignVCenter
        //     Layout.minimumHeight: 30
        //     Layout.minimumWidth: 60
        //     verticalPadding: 0
        //     horizontalPadding: 0
        //     text: "V" + configMgr.appVersion
        // }

        GcsIconButton{
            id:btn_dark
            padding: 0
            Layout.minimumHeight: 30
            Layout.minimumWidth: 60
            verticalPadding: 0
            horizontalPadding: 0
            iconSource: ttfIcons["favorite"]
            Layout.alignment: Qt.AlignVCenter
            text: GcsTheme.dark ? base_bar.lightText : base_bar.darkText
            onClicked: darkClickListener()
        }

        GcsIconButton{
            id:btn_stay_top
            padding: 0
            Layout.minimumHeight: 30
            Layout.minimumWidth: 60
            verticalPadding: 0
            horizontalPadding: 0
            iconSource : ttfIcons["home"]
            Layout.alignment: Qt.AlignVCenter
            text: stayTop ? base_bar.stayTopCancelText : base_bar.stayTopText
            onClicked: stayTopClickListener()
        }
    }

    // Item {
    //     id: head_view
    //     anchors.fill: parent
    //     anchors.leftMargin: 160
    //     anchors.rightMargin: 160
    //     RowLayout{
    //         anchors.fill: parent
    //         Repeater {
    //             model: vehList
    //             FlightVehiclePanel {
    //                 vehicle: object
    //                 ctrlWidth: vWidth
    //                 ctrlHeight: vHeight
    //             }
    //         }
    //     }
    // }
}
