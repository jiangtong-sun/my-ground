import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Item {
    id: root
    property string text: ""
    property int duration: 2000
    property real maxWidth: 300
    property bool _showing: false
     property color backgroundColor: GcsTheme.backgroundColor
    property color txtColor: GcsTheme.fontPrimaryColor

    function show(message, showTime) {
        toastLoader.item.show(message, showTime || root.duration)
    }

    GcsLoader {
        id: toastLoader
        sourceComponent: Popup {
            id: toastPopup
            closePolicy: Popup.NoAutoClose
            modal: false
            padding: 0
            margins: 0

            background: ControlBackground {
                color: root.backgroundColor
                radius: 8
            }

            contentItem: GcsText {
                text: root.text
                padding: 12
                wrapMode: Text.Wrap
                textColor: root.txtColor
                width: Math.min(implicitWidth, root.maxWidth)
            }

            enter: Transition {
                NumberAnimation {
                    property: "opacity"
                    from: 0; to: 1
                    duration: 200
                }
            }

            exit: Transition {
                NumberAnimation {
                    property: "opacity"
                    from: 1; to: 0
                    duration: 200
                }
            }

            function show(message, duration) {
                root.text = message
                toastPopup.open()

                hideTimer.restart(duration)
            }

            Timer {
                id: hideTimer
                onTriggered: toastPopup.close()
            }
        }
    }
}
