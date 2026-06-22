import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import GcsQuick 1.0

Item {
    id: linkctl
    property int linkWidth: 100
    property int linkHeight: 50

    Item{
        id: llink
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.top: parent.top
        height: linkHeight
        width: linkWidth/4
        Canvas {
            anchors.fill: parent
            onPaint: {
                var ctx = getContext('2d');
                ctx.fillStyle = "transparent"
                ctx.fillRect(0, 0,  ulink.width, ulink.height);
                ctx.beginPath()
                context.fillStyle = GcsTheme.drawLineColor
                context.fillRect(5 , 40, 2,  5)
                context.fillRect(10, 35, 2, 10)
                context.fillRect(15, 30, 2, 15)
                context.fillRect(20, 25, 2, 20)
                context.fillRect(25, 20, 2, 25)
                context.fillRect(30, 15, 2, 30)
                ctx.fill()
                ctx.stroke()
            }
        }

        Item{
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 5
            anchors.leftMargin: 10
            height: 20
            width: 20
            GcsText {
                text: "L"
            }
        }
    }

    Item{
        id: llinkt
        anchors.left: llink.right
        anchors.top: parent.top
        anchors.topMargin: 10
        height: linkHeight*2/3
        width: linkWidth/4
        ColumnLayout{
            anchors.fill: parent
            GcsText {
                text: "20"
            }
            GcsText {
                text: "10"
            }
        }
    }

    Item{
        id: ulink
        anchors.right: parent.right
        anchors.rightMargin: linkWidth/4
        anchors.top: parent.top
        height: linkHeight
        width: linkWidth/4

        Canvas {
            anchors.fill: parent
            onPaint: {
                var ctx = getContext('2d');
                ctx.fillStyle = "transparent"
                ctx.fillRect(0, 0,  ulink.width, ulink.height);

                ctx.beginPath()
                context.fillStyle = GcsTheme.drawLineColor
                context.fillRect(5 , 40, 2,  5)
                context.fillRect(10, 35, 2, 10)
                context.fillRect(15, 30, 2, 15)
                context.fillRect(20, 25, 2, 20)
                context.fillRect(25, 20, 2, 25)
                context.fillRect(30, 15, 2, 30)
                ctx.fill()
                ctx.stroke()
            }
        }

        Item{
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 5
            anchors.leftMargin: 10
            height: 20
            width: 20
            GcsText {
                text: "U"
            }
        }
    }

    Item{
        id: ulinkt
        anchors.left: ulink.right
        anchors.top: parent.top
        anchors.topMargin: 10
        height: linkHeight*2/3
        width: linkWidth/4
        ColumnLayout{
            anchors.fill: parent
            GcsText {
                text: "20"
            }
            GcsText {
                text: "10"
            }
        }
    }

}
