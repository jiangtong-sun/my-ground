import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

PopupOperatePanel {
    id: control
    property string iconSource
    property string title: ""
    property string message: ""
    property string negativeText: qsTr("取消")
    property string positiveText: qsTr("确定")
    property int messageTextFormart: Text.AutoText
     property color iconColor: GcsTheme.orange
    property var onCancelClickListener: function(){ control.close()}
    property var onSettingClickListener: function(){ control.close()}

    implicitWidth: 300
    implicitHeight: content.height
    focus: true

    Item {
        id:content
        width: parent.width
        height: layout_column.childrenRect.height

        ColumnLayout{
            id: layout_column
            width: parent.width
            ControlBackground{
                radius:5
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                GcsText{
                    text:title
                    leftPadding: 20
                    rightPadding: 20
                    anchors.fill: parent
                    wrapMode: Text.WrapAnywhere
                }
            }

            Row{
                id: txtArea
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                Item {
                    width: parent.width/6
                    height: parent.height
                    GcsIcon {
                        iconSize: 30
                        anchors.fill: parent
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        iconColor: control.iconColor
                        iconSource: control.iconSource
                    }
                }

                Item {
                    width: parent.width*5/6
                    height: parent.height
                    GcsText{
                        text: message
                        anchors.fill: parent
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        wrapMode: Text.WrapAnywhere
                        textColor: control.iconColor
                        Component.onCompleted: {
                            if (message.indexOf("\n") > 0) {
                                txtArea.height = 40*2
                                return;
                            }
                            txtArea.height = 40
                        }
                    }
                }
            }

            Item{
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                RowLayout{
                    spacing: 10
                    anchors.fill: parent
                    anchors.leftMargin: 80
                    anchors.rightMargin: 80
                    GcsButton{
                        text: negativeText
                        onClicked: {
                            if(control.onCancelClickListener){
                                control.onCancelClickListener()
                            }
                        }
                    }
                    GcsFilledButton{
                        text: positiveText
                        onClicked: {
                            if(control.onSettingClickListener){
                                control.onSettingClickListener()
                            }
                        }
                    }
                }
            }
        }
    }
}
