import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsStationRole 1.0
import GcsQuick 1.0

ControlBackground {

    GcsFilledButton{
        id: switchRole
        radiusVal: 0
        anchors.fill: parent
        text: stationRole === GcsStationRole.ControlRole ? qsTr("控制席位") :  qsTr("任务席位")
        Layout.alignment: Qt.AlignVCenter
        onClicked: {
            commandMgr.switchStationRoleMsg()
        }
    }
}
