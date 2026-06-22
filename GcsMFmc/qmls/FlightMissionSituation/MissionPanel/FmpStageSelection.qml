import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import GcsQuick 1.0

ControlBackground {
    property var currFightLine: null
    property string nameStage: ""
    GcsToggleButton {
        text: nameStage
        width: parent.width
        height: 30
    }

    ListModel{
        id: selectStageArr
    }

    Grid {
        id: destArea
        columns: 4
        rows: 2
        spacing: 1
        anchors.leftMargin: 4
        anchors.rightMargin: 4
        anchors.topMargin: 40
        anchors.bottomMargin: 4
        anchors.fill: parent
        Repeater {
            model: selectStageArr
            delegate: GcsToggleButton {
                property var model: stageSelect
                property string name: stageSelect.stageName
                property bool defNode: model.defNode
                property bool indrag: false;
                property int pressonx: 0
                property int pressony: 0

                id: dragItem
                width: 50;
                height: 26;
                hoverEnabled: true
                Drag.dragType: Drag.Internal
                Drag.supportedActions: Qt.MoveAction
                Drag.active: dragArea.drag.active;
                Drag.mimeData: {"name": name}
                checkeState: defNode
                text: name

                MouseArea {
                    id: dragArea;
                    anchors.fill: parent;
                    drag.target: parent;
                    onDoubleClicked: {
                        removeSelectStage(model)
                    }
                    onClicked: {
                        enableStageDefault(model)
                    }
                    onPressed: {
                        pressonx = dragItem.x
                        pressony = dragItem.y
                    }
                    onPositionChanged: {
                        if (Math.abs(dragItem.x - pressonx) > 20 ||
                            Math.abs(dragItem.y - pressony) > 10) {
                            indrag = true
                        }
                    }
                    onReleased: {
                        if (indrag) {
                            moveStage(model)
                        }

                        indrag = false
                    }
                }
            }
        }
    }

    function enableStageDefault(modeStage) {
        currFightLine.defaultFlineStage(modeStage)
    }

    function moveStage(model) {
        var moveElem = 0;
        for(; moveElem < selectStageArr.count; moveElem++) {
            var cur = selectStageArr.get(moveElem)
            if (cur.stageSelect === model) {
                break
            }
        }

        var tmp = selectStageArr.get(moveElem);
        if (moveElem + 1 < selectStageArr.count) {
            var last = selectStageArr.count - moveElem - 1
            currFightLine.moveFlineStage(model, moveElem)
            selectStageArr.move(moveElem + 1, moveElem, last)
            selectStageArr.set(selectStageArr.count-1, tmp)
        }
    }

    function selectStageClone(selectStage, dropx, name) {
        var newStage = selectStage.clone()
        currFightLine.addFlineStage(newStage, false)
        selectStageArr.append({"stageSelect": newStage})
    }

    function clearSelectStage() {
        selectStageArr.clear()
    }

    function removeSelectStage(select) {
        var moveElem = 0;
        for(; moveElem < selectStageArr.count; moveElem++) {
            var cur = selectStageArr.get(moveElem)
            if (cur.stageSelect === select) {
                currFightLine.removeFlineStage(cur.stageSelect)
                selectStageArr.remove(moveElem, 1)
            }
        }
    }
}
