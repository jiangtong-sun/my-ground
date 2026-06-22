import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QuickDataLinkElem 1.0
import ChannelSelection 1.0
import GcsQuick 1.0

Item {
    property int itemWidth: 70
    property var mainLinker: commandMgr.mainLinker
    property var slaveLinker: commandMgr.slaveLinker
    property var serialLinker: commandMgr.serialLinker

    id: linker

    Grid {
        anchors.fill: parent
        columns: 2
        QuickDataLinkElem {
            id: mainElem
            color: GcsTheme.green
            dataNode: mainLinker
            linkName: mainLinker.name
            width: itemWidth
            height: 50
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    commandMgr.netChannel = ChannelSelection.MainNetwork
                    serialLinker.working = false
                    slaveLinker.working = false
                    mainLinker.working = true
                }
            }
        }
        QuickDataLinkElem {
            id: slaveElem
            color: GcsTheme.green
            dataNode: slaveLinker
            linkName: slaveLinker.name
            width: itemWidth
            height: 50
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    commandMgr.netChannel = ChannelSelection.SlaveNetwork
                    serialLinker.working = false
                    slaveLinker.working = true
                    mainLinker.working = false
                }
            }
        }
        QuickDataLinkElem {
            id: serailElem
            color: GcsTheme.green
            dataNode: serialLinker
            linkName: serialLinker.name
            width: itemWidth
            height: 50
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    commandMgr.netChannel = ChannelSelection.SerialNetwork
                    serialLinker.working = true
                    slaveLinker.working = false
                    mainLinker.working = false
                }
            }
        }
    }
}
