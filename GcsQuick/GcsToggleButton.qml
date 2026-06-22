import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Button {
    property int btnRadius: 1
    property bool disabled: false
    property bool checkeState: false
    property string contentDescription: ""
    property alias borderColor: bkgCtrl.borderColor
    property color normalColor: {
        if(checkeState){
            return GcsTheme.primaryColor
        }else{
            return GcsTheme.itemNormalColor
        }
    }

    property color textColor: {
        if (checkeState) {
            return GcsTheme.White
        }

        if(GcsTheme.dark){
            return GcsTheme.White
        }else{
            return GcsTheme.Black
        }
    }

    Accessible.role: Accessible.Button
    Accessible.name: control.text
    Accessible.description: contentDescription
    Accessible.onPressAction: control.clicked()
    focusPolicy: Qt.TabFocus

    id: control

    enabled: !disabled
    verticalPadding: 0
    horizontalPadding:12

    background: ControlBackground{
        id: bkgCtrl
        anchors.fill: parent
        radius: btnRadius
        color: normalColor
        bottomMargin:  1
        borderWidth: 1
    }

    contentItem: GcsText {
        text: control.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: control.textColor
    }
}
