import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

Grid{
    property int unittestCode: 0
    property int unittestJump: 0
    property bool testMode: configMgr.autoTestMode
    visible: testMode
    width: 110
    height: 60
    columns: 2
    GcsTextBox{
        height: 20
        width: 60
        id: randval
    }
    GcsButton{
        height: 20
        width: 50
        text: qsTr("u8");
        normalColor: "transparent"
        onClicked: {
            var val = randval.text
            unittestCode = parseInt(val)
            var dval = jumpval.text
            unittestJump = parseInt(dval)
            if (testMode) {
                commandMgr.unittestRandSimple(unittestCode, unittestJump, 0)
            }
        }
    }
    GcsTextBox{
        id: jumpval
        height: 20
        width: 60
    }
    GcsButton{
        height: 20
        width: 50
        text: qsTr("n1");
        normalColor: "transparent"
        onClicked: {
            var val = randval.text
            unittestCode = parseInt(val)
            var dval = jumpval.text
            unittestJump = parseInt(dval)
            if (testMode) {
                commandMgr.unittestRandSimple(unittestCode, unittestJump, 1)
            }
        }
    }
    GcsButton{
        height: 20
        width: 60
        text: qsTr("auto");
        normalColor: "transparent"
        onClicked: {
            if (testMode) {
                commandMgr.unittestSimpleThd()
            }
        }
    }
}
