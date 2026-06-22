import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Text {
    property color textColor: GcsTheme.fontPrimaryColor
    property int defFontSize: TextStyle.Body

    id:text

    color: textColor
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    //renderType: Text.QtRendering
    font.family: TextStyle.family
    font.pixelSize: defFontSize
}
