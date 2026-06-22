#include "WindowShortCut.h"
#include "log_category.h"
#include "stringtranslate.h"

#define NEXT_COMMAND_DELAY 15
WindowShortCut::WindowShortCut() {
    stopSlideAction = nullptr;
    enginePreAction = nullptr;
    emicPreAction = nullptr;
    brakeAddAction = nullptr;
    brakeSubAction = nullptr;
    brakeAdd2Action = nullptr;
    brakeSub2Action = nullptr;
    engencyBrakeAction = nullptr;
    adjustLeftAction = nullptr;
    adjustRightAction = nullptr;
    throttleAddAction = nullptr;
    throttleSubAction = nullptr;
    swithFlightAction = nullptr;
    goAroundAction = nullptr;
}

WindowShortCut::~WindowShortCut() {}

RCAdjustItem *WindowShortCut::nextAdjustExt() {
    if (adjustExtQueue.size_approx() > 0 && (currentTimeMs() > cal_next_time)) {
        uint32_t data = 0;
        if (adjustExtQueue.try_dequeue(data) && (data > 0)) {
            AdjustControl *commands = adjust_control_cmds->commands();
            int feed = (data >> 16) & 0xff;
            int itm = (data >> 24) & 0xff;
            int16_t value = (int16_t)(data & 0xffff);
            double dataValue = value / 10.0f; //
            RCAdjustItem *item = commands->getAdjustByfeedback(feed, itm, 0xffff);
            item->setRawValue(QString::number(dataValue));
            cal_next_time += NEXT_COMMAND_DELAY * NEXT_COMMAND_DELAY;
            COMMON_INFO("next adjust smart key {}, {}, {}", itm, feed, dataValue);
            return item;
        }
    }

    return nullptr;
}

RCCommandItem *WindowShortCut::nextControl() {
    if (commandQueue.size_approx() > 0 && (currentTimeMs() > cal_next_time)) {
        uint32_t data = 0;
        if (commandQueue.try_dequeue(data) && (data > 0)) {
            AdjustControl *commands = adjust_control_cmds->commands();
            RCCommandItem *item = commands->getControlByFeedback(data);
            cal_next_time += NEXT_COMMAND_DELAY * NEXT_COMMAND_DELAY;
            COMMON_INFO("next control smart key {}", data);
            return item;
        }
    }
    return nullptr;
}

RCAdjustItem *WindowShortCut::nextAdjust() {
    if (adjustQueue.size_approx() > 0 && (currentTimeMs() > cal_next_time)) {
        uint32_t data = 0;
        if (adjustQueue.try_dequeue(data) && (data > 0)) {
            AdjustControl *commands = adjust_control_cmds->commands();
            int feed = (data >> 16) & 0xff;
            int itm = (data >> 24) & 0xff;
            RCAdjustItem *item = commands->getAdjustByfeedback(feed, itm, 0xffff);
            item->setSelectSub((data >> 8) & 0xff);
            item->setSelectId((data >> 0) & 0xff);
            cal_next_time += NEXT_COMMAND_DELAY * NEXT_COMMAND_DELAY;
            COMMON_INFO("next adjust smart key {}, {}, {}, {}", itm, feed, (data >> 8) & 0xff,
                        (data >> 0) & 0xff);
            return item;
        }
    }

    return nullptr;
}

void WindowShortCut::stopSlideActionActivated() {
    assert(adjust_control_cmds != nullptr);
    if (adjust_control_cmds == nullptr) {
        return;
    }
    AdjustControl *commands = adjust_control_cmds->commands();
    if (commands == nullptr) {
        COMMON_INFO("unknown command object");
        return;
    }
    commandQueue.enqueue(0x34); //终止滑行
    cal_next_time = currentTimeMs();
}

void WindowShortCut::enginePreActionActivated() {
    assert(adjust_control_cmds != nullptr);
    if (adjust_control_cmds == nullptr) {
        return;
    }
    AdjustControl *commands = adjust_control_cmds->commands();
    if (commands == nullptr) {
        COMMON_INFO("unknown command object");
        return;
    }
    commandQueue.enqueue(0x95); //发动机预指令
    commandQueue.enqueue(0x96); //关车
    cal_next_time = currentTimeMs();
}

