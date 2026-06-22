import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Button {
    id: control
    property int radiusVal: 6
    property bool warning: false
    property bool transparentMode: false
    property string contentDescription: ""
    property color normalColor: GcsTheme.itemNormalColor

    verticalPadding: 0
    horizontalPadding:12
    focusPolicy:Qt.TabFocus
    Accessible.role: Accessible.Button
    Accessible.name: control.text
    Accessible.description: contentDescription
    Accessible.onPressAction: control.clicked()

    background: ControlBackground{
        id: controlBk
        anchors.fill: parent
        radius: radiusVal
        color: transparentMode? "transparent": normalColor
        borderWidth: transparentMode ? 0 : 1
        borderColor: {
            if(warning){
                return GcsColors.Orange.dark
            } else {
                return normalColor
            }
        }
    }

    contentItem: GcsText {
        text: control.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
