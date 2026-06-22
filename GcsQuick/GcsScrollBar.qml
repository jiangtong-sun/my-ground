import QtQuick 2.15
import QtQuick.Templates 2.15 as T
import GcsQuick 1.0

T.ScrollBar {
    id: control

    property color color: GcsTheme.primaryColor
    property color pressedColor: GcsTheme.dark ? Qt.darker(color,1.2) : Qt.lighter(color,1.2)

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)

    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    visible: control.policy !== T.ScrollBar.AlwaysOff

    minimumSize: Math.max(orientation === Qt.Horizontal ? height / width : width / height,0.3)

    z: horizontal? 10 : 20

    verticalPadding : vertical ? 10 : 3
    horizontalPadding : horizontal ? 10 : 3

    background: Rectangle{
        id:back_rect
        radius: 5
        color: "transparent"
    }

    contentItem: Item {
        implicitWidth: width / 2
        implicitHeight: width / 2

        Rectangle{
            id: rect_bar
            width: vertical ? width / 2 : parent.width
            height: horizontal  ? width / 2 : parent.height
            color: control.color
            anchors{
                right: vertical ? parent.right : undefined
                bottom: horizontal ? parent.bottom : undefined
            }
            radius: width / 2
            visible: control.size < 1.0
        }
    }
}