void WindowShortCut::emicPreActionActivated() {

    assert(adjust_control_cmds != nullptr);
    if (adjust_control_cmds == nullptr) {
        return;
    }
    AdjustControl *commands = adjust_control_cmds->commands();
    if (commands == nullptr) {
        COMMON_INFO("unknown command object");
        return;
    }
    uint32_t code1 = (35 << 24) | (0x25 << 16) | (1 << 8) | 0;
    uint32_t code2 = (35 << 24) | (0x25 << 16) | (16 << 8) | 1;
    uint32_t code3 = (35 << 24) | (0x25 << 16) | (17 << 8) | 1;
    adjustQueue.enqueue(code1); //预指令
    adjustQueue.enqueue(code2); //供油泵主
    adjustQueue.enqueue(code3); //供油泵备
    cal_next_time = currentTimeMs();
}

void WindowShortCut::sinkRateUpActionActivated() {
    assert(adjust_control_cmds != nullptr);
    if (adjust_control_cmds == nullptr) {
        return;
    }
    AdjustControl *commands = adjust_control_cmds->commands();
    if (commands == nullptr) {
        COMMON_INFO("unknown command object");
        return;
    }
    uint16_t value = 5;
    uint32_t code1 = (18 << 24) | (0xE4 << 16) | value;
    adjustExtQueue.enqueue(code1);
    cal_next_time = currentTimeMs();
}

void WindowShortCut::sinkRateDownActionActivated() {
    assert(adjust_control_cmds != nullptr);
    if (adjust_control_cmds == nullptr) {
        return;
    }
    AdjustControl *commands = adjust_control_cmds->commands();
    if (commands == nullptr) {
        COMMON_INFO("unknown command object");
        return;
    }
    uint16_t value = -5;
    uint32_t code1 = (18 << 24) | (0xE4 << 16) | value;
    adjustExtQueue.enqueue(code1);
    cal_next_time = currentTimeMs();
}

void WindowShortCut::lateralAddActionActivated() {
    assert(adjust_control_cmds != nullptr);
    if (adjust_control_cmds == nullptr) {
        return;
    }
    AdjustControl *commands = adjust_control_cmds->commands();
    if (commands == nullptr) {
        COMMON_INFO("unknown command object");
        return;
    }
    uint16_t value = 50;
    uint32_t code1 = (5 << 24) | (0xF5 << 16) | value;
    adjustExtQueue.enqueue(code1);
    cal_next_time = currentTimeMs();
}

void WindowShortCut::lateralSubActionActivated() {
    assert(adjust_control_cmds != nullptr);
    if (adjust_control_cmds == nullptr) {
        return;
    }
    AdjustControl *commands = adjust_control_cmds->commands();
    if (commands == nullptr) {
        COMMON_INFO("unknown command object");
        return;
    }
    uint16_t value = -50;
    uint32_t code1 = (5 << 24) | (0xF5 << 16) | value;
    adjustExtQueue.enqueue(code1);
    cal_next_time = currentTimeMs();
}

void WindowShortCut::pitchDown2DegActionActivated() {
    assert(adjust_control_cmds != nullptr);
    if (adjust_control_cmds == nullptr) {
        return;
    }
    AdjustControl *commands = adjust_control_cmds->commands();
    if (commands == nullptr) {
        COMMON_INFO("unknown command object");
        return;
    }
    uint16_t value = -15;
    uint32_t code1 = (14 << 24) | (0xE0 << 16) | value;
    adjustExtQueue.enqueue(code1);
    cal_next_time = currentTimeMs();
}

void WindowShortCut::pitchDown3DegActionActivated() {
    assert(adjust_control_cmds != nullptr);
    if (adjust_control_cmds == nullptr) {
        return;
    }
    AdjustControl *commands = adjust_control_cmds->commands();
    if (commands == nullptr) {
        COMMON_INFO("unknown command object");
        return;
    }
    uint16_t value = -20;
    uint32_t code1 = (14 << 24) | (0xE0 << 16) | value;
    adjustExtQueue.enqueue(code1);
    cal_next_time = currentTimeMs();
}

void WindowShortCut::pitchDown5DegActionActivated() {
    assert(adjust_control_cmds != nullptr);
    if (adjust_control_cmds == nullptr) {
        return;
    }
    AdjustControl *commands = adjust_control_cmds->commands();
    if (commands == nullptr) {
        COMMON_INFO("unknown command object");
        return;
    }
    uint16_t value = -40;
    uint32_t code1 = (14 << 24) | (0xE0 << 16) | value;
    adjustExtQueue.enqueue(code1);
    cal_next_time = currentTimeMs();
}

