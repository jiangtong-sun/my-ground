// Qt核心UI模块：基础组件、窗口、布局、控件、对话框
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.0

// 自定义业务模块：席位角色（控制/非控制）、GcsQuick通用组件
import GcsStationRole 1.0
import GcsQuick 1.0

// 本地业务组件目录：各功能子系统（飞行任务、空管、数据链、发动机等）
import "FlightMissionSituation"
import "AirTrafficControlSystem"
import "DataLinkerSystem"
import "DeviceExecutorSystem"
import "EnginePowerSystem"
import "FlightManageSystem"
import "NavigationSensor"
import "HeadUpDisplay"

//主窗口核心定义
ApplicationWindow {
    // ========== 核心属性定义 ==========
    // 样式属性：主题色（灰色/背景色）
    property color grayStyle: GcsTheme.Gray //GcsTheme.green
    property color backgroundColor: GcsTheme.backgroundColor
    // 业务状态属性：席位角色（从C++的commandMgr获取）
    property int stationRole: commandMgr.stationRole
    // 数据模型属性：数据帧、飞行器遥控、指令组等（绑定C++的commandMgr/vechicleRC）
    // commandMgr在main函数中已经设置了上下文对应关系，对应GcsMFmcManager::instance()
    property var dataFrameModel: commandMgr.frameSet
    property var vechicleRC: commandMgr.vehicleRC
    property var sysCmdGroup: vechicleRC.commands.sysCmdGroup
    property var commandTypes: vechicleRC.commands.commandTypes
    // 状态缓存属性：上次选中的指令组、上次发送的指令
    property var lastCmdSetGroup: null
    property var lastSendCommand: null
    //TBD
    property var currVehicle: null
    property int menuModelSel: -1
    property int leftMenuGroupSel: 0
    property int rightMenuGroupSel: 0
    property int selectCmdGroup: 0
    //TBD
    property var leftDeviceGroup: null
    property var airTrafficControl: null
    property var flightManage: null
    property var navigateSensor:  null
    property var enginePower: null
    property var deviceExecutor: null
    property var dataLinker: null
    property var extraDevice: null
    property var devicePowerOnOff: null
    // 尺寸计算属性：基于窗口大小的动态尺寸（适配不同分辨率）
    property int lrCtlItemHeight: (fmcwindow.height - 2*headHeight - 20)/8 - 5
    property int winHeight: configMgr.fmcLocate.height  // 从配置读取窗口高度
    property int winWidth: configMgr.fmcLocate.width    // 从配置读取窗口宽度
    property int headHeight: 64 // 头部/底部面板高度
    property int lrOptWidth: 64 // 左右操作面板宽度
    property int firstHeight: fmcwindow.height/2    // 上半区高度
    property int middleWidth: fmcwindow.width*2/7   // 中间区域宽度
    property int rightWidth: (fmcwindow.width - middleWidth - lrOptWidth*2)/2   // 右侧宽度
    property int leftWidth: (fmcwindow.width - middleWidth - lrOptWidth*2)/2    // 左侧宽度
    property int secondHeight: fmcwindow.height - firstHeight - headHeight*2    // 下半区高度
    // 颜色映射属性：文本/状态颜色数组（适配不同告警级别）
    property var txtColorMap: new Array
    property var rmGreenColorMap: new Array
    // 其他状态属性：当前网络通道、初始化标记、底部操作指令
    property int currChannel: commandMgr.netChannel
    property bool initModelSelect: false
    property var footOptCmds: null

    // ========== 窗口基础配置 ==========
    id: fmcwindow
    visible: true   // 窗口可见
    title: qsTr("GCSMFMC")
    // 窗口标志：无边框 + 窗口 + 置顶（从配置读取）
    flags: Qt.FramelessWindowHint|Qt.Window|configMgr.staysOnTopHint //|Qt.WindowStaysOnTopHint
    // 窗口屏幕：从配置读取显示的屏幕ID（多屏适配）
    screen: Qt.application.screens[configMgr.fmcLocate.screenId]
    // 窗口位置：从配置读取，偏移1像素避免边缘遮挡
    x: screen.virtualX + configMgr.fmcLocate.xpos+1
    y: screen.virtualY + configMgr.fmcLocate.ypos
    // 窗口尺寸：从配置读取，宽度减1适配
    width: winWidth-1
    height: winHeight
    // 窗口背景色
    color: backgroundColor

    // ========== 核心业务函数 ==========
    // 1. 校验席位权限：非控制席位提示并返回false
    function noticeStationRole() {
        if (stationRole !== GcsStationRole.ControlRole) {
            toolbar.notice = qsTr("非控制席位，不执行遥控命令")
            return false
        }
        return true
    }

    // 2. 文本换行处理：超过2个字符时中间换行（适配UI显示）
    function insertNewlineUnicode(text) {
        const chars = Array.from(text);
        if (chars.length <= 2) {
            return text;
        }

        const mid = Math.floor(chars.length / 2);
        const firstHalf = chars.slice(0, mid).join("");
        const secondHalf = chars.slice(mid).join("");
        return firstHalf + "\n" + secondHalf;
    }

    // 3. 选择设备组：切换选中的飞行器设备组，更新数据帧和指令组
    function selectGroupDevice(index) {
        let deviceGroup = currVehicle.deviceList.get(index)
        if (deviceGroup) {
            menuModelSel = index                                // 更新菜单选中项
            leftDeviceGroup = deviceGroup                       // 缓存当前设备组
            leftDeviceGroup.selectIndex = 0                     // 重置设备组内选中项
            let selectNode = leftDeviceGroup.selectNode
            let mapGroupName = selectNode.mapGroup
            commandMgr.setDataFrame(mapGroupName)               // 通知C++更新数据帧
            vechicleRC.commands.updateSysCmdGroup(mapGroupName) // 更新系统指令组
            rightMenuGroupSel = 0                               // 重置右侧菜单选中项
            leftMenuGroupSel = 0                                // 重置左侧菜单选中项
            selectCmdGroup = 0                                  // 重置指令组选中项
        }
    }

    // 4. 默认监控页面：初始化默认设备和监控页
    function defaultMonitorPage() {
        currVehicle.deviceIdx = 0;
        leftDeviceGroup = currVehicle.deviceSelect
        if (leftDeviceGroup) {
            leftDeviceGroup.selectIndex = 0
            var selectNode = leftDeviceGroup.selectNode
            var mapGroupName = selectNode.mapGroup
            leftMenuGroupSel = 0
            menuModelSel = 0
        }
    }

    // 5. 物理发送指令：带确认弹窗（popup>0时），直接调用C++的sendControlCommand
    function physicalSendCommand(object) {
        if (object.popup > 0) {
             // 弹窗确认：设置标题、内容、图标，绑定确认/取消回调
            simpleMsgDialog.title = qsTr("发送确认");
            simpleMsgDialog.message = qsTr("是否发送【")+ object.name+qsTr("】指令");
            simpleMsgDialog.iconSource= ttfIcons["warning"]
            simpleMsgDialog.onSettingClickListener = ()=>{
                commandMgr.sendControlCommand(object);  // 调用C++发送指令
                simpleMsgDialog.close()
            }
            simpleMsgDialog.onCancelClickListener= ()=>{
                simpleMsgDialog.close()
                object.working = false                  // 重置指令状态
            }
            simpleMsgDialog.open()
        }
        else {
            // 无需确认，直接发送
            commandMgr.sendControlCommand(object);
        }
    }

    // ========== 信号连接（C++ → QML） ==========
    // 1. 监听硬件遥控点击事件（physicalRC是C++注册的硬件交互对象）
    Connections {
        target: physicalRC
        function onHardwareClickMessage(index, side) {
            // 顶部开关：选择设备组
            if (side === HardwareSwitchType.SwitchTop){
                if (currVehicle && index < currVehicle.deviceList.count) {
                    selectGroupDevice(index)
                }
            }

            // 左侧开关：选择设备组内节点，更新数据帧/指令组
            else if(side === HardwareSwitchType.SwitchLeft){
                if (leftDeviceGroup && index < leftDeviceGroup.elemCount){
                    let mapGroupName = leftDeviceGroup.deviceNodeName(index)
                    commandMgr.setDataFrame(mapGroupName) //
                    vechicleRC.commands.updateSysCmdGroup(mapGroupName)
                    leftDeviceGroup.selectIndex = index
                    leftMenuGroupSel = index
                    rightMenuGroupSel = 0
                }
            }
            // 右侧开关：选择指令组，更新指令并重置监控页
            else if(side === HardwareSwitchType.SwitchRight){
                if (commandTypes && index < commandTypes.count){
                    if (lastCmdSetGroup) {
                        lastCmdSetGroup.checked = false
                    }
                    let commandGroup = commandTypes.get(index)
                    if (commandGroup) {
                        commandGroup.checked = true
                        lastCmdSetGroup = commandGroup
                        var groupName = lastCmdSetGroup.groupName
                        vechicleRC.commands.updateSysCmdGroup(groupName)
                        vechicleRC.commands.uptExtCmdGroup(groupName)
                        vechicleRC.commands.selCmdGroup = index
                        currVehicle.setRcGroupName(groupName)
                        defaultMonitorPage()
                        rightMenuGroupSel = 2
                        selectCmdGroup = 1
                    }
                }
            }
            // 普通指令：发送选中的系统指令
            else if(side === HardwareSwitchType.NormCommand) {
                if(sysCmdGroup && index < sysCmdGroup.commands.count) {
                    let currCommand = sysCmdGroup.commands.get(index)
                    if (currCommand) {
                        currCommand.working = true
                        physicalSendCommand(currCommand)
                    }
                }
            }
        }
    }

    // 2. 监听commandMgr的状态变化（席位角色、地址不可用）
    Connections{
        target: commandMgr
        // 席位角色变化：更新提示信息
        function onStationRoleChanged() {
            if (stationRole !== GcsStationRole.ControlRole) {
                toolbar.notice = qsTr("非控制席位，不执行遥控命令")
            } else {
                toolbar.notice = qsTr("")
            }
        }
        // 地址不可用：更新工具栏提示
        function onAddressNotAvail(addr) {
            //toastNotice.show(addr, 3000)
            toolbar.notice = addr
        }
    }

    // ========== 组件生命周期 ==========
    // 1. 组件加载完成：初始化数据、颜色映射、默认飞行器
    Component.onCompleted: {
        // 初始化颜色映射数组（适配不同状态/告警）
        txtColorMap.push(GcsTheme.green)
        txtColorMap.push(GcsTheme.red)
        txtColorMap.push(GcsTheme.orange)
        txtColorMap.push(GcsTheme.yellow)
        txtColorMap.push(GcsTheme.dividerColor)
        txtColorMap.push(GcsTheme.primaryColor)
        rmGreenColorMap.push(GcsTheme.primaryColor)
        rmGreenColorMap.push(GcsTheme.dividerColor)
        rmGreenColorMap.push(GcsTheme.red)
        rmGreenColorMap.push(GcsTheme.orange)
        rmGreenColorMap.push(GcsTheme.yellow)
        rmGreenColorMap.push(GcsTheme.green)
        // 切换到第0个飞行器，初始化当前飞行器及子系统
        commandMgr.switchVehicel(0)
        currVehicle = commandMgr.currVehicle()
        devicePowerOnOff = currVehicle.devicePowerOnOff
        airTrafficControl = currVehicle.airTrafficControl
        flightManage = currVehicle.flightManage
        navigateSensor = currVehicle.navigateSensor
        enginePower = currVehicle.enginePower
        deviceExecutor = currVehicle.deviceExecutor
        dataLinker = currVehicle.dataLinker
        extraDevice = currVehicle.extraDevice
        // 标记初始化完成
        initModelSelect = true
    }

    // 2. 组件销毁：隐藏HUD和飞行任务面板
    Component.onDestruction: {
        hud.hide()
        fmp.hide()
    }

    // ========== 子组件实例化 ==========
    // 配置弹窗：居中显示，模态
    PopupSettingsPanel {
        id: settingPopup
        anchors.centerIn: parent
        modal: true
        width: 800
        height: 600
   }

    // 飞行任务场景面板
    FlightMissionSituation{
       id: fmp
    }

    // 抬头显示（HUD）面板
    HeadUpDisplay {
       id: hud
    }

    // 头部工具栏：含设置按钮，z:1保证置顶显示
    FmcFlightHeaderPanel {
        id: toolbar
        z: 1
        height: headHeight
        width: fmcwindow.width
        anchors.top: parent.top
        // 设置按钮点击回调：打开配置弹窗
        settingClickListener: () => {
            settingPopup.open()
        }
    }

    // 底部工具栏
    FlightFooterPanel {
        id: footbar
        height: headHeight
        width: fmcwindow.width
        anchors.bottom: parent.bottom
    }

    // 左侧操作面板：绑定尺寸，锚定到头部/底部面板之间
    LeftOperatePanel {
        id: all_dev_opt
        ctrlWidth: lrOptWidth
        ctrlHeight: fmcwindow.height - 2*headHeight
        anchors.top: toolbar.bottom
        anchors.bottom: footbar.bottom
        anchors.left: parent.left
    }

    // 右侧操作面板：同上
    RightOperatePanel{
        id: ctl_adjust_opt
        ctrlWidth: lrOptWidth
        ctrlHeight: fmcwindow.height - 2*headHeight
        anchors.top: toolbar.bottom
        anchors.right: parent.right
        anchors.bottom: footbar.bottom
    }

    // 提示弹窗：右侧显示，橙色文本
    ToastComponent {
        id: toastNotice
        txtColor: GcsTheme.orange
        anchors.right: parent.right
        anchors.rightMargin: 100
    }

    // 通用消息弹窗（指令确认、提示等）
    PopupContentPanel {
       id: simpleMsgDialog
    }

    // ========== 动态组件定义（按需加载） ==========
    // 执行器设备显示组件
    Component {
        id: xfexector
        XFDevExecutorDisplay{
            id:dev_panel
            anchors.left: parent.left
            ctrlWidth: leftWidth
            ctrlHeight: firstHeight
        }
    }

    // 中间主内容区域：左右操作面板之间的核心显示区
    Rectangle {
        anchors.left: all_dev_opt.right
        anchors.right: ctl_adjust_opt.left
        anchors.top: toolbar.bottom
        anchors.bottom: footbar.top

         // 上半区：固定显示HUD、执行器、电力系统
        Item {
            id: common
            height: firstHeight
            width: parent.width
            // HUD面板（中间）
            FlightHeadUpDisplay{
                id:hud_panel
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: rightWidth
                anchors.leftMargin: leftWidth
                ctrlWidth: middleWidth
                ctrlHeight: firstHeight
            }
            // 执行器面板（左侧）：动态加载xfexector组件
            Item {
                id:dev_panel
                anchors.left: parent.left
                width: leftWidth
                height: firstHeight
                GcsLoader {
                    sourceComponent: {
                        return xfexector;
                    }
                }
            }
            // 电力系统面板（右侧）
            FlightElectricSysDisplay{
                id: ele_panel
                anchors.right: parent.right
                ctrlWidth: rightWidth
                ctrlHeight: firstHeight
            }
        }
        // 各种业务监控面板定义（导航、发动机、数据链等）
        Component {
            id: monitorpanel
            GeneralMonitoringPanel {
                anchors.fill: parent
                id: generalMonitoringPanel
            }
        }

        Component{
            id: navinfopanel
            NavigationInformationPanel {
                anchors.fill: parent
                id: navigationComponent
                Component.onCompleted: {
                    var sysmonitor = navigateSensor ? navigateSensor.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        navigationComponent.setModePart(sysmonitor.getAllMode,
                                                        sysmonitor.countGroupElem,
                                                        36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component{
            id: atomsInfoPanel
            AtmosphericInforPanel {
                anchors.fill: parent
                id: atmosphericInfor
                Component.onCompleted: {
                    var sysmonitor = navigateSensor ? navigateSensor.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        atmosphericInfor.setModePart(sysmonitor.getAllMode,
                                                     sysmonitor.countGroupElem,
                                                     36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component{
            id: fcomputerpanel
            FlightComputerPanel {
                anchors.fill: parent
                id: fcomputerComp
                Component.onCompleted: {
                    var sysmonitor = navigateSensor ? navigateSensor.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        fcomputerComp.setModePart(sysmonitor.getAllMode,
                                                  sysmonitor.countGroupElem,
                                                  36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component{
            id: infoSourcePanel
            InformationSourcePanel {
                anchors.fill: parent
                id: infoSourceComp
                Component.onCompleted: {
                    var sysmonitor = navigateSensor ? navigateSensor.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        infoSourceComp.setModePart(sysmonitor.getAllMode,
                                                   sysmonitor.countGroupElem,
                                                   36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component{
            id: systemStatePanel
            SystemStateWordPanel {
                anchors.fill: parent
                id: systemStateComp
                Component.onCompleted: {
                    var sysmonitor = navigateSensor ? navigateSensor.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        systemStateComp.setModePart(sysmonitor.getAllMode,
                                                    sysmonitor.countGroupElem,
                                                    36, sysmonitor.subGroupStr)
                    }
                }
            }
        }


        Component {
            id: engineOilPanel
            EngineOilSystemPanel {
                anchors.fill: parent
                id: oilSystemComponent
                Component.onCompleted: {
                    var sysmonitor = enginePower ? enginePower.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        oilSystemComponent.setModePart(sysmonitor.getAllMode,
                                                       sysmonitor.countGroupElem,
                                                       36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component {
            id: elecPowerSysPanel
            ElecPowerSystemPanel {
                anchors.fill: parent
                id: powerSystemComp
                Component.onCompleted: {
                    var sysmonitor = enginePower ? enginePower.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        powerSystemComp.setModePart(sysmonitor.getAllMode,
                                                    sysmonitor.countGroupElem,
                                                    36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component {
            id: deviceSwitchPanel
            DeviceSwitchPanel {
                anchors.fill: parent
                id: deviceOnOffComp
                Component.onCompleted: {
                    var sysmonitor = enginePower ? enginePower.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        deviceOnOffComp.setModePart(sysmonitor.getAllMode,
                                                    sysmonitor.countGroupElem,
                                                    36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component {
            id: electricInfoPanel
            ElectricInformationPanel {
                anchors.fill: parent
                id: engineInfoComp
                Component.onCompleted: {
                    var sysmonitor = enginePower ? enginePower.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        engineInfoComp.setModePart(sysmonitor.getAllMode,
                                                   sysmonitor.countGroupElem,
                                                   36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component {
            id: takeofflandingPanel
            TakeoffLandingDevicePanel {
                anchors.fill: parent
                id: takeoffAndLanding
                Component.onCompleted: {
                    var sysmonitor = deviceExecutor ? deviceExecutor.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        takeoffAndLanding.setModePart(sysmonitor.getAllMode,
                                                      sysmonitor.countGroupElem,
                                                      36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component {
            id: engineDetailPanel
            EngineDetailInforPanel {
                anchors.fill: parent
                id: engineDetailComp
                Component.onCompleted: {
                    var sysmonitor = deviceExecutor ? deviceExecutor.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        engineDetailComp.setModePart(sysmonitor.getAllMode,
                                                     sysmonitor.countGroupElem,
                                                     36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component {
            id: displayInfoPanelC
            DisplayDetailInforPanelC {
                anchors.fill: parent
                id: displayPanelC
                Component.onCompleted: {
                    var sysmonitor = deviceExecutor ? deviceExecutor.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        displayPanelC.setModePart(sysmonitor.getAllMode,
                                                     sysmonitor.countGroupElem,
                                                     36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component {
            id: displayInfoPanelD
            DisplayDetailInforPanelD {
                anchors.fill: parent
                id: displayPanelD
                Component.onCompleted: {
                    var sysmonitor = deviceExecutor ? deviceExecutor.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        displayPanelD.setModePart(sysmonitor.getAllMode,
                                                     sysmonitor.countGroupElem,
                                                     36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component {
            id: displayInfoPanelE
            DisplayDetailInforPanelE {
                anchors.fill: parent
                id: displayPanelE
                Component.onCompleted: {
                    var sysmonitor = extraDevice ? extraDevice.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        displayPanelE.setModePart(sysmonitor.getAllMode,
                                                     sysmonitor.countGroupElem,
                                                     36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component {
            id: displayInfoPanelF
            DisplayDetailInforPanelF {
                anchors.fill: parent
                id: displayPanelF
                Component.onCompleted: {
                    var sysmonitor = extraDevice ? extraDevice.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        displayPanelF.setModePart(sysmonitor.getAllMode,
                                                     sysmonitor.countGroupElem,
                                                     36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component {
            id: displayInfoPanelG
            DisplayDetailInforPanelG {
                anchors.fill: parent
                id: displayPanelG
                Component.onCompleted: {
                    var sysmonitor = extraDevice ? extraDevice.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        displayPanelG.setModePart(sysmonitor.getAllMode,
                                                     sysmonitor.countGroupElem,
                                                     36, sysmonitor.subGroupStr)
                    }
                }
            }
        }


        Component {
            id: antennaMgrPanel
            AntennaManagementPanel {
                anchors.fill: parent
                id: antennaManagement
                Component.onCompleted: {
                    var sysmonitor = dataLinker ? dataLinker.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        antennaManagement.setModePart(sysmonitor.getAllMode,
                                                      sysmonitor.countGroupElem,
                                                      36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component{
            id: linkManagementPanel
            LinkManagementPanel {
                anchors.fill: parent
                id: linkManageComp
                Component.onCompleted: {
                    var sysmonitor = dataLinker ? dataLinker.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        linkManageComp.setModePart(sysmonitor.getAllMode,
                                                   sysmonitor.countGroupElem,
                                                   36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component {
            id: adsbDevPanel
            ADSBDevicePanel {
                anchors.fill: parent
                id: adsbDevicePanel
                Component.onCompleted: {
                    var sysmonitor = airTrafficControl ? airTrafficControl.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        adsbDevicePanel.setModePart(sysmonitor.getAllMode,
                                                    sysmonitor.countGroupElem,
                                                    36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component {
            id: deftTablePanel
            FlightDefaultTableDisplay {
                anchors.fill: parent
                id: deftTableComponent
                Component.onCompleted: {
                    var sysmonitor = deviceExecutor ? deviceExecutor.deviceNode(leftMenuGroupSel) : null;
                    if (sysmonitor) {
                        deftTableComponent.setModePart(sysmonitor.getAllMode, 36, sysmonitor.subGroupStr)
                    }
                }
            }
        }

        Component {
            id: c21CtrlRatePanel
            GcsControlRatePanel {
                anchors.fill: parent
                id: controlRatePanel
            }
        }

        Component {
            id: xfdevPowerPanel
            XFGcsDevicePowerOnOff {
                anchors.fill: parent
                id: controlDevPower
            }
        }

        Component {
            id: flightDataPanel
            FlightDataFrameDisplay {
                id: flightDataFrame
                anchors.fill: parent
            }
        }

        // 下半区：动态加载监控面板（根据menuModelSel/leftMenuGroupSel切换）
        Rectangle {
            color: backgroundColor
            anchors.bottom: parent.bottom
            anchors.top: common.bottom
            width: leftWidth + rightWidth + middleWidth
            height: secondHeight

            // 核心动态加载器：根据选中的菜单项切换面板
            GcsLoader {
                active: true
                anchors.fill: parent
                sourceComponent: {
                    // 根据menuModelSel（设备组）和leftMenuGroupSel（子项）切换面板
                    // 通用监控
                    if (menuModelSel == 0) {
                        return monitorpanel
                    }
                    // 导航传感器
                    if (menuModelSel == 1) {
                        if (leftMenuGroupSel == 0) {
                            return navinfopanel
                        }
                        if (leftMenuGroupSel == 1) {
                            return atomsInfoPanel
                        }
                        if (leftMenuGroupSel == 2) {
                            return fcomputerpanel
                        }
                        if (leftMenuGroupSel == 3) {
                            return infoSourcePanel
                        }
                        if (leftMenuGroupSel == 4) {
                            return systemStatePanel
                        }
                    }
                    //发动机
                    if (menuModelSel == 2) {
                        if (leftMenuGroupSel == 0) {
                            return engineOilPanel
                        }
                        if (leftMenuGroupSel == 1) {
                            return elecPowerSysPanel
                        }
                        if (leftMenuGroupSel == 2) {
                            return deviceSwitchPanel
                        }
                        if (leftMenuGroupSel == 3) {
                            return electricInfoPanel
                        }
                    }
                    if (menuModelSel == 3) {
                        if (leftMenuGroupSel == 0) {
                            return takeofflandingPanel
                        }
                        if (leftMenuGroupSel == 1) {
                            return engineDetailPanel
                        }
                        if (leftMenuGroupSel == 2) {
                            return displayInfoPanelC
                        }
                        if (leftMenuGroupSel == 3) {
                            return displayInfoPanelD
                        }
                    }
                    if (menuModelSel == 4) {
                        if (projectEnv === 2) {
                            if (leftMenuGroupSel == 0) {
                                return xfdevPowerPanel
                            }
                            if (leftMenuGroupSel == 1) {
                                return displayInfoPanelE
                            }
                            if (leftMenuGroupSel == 2) {
                                return displayInfoPanelF
                            }
                            if (leftMenuGroupSel == 3) {
                                return displayInfoPanelG
                            }
                        }
                        else if (projectEnv === 1) {
                            if (leftMenuGroupSel == 0) {
                                return displayInfoPanelE
                            }
                            if (leftMenuGroupSel == 1) {
                                return displayInfoPanelF
                            }
                            if (leftMenuGroupSel == 2) {
                                return displayInfoPanelG
                            }
                        }
                    }
                    if (menuModelSel == 5) {
                        if (leftMenuGroupSel == 0) {
                            return antennaMgrPanel
                        }
                        if (leftMenuGroupSel == 1) {
                            return linkManagementPanel
                        }
                        if (leftMenuGroupSel == 2) {
                            return c21CtrlRatePanel;
                        }
                    }
                    if (menuModelSel == 6 ) {
                        return adsbDevPanel
                    }
                    // 默认返回飞行数据面板
                    return flightDataPanel
                }
                // 面板加载完成：获取焦点
                onLoaded: {
                    item.focus = true
                    item.forceActiveFocus()
                }
            }
        }

        // 分割线：上下/左右区域分隔（美化+边界清晰）
        GcsDivider{
            anchors.bottom: parent.bottom
            borderColor: GcsTheme.itemGrayColor
            width: parent.width
            spacing: 0
        }

        GcsDivider{
            anchors.bottom: parent.top
            width: parent.width
            spacing: 0
        }

        GcsDivider {
            orientation: Qt.Vertical
            borderColor: GcsTheme.itemGrayColor
            anchors.right: parent.right
            height: parent.height
            spacing: 0
        }

        GcsDivider {
            borderColor: GcsTheme.itemGrayColor
            orientation: Qt.Vertical
            anchors.left: parent.left
            height: parent.height
            spacing: 0
        }
    }
}
