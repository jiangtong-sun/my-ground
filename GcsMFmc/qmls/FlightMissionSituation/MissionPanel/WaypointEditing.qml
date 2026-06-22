import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import GcsQuick 1.0

ControlBackground{
    id: backControl
    property var iValidator: RegularExpressionValidator{regularExpression: /^[0-9.-]*$/}
    property bool editorPoint: false
    property alias text: inputBox.text
    property var editFinishListener : function(){ }
    property var commitListener : function(){ }

    GcsTextBox {
        id: inputBox
        anchors.fill: parent
        readOnly: editorPoint
        validator: iValidator
        maximumLength: 12
        focus: true
        onActiveFocusChanged: {
            if (inputBox.activeFocus) {
                backControl.borderColor = GcsTheme.primaryColor
            } else {
                backControl.borderColor = GcsTheme.windowBorderColor
                commitListener()
            }
        }
        onCommit: {
            commitListener()
            inputBox.focus = false
        }
        onEditingFinished: {
            editFinishListener()
        }
    }
}
