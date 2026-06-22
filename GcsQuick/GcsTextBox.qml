import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

TextField {
    signal commit(string text)
    property bool disabled: false
    property string notifyText: qsTr("")
    property color normalColor: GcsTheme.fontPrimaryColor

    id:control

    background: Label {
        id: placeholder
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        color: GcsTools.withOpacity(GcsTheme.primaryColor, 0.7)
        text: notifyText
    }

    onFocusChanged: {
        if (activeFocus || control.text.length > 0) {
            placeholder.text = qsTr("")
        } else {
            placeholder.text = notifyText
        }
    }

    maximumLength: 12
    enabled: !disabled
    color: normalColor
    bottomPadding: 1
    topPadding: 1

    font.family: TextStyle.family
    font.pixelSize: TextStyle.Body
    verticalAlignment: Text.AlignVCenter
    Keys.onEnterPressed: (event)=> d.handleCommit(event)
    Keys.onReturnPressed:(event)=> d.handleCommit(event)

    QtObject{
        id:d
        function handleCommit(event){
            control.commit(control.text)
        }
    }
}
