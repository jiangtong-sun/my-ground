#pragma once

#include "QHosKey/qhotkey.h"
#include "VehicleCommand.h"
#include "concurrentqueue.h"

class WindowShortCut : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(WindowShortCut)

  public:
    RCCommandItem *nextControl();
    RCAdjustItem *nextAdjust();
    RCAdjustItem *nextAdjustExt();
    void registerWinHotKey(VehicleCommand *allcommands);
    void removeHotKey();
    WindowShortCut();
    ~WindowShortCut();

  private slots:
    void stopSlideActionActivated();
    void enginePreActionActivated();
    void emicPreActionActivated();
    void brakeAddActionActivated();
    void brakeSubActionActivated();
    void brakeAdd2ActionActivated();
    void brakeSub2ActionActivated();
    void engencyBrakeActionActivated();
    void adjustLeftActionActivated();
    void adjustRightActionActivated();
    void throttleAddActionActivated();
    void throttleSubActionActivated();
    void swithFlightActionActivated();
    void goAroundActionActivated();

    void sinkRateUpActionActivated();
    void sinkRateDownActionActivated();
    void lateralAddActionActivated();
    void lateralSubActionActivated();
    void pitchDown2DegActionActivated();
    void pitchDown3DegActionActivated();
    void pitchDown5DegActionActivated();

  protected:
    VehicleCommand *adjust_control_cmds = nullptr;
    std::shared_ptr<QHotkey> stopSlideAction = nullptr;
    std::shared_ptr<QHotkey> enginePreAction = nullptr;
    std::shared_ptr<QHotkey> emicPreAction = nullptr;
    std::shared_ptr<QHotkey> brakeAddAction = nullptr;
    std::shared_ptr<QHotkey> brakeSubAction = nullptr;
    std::shared_ptr<QHotkey> brakeAdd2Action = nullptr;
    std::shared_ptr<QHotkey> brakeSub2Action = nullptr;
    std::shared_ptr<QHotkey> engencyBrakeAction = nullptr;
    std::shared_ptr<QHotkey> adjustLeftAction = nullptr;
    std::shared_ptr<QHotkey> adjustRightAction = nullptr;
    std::shared_ptr<QHotkey> throttleAddAction = nullptr;
    std::shared_ptr<QHotkey> throttleSubAction = nullptr;
    std::shared_ptr<QHotkey> swithFlightAction = nullptr;
    std::shared_ptr<QHotkey> goAroundAction = nullptr;

    std::shared_ptr<QHotkey> sinkRateUpAction = nullptr;
    std::shared_ptr<QHotkey> sinkRateDownAction = nullptr;
    std::shared_ptr<QHotkey> lateralAddAction = nullptr;
    std::shared_ptr<QHotkey> lateralSubAction = nullptr;
    std::shared_ptr<QHotkey> pitchDown2DegAction = nullptr;
    std::shared_ptr<QHotkey> pitchDown3DegAction = nullptr;
    std::shared_ptr<QHotkey> pitchDown5DegAction = nullptr;

    uint64_t cal_next_time = 0;
    typedef moodycamel::ConcurrentQueue<uint32_t> NextControlCommandQueue;
    typedef moodycamel::ConcurrentQueue<uint32_t> NextAdjustCommandQueue;
    NextControlCommandQueue commandQueue;
    NextAdjustCommandQueue adjustQueue;
    NextAdjustCommandQueue adjustExtQueue;
};
