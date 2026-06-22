import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Text {
    property int iconSize: 18
    property string iconSource
    property color iconColor: GcsTheme.dark ? GcsTheme.White : GcsTheme.Black

    id:control

    color: iconColor
    font.family: font_loader.name
    font.pixelSize: iconSize
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    text: iconSource

    FontLoader{
        id: font_loader
        source: "../res/iconfont.ttf"
    }
}
