import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Text {
    property color textColor: GcsTheme.fontPrimaryColor
    property int pixelSize: TextStyle.Body

    id:text

    color: textColor
    renderType: Text.NativeRendering
    font.pixelSize: pixelSize
}