void WindowShortCut::brakeAddActionActivated() { COMMON_WARN("brakeAddActionActivated"); }
void WindowShortCut::brakeSubActionActivated() { COMMON_WARN("brakeSubActionActivated"); }
void WindowShortCut::brakeAdd2ActionActivated() { COMMON_WARN("brakeAdd2ActionActivated"); }
void WindowShortCut::brakeSub2ActionActivated() { COMMON_WARN("brakeSub2ActionActivated"); }
void WindowShortCut::engencyBrakeActionActivated() { COMMON_WARN("engencyBrakeActionActivated"); }
void WindowShortCut::adjustLeftActionActivated() { COMMON_WARN("adjustLeftActionActivated"); }
void WindowShortCut::adjustRightActionActivated() { COMMON_WARN("adjustRightActionActivated"); }
void WindowShortCut::throttleAddActionActivated() { COMMON_WARN("throttleAddActionActivated"); }
void WindowShortCut::throttleSubActionActivated() { COMMON_WARN("throttleSubActionActivated"); }
void WindowShortCut::swithFlightActionActivated() { COMMON_WARN("swithFlightActionActivated"); }
void WindowShortCut::goAroundActionActivated() { COMMON_WARN("goAroundActionActivated"); }

void WindowShortCut::removeHotKey() {

    if (stopSlideAction) {
        stopSlideAction->setRegistered(false);
        disconnect(stopSlideAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::stopSlideActionActivated);
    }

    if (enginePreAction) {
        enginePreAction->setRegistered(false);
        disconnect(enginePreAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::enginePreActionActivated);
    }

    if (emicPreAction) {
        emicPreAction->setRegistered(false);
        disconnect(emicPreAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::emicPreActionActivated);
    }

    if (brakeAddAction) {
        brakeAddAction->setRegistered(false);
        disconnect(brakeAddAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::brakeAddActionActivated);
    }

    if (brakeSubAction) {
        brakeSubAction->setRegistered(false);
        disconnect(brakeSubAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::brakeSubActionActivated);
    }

    if (brakeAdd2Action) {
        brakeAdd2Action->setRegistered(false);
        disconnect(brakeAdd2Action.get(), &QHotkey::activated, this,
                   &WindowShortCut::brakeAdd2ActionActivated);
    }

    if (brakeSub2Action) {
        brakeSub2Action->setRegistered(false);
        disconnect(brakeSub2Action.get(), &QHotkey::activated, this,
                   &WindowShortCut::brakeSub2ActionActivated);
    }

    if (engencyBrakeAction) {
        engencyBrakeAction->setRegistered(false);
        disconnect(engencyBrakeAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::engencyBrakeActionActivated);
    }

    if (adjustLeftAction) {
        adjustLeftAction->setRegistered(false);
        disconnect(adjustLeftAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::adjustLeftActionActivated);
    }

    if (adjustRightAction) {
        adjustRightAction->setRegistered(false);
        disconnect(adjustRightAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::adjustRightActionActivated);
    }

    if (throttleAddAction) {
        throttleAddAction->setRegistered(false);
        disconnect(throttleAddAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::throttleAddActionActivated);
    }

    if (throttleSubAction) {
        throttleSubAction->setRegistered(false);
        disconnect(throttleSubAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::throttleSubActionActivated);
    }

    if (swithFlightAction) {
        swithFlightAction->setRegistered(false);
        disconnect(swithFlightAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::swithFlightActionActivated);
    }

    if (goAroundAction) {
        goAroundAction->setRegistered(false);
        disconnect(goAroundAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::goAroundActionActivated);
    }

    if (pitchDown5DegAction) {
        pitchDown5DegAction->setRegistered(false);
        disconnect(pitchDown5DegAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::pitchDown5DegActionActivated);
    }

    if (pitchDown3DegAction) {
        pitchDown3DegAction->setRegistered(false);
        disconnect(pitchDown3DegAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::pitchDown3DegActionActivated);
    }

    if (pitchDown2DegAction) {
        pitchDown2DegAction->setRegistered(false);
        disconnect(pitchDown2DegAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::pitchDown2DegActionActivated);
    }

    if (lateralSubAction) {
        lateralSubAction->setRegistered(false);
        disconnect(lateralSubAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::lateralSubActionActivated);
    }

    if (lateralAddAction) {
        lateralAddAction->setRegistered(false);
        disconnect(lateralAddAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::lateralAddActionActivated);
    }

    if (sinkRateDownAction) {
        sinkRateDownAction->setRegistered(false);
        disconnect(sinkRateDownAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::sinkRateDownActionActivated);
    }

    if (sinkRateUpAction) {
        sinkRateUpAction->setRegistered(false);
        disconnect(sinkRateUpAction.get(), &QHotkey::activated, this,
                   &WindowShortCut::sinkRateUpActionActivated);
    }
}

