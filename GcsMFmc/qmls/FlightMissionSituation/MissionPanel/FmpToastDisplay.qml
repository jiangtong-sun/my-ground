import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Popup {
    id: toast
    width: 200
    height: 50
    opacity: 0    // 初始透明
    modal: false  // 非模态，允许其他交互
    dim: false    // 背景不变暗
    closePolicy: Popup.NoAutoClose  // 禁用自动关闭
    property string text: "提示内容"
    property int duration: 2000  // 默认显示 2 秒
    property string position: "center"  // 可选值: "top", "center", "bottom"

    background: ControlBackground {
        radius: 4
    }

    contentItem: GcsText {
        text: toast.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    enter: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 300 }
    }

    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 300 }
    }

    onPositionChanged: updatePosition()
    Component.onCompleted: updatePosition()

    function updatePosition() {
        if (position === "top") {
            toast.anchors.top = parent.top
            toast.anchors.topMargin = 20
            toast.anchors.horizontalCenter = parent.horizontalCenter
        } else if (position === "center") {
            toast.anchors.centerIn = parent
        } else {
            toast.anchors.bottom = parent.bottom
            toast.anchors.bottomMargin = 20
            toast.anchors.horizontalCenter = parent.horizontalCenter
        }
    }

    function show(message, durationMs) {
        if (message) toast.text = message
        if (durationMs) toast.duration = durationMs
        toast.open()
        timer.start()
    }

    Timer {
        id: timer
        interval: toast.duration
        onTriggered: toast.close()
    }
}
