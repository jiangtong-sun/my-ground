#include "GCSServiceProviderPlugin.h"
#include "GCSTiledManagerEngine.h"

//构造函数：创建插件对象时自动执行
//作用：初始化插件对象，同时打印调试信息（函数名+对象内存地址），方便调试追踪对象创建
//Q_FUNC_INFO：Qt宏，自动替换为当前函数的完整名称
// this：指向当前对象的指针，打印出对象的内存地址，区分不同插件实例
GCSServiceProviderPlugin::GCSServiceProviderPlugin() { qDebug() << Q_FUNC_INFO << this; }

//析构函数：插件对象被销毁时自动执行
//作用：清理插件资源（当前无额外资源需清理），打印调试信息追踪对象销毁
GCSServiceProviderPlugin::~GCSServiceProviderPlugin() { qDebug() << Q_FUNC_INFO << this; }

/**
 * @brief 创建地理编码引擎（地址↔经纬度转换功能）
 * @param parameters 插件配置参数（如API密钥、服务地址等，当前未使用）
 * @param error 输出参数：返回错误类型（调用方传入的指针，可能为nullptr）
 * @param errorString 输出参数：返回错误描述（调用方传入的指针，可能为nullptr）
 * @return QGeoCodingManagerEngine* 地理编码引擎指针（当前返回nullptr表示不支持）
 * @note const：该函数不会修改类的任何成员变量（只读函数）
 */
QGeoCodingManagerEngine *GCSServiceProviderPlugin::createGeocodingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const {
    // Q_UNUSED：告诉编译器"该参数声明但未使用，无需报警告"（避免编译警告）
    Q_UNUSED(parameters);
    // 检查error指针是否非空（防止空指针解引用导致程序崩溃）
    if (error) {
        // *error：指针解引用，修改error指向的变量值
        // NotSupportedError：Qt枚举值，表示"该功能不支持"
        *error = QGeoServiceProvider::NotSupportedError;
    }
    // 检查错误描述指针是否非空
    if (errorString) {
        // 设置错误描述：地理编码功能不支持
        *errorString = "Geocoding Not Supported";
    }
    // 返回空指针：表示地理编码引擎创建失败（功能不支持）
    return nullptr;
}

/**
 * @brief 创建地图渲染引擎（核心功能：地图绘制、缩放、平移等）
 * @param parameters 插件配置参数（传递给自定义地图引擎）
 * @param error 输出参数：返回错误类型
 * @param errorString 输出参数：返回错误描述
 * @return QGeoMappingManagerEngine* 自定义地图引擎指针（成功返回对象地址，失败返回nullptr）
 */
QGeoMappingManagerEngine *GCSServiceProviderPlugin::createMappingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const {
     // 检查error指针非空，设置为"无错误"（NoError是Qt枚举值）
    if (error) {
        *error = QGeoServiceProvider::NoError;
    }
    // 检查错误描述指针非空，设置为空字符串（表示无错误）
    if (errorString) {
        *errorString = "";
    }
    // new：在堆内存创建自定义地图引擎对象（堆对象不会随函数结束销毁，可返回给调用方）
    // GCSTiledMappingManagerEngine：自定义的瓦片地图引擎类，传入配置参数和错误输出参数
    // 返回新创建的引擎对象指针，调用方可通过该指针使用地图渲染功能
    return new GCSTiledMappingManagerEngine(parameters, error, errorString);
}

QGeoRoutingManagerEngine *GCSServiceProviderPlugin::createRoutingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const {
    Q_UNUSED(parameters);
    if (error) {
        *error = QGeoServiceProvider::NotSupportedError;
    }
    if (errorString) {
        *errorString = "Routing Not Supported";
    }
    return nullptr;
}

QPlaceManagerEngine *GCSServiceProviderPlugin::createPlaceManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const {
    Q_UNUSED(parameters);
    if (error) {
        *error = QGeoServiceProvider::NotSupportedError;
    }
    if (errorString) {
        *errorString = "Place Not Supported";
    }
    return nullptr;
}

/**
 * @brief 设置QML引擎指针（供插件内部访问QML上下文/创建QML组件）
 * @param engine 外部传入的QML引擎指针
 * @note m_engine是类的私有成员，外部无法直接修改，通过该公开函数赋值（封装特性）
 */
// 将传入的QML引擎指针赋值给类的私有成员m_engine，供插件内部使用
void GCSServiceProviderPlugin::setQmlEngine(QQmlEngine *engine) { m_engine = engine; }