void WindowShortCut::registerWinHotKey(VehicleCommand *allcommands) {

    adjust_control_cmds = allcommands;

    // pitchDown5DegAction = std::make_shared<QHotkey>(
    //     Qt::Key(VK_NUMPAD4), Qt::ControlModifier | Qt::AltModifier, true, this);
    // pitchDown3DegAction = std::make_shared<QHotkey>(
    //     Qt::Key(VK_NUMPAD2), Qt::ControlModifier | Qt::AltModifier, true, this);
    // pitchDown2DegAction = std::make_shared<QHotkey>(
    //     Qt::Key(VK_NUMPAD1), Qt::ControlModifier | Qt::AltModifier, true, this);

    pitchDown5DegAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+I"), true, this);
    pitchDown3DegAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+O"), true, this);
    pitchDown2DegAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+P"), true, this);

    lateralSubAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+Left"), true, this);
    lateralAddAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+Right"), true, this);
    sinkRateDownAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+Down"), true, this);
    sinkRateUpAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+Up"), true, this);

    stopSlideAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+Return"), true, this);
    enginePreAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Shift+E"), true, this);
    emicPreAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Shift+F"), true, this);

    brakeAddAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+W"), true, this);
    brakeSubAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+S"), true, this);
    brakeAdd2Action = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+A"), true, this);
    brakeSub2Action = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+D"), true, this);
    engencyBrakeAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+E"), true, this);
    adjustLeftAction = std::make_shared<QHotkey>(QKeySequence("Shift+Alt+Left"), true, this);
    adjustRightAction = std::make_shared<QHotkey>(QKeySequence("Shift+Alt+Right"), true, this);
    throttleAddAction = std::make_shared<QHotkey>(QKeySequence("Shift+Alt+Up"), true, this);
    throttleSubAction = std::make_shared<QHotkey>(QKeySequence("Shift+Alt+Down"), true, this);
    swithFlightAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+G"), true, this);
    goAroundAction = std::make_shared<QHotkey>(QKeySequence("Ctrl+Alt+Shift"), true, this);

    connect(sinkRateUpAction.get(), &QHotkey::activated, this,
            &WindowShortCut::sinkRateUpActionActivated);
    connect(sinkRateDownAction.get(), &QHotkey::activated, this,
            &WindowShortCut::sinkRateDownActionActivated);
    connect(lateralAddAction.get(), &QHotkey::activated, this,
            &WindowShortCut::lateralAddActionActivated);
    connect(lateralSubAction.get(), &QHotkey::activated, this,
            &WindowShortCut::lateralSubActionActivated);
    connect(pitchDown2DegAction.get(), &QHotkey::activated, this,
            &WindowShortCut::pitchDown2DegActionActivated);
    connect(pitchDown3DegAction.get(), &QHotkey::activated, this,
            &WindowShortCut::pitchDown3DegActionActivated);
    connect(pitchDown5DegAction.get(), &QHotkey::activated, this,
            &WindowShortCut::pitchDown5DegActionActivated);

    connect(stopSlideAction.get(), &QHotkey::activated, this,
            &WindowShortCut::stopSlideActionActivated);
    connect(enginePreAction.get(), &QHotkey::activated, this,
            &WindowShortCut::enginePreActionActivated);
    connect(emicPreAction.get(), &QHotkey::activated, this,
            &WindowShortCut::emicPreActionActivated);
    connect(brakeAddAction.get(), &QHotkey::activated, this,
            &WindowShortCut::brakeAddActionActivated);
    connect(brakeSubAction.get(), &QHotkey::activated, this,
            &WindowShortCut::brakeSubActionActivated);
    connect(brakeAdd2Action.get(), &QHotkey::activated, this,
            &WindowShortCut::brakeAdd2ActionActivated);
    connect(brakeSub2Action.get(), &QHotkey::activated, this,
            &WindowShortCut::brakeSub2ActionActivated);
    connect(engencyBrakeAction.get(), &QHotkey::activated, this,
            &WindowShortCut::engencyBrakeActionActivated);
    connect(adjustLeftAction.get(), &QHotkey::activated, this,
            &WindowShortCut::adjustLeftActionActivated);
    connect(adjustRightAction.get(), &QHotkey::activated, this,
            &WindowShortCut::adjustRightActionActivated);
    connect(throttleAddAction.get(), &QHotkey::activated, this,
            &WindowShortCut::throttleAddActionActivated);
    connect(throttleSubAction.get(), &QHotkey::activated, this,
            &WindowShortCut::throttleSubActionActivated);
    connect(swithFlightAction.get(), &QHotkey::activated, this,
            &WindowShortCut::swithFlightActionActivated);
    connect(goAroundAction.get(), &QHotkey::activated, this,
            &WindowShortCut::goAroundActionActivated);
}
