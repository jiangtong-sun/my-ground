#include "ActuatorItemDraw.h"
#include "AttitudeItemDraw.h"
#include "BatteryItemDraw.h"
#include "DataLinkItemDraw.h"
#include "ElectricItemDraw.h"
#include "EngineItemDraw.h"
#include "ExecutorItemDraw.h"
#include "GcsDataManager.h"
#include "GcsMFmcManager.h"
#include "MiniDmp.h"
#include "QQuickVideoView.h"
#include "SimpleItemDraw.h"
#include "TemperatureItem.h"
#include "TemperatureItemDraw.h"
#include "TerrainManager.h"
#include "VehicleSession.h"
#include "log_category.h"

#include <QApplication>
#include <QFontDatabase>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QTextCodec>
#include <QtQml/qqmlextensionplugin.h>

int main(int argc, char *argv[]) {
    // 1. 导入自定义Qt插件：GCSServiceProviderPlugin（扩展QML功能/业务服务）
    Q_IMPORT_PLUGIN(GCSServiceProviderPlugin)

    // 2. 设置未处理异常过滤器：捕获程序崩溃时的异常，生成MiniDump用于调试
    // MyUnhandledExceptionFilter是自定义的异常处理函数（在MiniDmp.h中声明）
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

    // 3. 获取程序运行路径：优先取应用程序目录，为空则取当前工作目录
    QString exepath = QApplication::applicationDirPath();
    if (exepath.isEmpty()) {
        exepath = QDir::currentPath();
    }

    // 注释掉的代码：设置QML场景图渲染后端为OpenGL RHI（硬件加速渲染）
    // QQuickWindow::setSceneGraphBackend(QSGRendererInterface::OpenGLRhi);

    // 4. 添加Qt库加载路径：指定GcsQuick目录为额外库路径（加载自定义QML插件/库）
    QCoreApplication::addLibraryPath(exepath + "/GcsQuick");
    // 设置应用组织名称（用于配置/缓存路径生成）
    QCoreApplication::setOrganizationName("Aifly");
    // 5. 初始化日志：将标准输出重定向到fmchudfmp.log文件（自定义spdlog日志）
    init_stdout_file_spdlog("fmchudfmp.log");
    // 6. 创建Qt应用程序实例：QApplication适配带GUI的桌面程序（区别于QGuiApplication）
    QApplication app(argc, argv);
    // 设置应用版本号
    app.setApplicationVersion(APP_VERSION);
    // 设置应用窗口图标（从Qt资源文件加载aifly.ico）
    app.setWindowIcon(QIcon(":/res/aifly.ico"));
    // 设置应用名称（转Unicode避免中文乱码）
    app.setApplicationName(transToUnicode("地面站指控系统"));
    // 设置应用显示名称（英文）
    app.setApplicationDisplayName(QString("GroundControlSystem"));
    // 设置组织名称（中文，转Unicode）
    app.setOrganizationName(transToUnicode("深圳艾飞智控科技有限公司"));
     // 打印应用启动日志（COMMON_INFO是自定义日志宏，在log_category.h中定义）
    COMMON_INFO("application {} start....", app.applicationVersion().toStdString());
    // 7. 加载自定义字体：从Qt资源文件加载iconfont.ttf（图标字体/自定义字体）
    QFontDatabase::addApplicationFont((":/res/iconfont.ttf"));
    // 8. 初始化QML引擎：QML界面的核心运行环境
    QQmlApplicationEngine engine;
    /**添加QML导入路径：指定qrc:/qmls为QML文件的导入根路径（QML中可直接import该路径下的组件）
     * qrc:/qmls，是 Qt 资源系统（Qt Resource System） 的路径格式，而非本地文件系统路径，跨平台无路径问题
     * qrc: 固定前缀，标识该路径指向 Qt 资源文件（.qrc）中的内容，而非硬盘上的文件
     * /qmls 资源文件中定义的 “虚拟目录名”，对应在gcsfmchudfmp.qrc 文件中配置的 prefix
     */
    engine.addImportPath("qrc:/qmls");

    // 9. 注册C++类型到QML：让QML可以识别并创建这些类的实例
    //TBD
    VehicleSession::registerType();
    //TBD
    FactDecoderMgr::registerType();
    //TBD
    QQuickVideoView::registerType();
    //TBD
    MissionManager::registerType();

    // 10. 注册自定义QML组件（命名空间+版本+QML中使用的名称）
    // 格式：qmlRegisterType<Cpp类名>("QML模块名", 主版本, 次版本, "QML中组件名");
    // 这类组件是UI绘制相关的可视化元素（如姿态、温度、引擎、电池等）
    //TBD
    qmlRegisterType<QuickCircleElem>("QuickCircleElem", 1, 0, "QuickCircleElem");
    //TBD
    qmlRegisterType<GcsStationRole>("GcsStationRole", 1, 0, "GcsStationRole");
    //TBD
    qmlRegisterType<ChannelSelection>("ChannelSelection", 1, 0, "ChannelSelection");
    //TBD
    qmlRegisterType<QuickAttitudeElem>("QuickAttitudeElem", 1, 0, "QuickAttitudeElem");
    //TBD
    qmlRegisterType<QuickTemperature>("QuickTemperature", 1, 0, "QuickTemperature");
    //TBD
    qmlRegisterType<QuickElectricElem3>("QuickElectricElem3", 1, 0, "QuickElectricElem3");
    //TBD
    qmlRegisterType<QuickElectricElem2>("QuickElectricElem2", 1, 0, "QuickElectricElem2");
    //TBD
    qmlRegisterType<QuickEngineElemNode2>("QuickEngineElemNode2", 1, 0, "QuickEngineElemNode2");
    //TBD
    qmlRegisterType<QuickEngineElemNode3>("QuickEngineElemNode3", 1, 0, "QuickEngineElemNode3");
    //TBD
    qmlRegisterType<QuickSimpleElemNode>("QuickSimpleElemNode", 1, 0, "QuickSimpleElemNode");
    //TBD
    qmlRegisterType<QuickBusbarElemNode>("QuickBusbarElemNode", 1, 0, "QuickBusbarElemNode");
    //TBD
    qmlRegisterType<QuickBatteryElemNode>("QuickBatteryElemNode", 1, 0, "QuickBatteryElemNode");
    //TBD
    qmlRegisterType<QuickEngineElemNode>("QuickEngineElemNode", 1, 0, "QuickEngineElemNode");
    //TBD
    qmlRegisterType<QuickExecutorElem>("QuickExecutorElem", 1, 0, "QuickExecutorElem");
    //TBD
    qmlRegisterType<QuickActuatorElem>("QuickActuatorElem", 1, 0, "QuickActuatorElem");
    //TBD
    qmlRegisterType<QuickElectricElem>("QuickElectricElem", 1, 0, "QuickElectricElem");
    //TBD
    qmlRegisterType<QuickEngineElem>("QuickEngineElem", 1, 0, "QuickEngineElem");
    //TBD
    qmlRegisterType<QuickDataLinkElem>("QuickDataLinkElem", 1, 0, "QuickDataLinkElem");
    //TBD
    qmlRegisterType<QuickTemperatureElem>("QuickTemperatureElem", 1, 0, "QuickTemperatureElem");
    //TBD
    qmlRegisterType<QuickBatteryElem>("QuickBatteryElem", 1, 0, "QuickBatteryElem");
    //TBD
    qmlRegisterType<QuickHBusbarElemNode>("QuickHBusbarElemNode", 1, 0, "QuickHBusbarElemNode");

    //11. 注册不可创建的C++类型到QML（仅用于QML中访问其枚举/静态属性，不能new实例）
    // 原因：这类是单例/核心管理类，通过上下文属性暴露实例，而非让QML创建
    //TBD
    qmlRegisterUncreatableType<GcsMFmcManager>("GcsMFmcManager", 1, 0, "GcsMFmcManager", "Reference");
    //TBD
    qmlRegisterUncreatableType<ConfigControllerExt>("ConfigControllerExt", 1, 0, "ConfigControllerExt", "Reference");
    //TBD
    qmlRegisterUncreatableType<DataRecordItem>("DataRecordItem", 1, 0, "DataRecordItem", "Reference");
    //TBD
    qmlRegisterUncreatableType<GcsDataManager>("GcsDataManager", 1, 0, "GcsDataManager", "Reference");

    // 12. 初始化地形数据：加载exepath/dem目录下的地形（DEM）数据
    TerrainThread::instance().loadTerrain(exepath + "/dem");

    // 13. 业务环境配置：设置项目环境标识（2为自定义业务环境，如试飞/正式环境）
    qint32 projectEvn = 0;
    projectEvn = 2;

    // 初始化配置文件路径
    QStringList adjustFiles;
    QStringList missionFiles;
    adjustFiles.append("xfres/adjustv2.xlsx");
    missionFiles.append(QStringLiteral("xfres/route.txt"));

    // 14. 加载核心业务配置（通过GcsMFmcManager单例）
    GcsMFmcManager::instance()->loadRCProto(adjustFiles, "xfres/controlv2.xlsx");
    GcsMFmcManager::instance()->loadExcelProto("xfres/protocal.xlsx");
    GcsMFmcManager::instance()->loadExcelWarn("xfres/warndefine.xlsx");
    GcsMFmcManager::instance()->reloadDisplayConfig("xfres/displayv2.xlsx");
    GcsMFmcManager::instance()->reloadTransConfig("xfres/transdatav2.xlsx");
    GcsMFmcManager::instance()->reloadNetComConfig("xfres/networkserail.ini");

    // 15. 获取配置控制器实例，并设置应用版本号
    ConfigControllerExt *cnfControl = nullptr;
    cnfControl = (ConfigControllerExt *) GcsMFmcManager::instance()->configCtrl();
    cnfControl->setAppVersion(app.applicationVersion());

    // 16. 设置QML根上下文属性：让QML可以直接访问这些C++对象/变量
    // 格式：setContextProperty("QML中访问的名称", C++对象/变量);
    engine.rootContext()->setContextProperty("projectEnv", projectEvn);
    engine.rootContext()->setContextProperty("configMgr", cnfControl);
    engine.rootContext()->setContextProperty("dataManager", GcsMFmcManager::instance()->dataManager());
    engine.rootContext()->setContextProperty("missionMgr", GcsMFmcManager::instance()->missionMgr());
    engine.rootContext()->setContextProperty("commandMgr", GcsMFmcManager::instance());
    engine.rootContext()->setContextProperty("ttfIcons", ConfigController::loadTTFDefine());

    // 17. 定义UI入口QML文件路径（Qt资源路径）：FlightMonitorControl.qml是整个UI的根组件
    const QUrl url(QStringLiteral("qrc:///qmls/FlightMonitorControl.qml"));

    // 18. 初始化车辆环境、MFmc管理器、启用保活机制、配置服务
    //初始化飞机信息，如飞机ID，TBD
    GcsMFmcManager::instance()->loadVehicleEnv(missionFiles.count(), "");
    //TBD
    GcsMFmcManager::instance()->initlialize(missionFiles);
    //启用上行心跳，但是实际这块并没有开关效果TBD
    GcsMFmcManager::instance()->enableUpKeepalive();
    //TBD
    GcsMFmcManager::instance()->configService();

    // 19. 加载QML入口文件：启动UI界面
    engine.load(url);

    // 20. 运行应用程序事件循环：阻塞直到程序退出，返回退出码
    int rnt = app.exec();

    // 21. 程序退出时的资源清理
    GcsMFmcManager::instance()->release();      // 释放MFmc管理器资源
    GcsMFmcManager::instance()->stopService();  // 停止业务服务
    TerrainThread::instance().close();          // 关闭地形线程
    COMMON_ERROR("close application");          // 打印程序退出日志

    // 返回退出码
    return rnt;
}
