#include "GCSTiledManagerEngine.h"
#include "GCSMapEngine.h"
#include "GeoTileFetcher.h"
#include "MapProvider.h"
#include "MapUrlEngine.h"
#include "log_category.h"

// 引入Qt Location私有头文件（_p.h表示私有API，实现瓦片地图底层功能，需Qt源码/私有库支持）
// 相机能力：控制地图缩放、旋转等相机行为
#include <QtLocation/private/qgeocameracapabilities_p.h>
// 文件瓦片缓存：管理本地磁盘/内存中的瓦片缓存
#include <QtLocation/private/qgeofiletilecache_p.h>
// 地图类型：定义地图的样式（普通/卫星/地形等）、名称等属性
#include <QtLocation/private/qgeomaptype_p.h>
// 瓦片地图核心类：Qt瓦片地图的底层实现基类
#include <QtLocation/private/qgeotiledmap_p.h>

// Qt核心头文件：处理目录/文件操作
#include <QtCore/QDir>

/**
 * @brief GCSGeoTiledMap构造函数：自定义瓦片地图对象的构造
 * @param engine 所属的地图引擎指针（父类需要）
 * @param parent 父对象（Qt父子对象机制，用于内存自动管理）
 * @note 继承自QGeoTiledMap（Qt瓦片地图基类），仅调用父类构造，无额外逻辑
 */
GCSGeoTiledMap::GCSGeoTiledMap(QGeoTiledMappingManagerEngine *engine, QObject *parent)
    : QGeoTiledMap(engine, parent) {}

/**
 * @brief GCSTiledMappingManagerEngine构造函数：核心初始化逻辑
 * @param parameters 插件配置参数（如useragent、缓存路径等）
 * @param error 输出参数：返回引擎创建的错误类型
 * @param errorString 输出参数：返回错误描述
 * @note 继承自QGeoTiledMappingManagerEngine（Qt瓦片地图引擎基类），是地图渲染的核心初始化入口
 */
GCSTiledMappingManagerEngine::GCSTiledMappingManagerEngine(const QVariantMap &parameters,
                                                           QGeoServiceProvider::Error *error,
                                                           QString *errorString)
    : QGeoTiledMappingManagerEngine() {
    // 打印当前函数名（调试用），qInfo()是Qt信息级日志
    qInfo() << Q_FUNC_INFO;

    // 初始化GCS地图引擎核心（加载地图配置、提供者等基础数据）
    getGCSMapEngine()->init();

    // ========== 步骤1：配置地图相机能力（控制缩放、旋转等） ==========
    // QGeoCameraCapabilities：管理地图相机的能力（缩放范围、是否支持旋转等）
    QGeoCameraCapabilities cameraCaps;
    // 设置最小缩放级别（2级，数值越小地图越宏观）
    cameraCaps.setMinimumZoomLevel(2.0);
    // 设置最大缩放级别（20级，数值越大地图越精细）
    cameraCaps.setMaximumZoomLevel(20);
    // 支持地图方位角（即地图可以旋转）
    cameraCaps.setSupportsBearing(true);
    // 将相机能力设置到引擎中，生效配置
    setCameraCapabilities(cameraCaps);

    // ========== 步骤2：设置瓦片大小 ==========
    // 地图瓦片的标准尺寸：256x256像素（绝大多数地图服务商的瓦片尺寸）
    setTileSize(QSize(256, 256));

    // ========== 宏定义：简化QGeoMapType对象的创建 ==========
    // QGCGEOMAPTYPE：自定义宏，封装QGeoMapType的构造参数，减少重复代码
    // 参数说明：
    // a-地图样式 b-名称 c-描述 d-是否灰度 e-是否夜间模式 f-唯一ID
    // QByteArray("cdmcoc")：自定义地图标识（可理解为引擎ID）
    // cameraCaps：关联上面配置的相机能力
#define QGCGEOMAPTYPE(a, b, c, d, e, f)                                                            \
    QGeoMapType(a, b, c, d, e, f, QByteArray("cdmcoc"), cameraCaps)

    // ========== 步骤3：加载所有地图提供者，生成地图类型列表 ==========
    // 初始化Qt地图ID（用于区分不同地图类型/服务商）
    int qtMapId = 1;
    // 获取地图URL引擎（用于生成瓦片请求URL、管理地图提供者）
    auto MapUrlEngine = getGCSMapEngine()->mapUrlEngine();
    // 根据ID获取第一个地图提供者（如高德地图、百度地图等）
    MapProvider *provider = MapUrlEngine->getMapProviderFromQtMapId(qtMapId);
    // 存储所有支持的地图类型列表
    QList<QGeoMapType> mapList;

    // 循环遍历所有地图提供者（直到provider为nullptr，表示无更多提供者）
    while (provider) {
        // 获取当前地图提供者的类型（如"普通地图"、"卫星地图"）
        QString providerType = MapUrlEngine->getProviderTypeFromQtMapId(qtMapId);
        // 向列表中添加当前地图类型（通过自定义宏快速创建QGeoMapType对象）
        mapList.append(QGCGEOMAPTYPE(provider->getMapStyle(), providerType, providerType, false,
                                     false, qtMapId++));
        // 获取下一个地图提供者（ID自增）
        provider = MapUrlEngine->getMapProviderFromQtMapId(qtMapId);
    }

    // 将加载的所有地图类型设置到引擎中，使地图类型生效（如UI中可切换不同地图）
    setSupportedMapTypes(mapList);

    // ========== 步骤4：设置HTTP请求的User-Agent（可选配置） ==========
    // 检查配置参数中是否包含"useragent"（HTTP请求头的用户代理）
    if (parameters.contains(QStringLiteral("useragent"))) {
        // 从参数中提取useragent，并设置到GCS地图引擎中（用于瓦片请求的HTTP头）
        getGCSMapEngine()->setUserAgent(
            parameters.value(QStringLiteral("useragent")).toString().toLatin1());
    }

    // ========== 步骤5：配置地图瓦片缓存（磁盘+内存） ==========
    _setCache(parameters);

    // ========== 步骤6：设置瓦片获取器 ==========
    // 创建自定义瓦片获取器（负责根据URL获取瓦片数据：网络下载/本地缓存读取）
    // this：将引擎设为瓦片获取器的父对象，利用Qt父子机制自动管理内存
    setTileFetcher(new GeoTileFetcher(this));

    // ========== 步骤7：设置引擎创建结果（无错误） ==========
    // 设置错误类型为"无错误"
    *error = QGeoServiceProvider::NoError;

    // 清空错误描述字符串（表示创建成功）
    errorString->clear();
}

/**
 * @brief 析构函数：销毁地图引擎对象
 * @note 当前无自定义清理逻辑（Qt父子对象机制会自动销毁子对象：如瓦片获取器、缓存等）
 */
GCSTiledMappingManagerEngine::~GCSTiledMappingManagerEngine() {}

/**
 * @brief 创建自定义瓦片地图对象
 * @return QGeoMap* 自定义地图对象指针（供Qt Location框架调用，渲染地图）
 * @note 重写父类纯虚函数，必须实现；返回自定义的GCSGeoTiledMap对象
 */
QGeoMap *GCSTiledMappingManagerEngine::createMap() { return new GCSGeoTiledMap(this); }

/**
 * @brief 配置地图瓦片缓存（磁盘缓存路径、内存/磁盘缓存大小限制）
 * @param parameters 插件配置参数（未直接使用，预留扩展）
 * @note 核心作用：创建缓存目录、初始化瓦片缓存对象、设置缓存大小限制
 */
void GCSTiledMappingManagerEngine::_setCache(const QVariantMap &parameters) {
    // 检查缓存是否被重置（如数据库升级），输出警告日志
    if (getGCSMapEngine()->wasCacheReset()) {
        COMMON_WARN("The Offline Map Cache database has been upgraded. ");
    }

    // ========== 步骤1：获取并创建缓存目录 ==========
    // 获取GCS引擎配置的缓存路径（自定义的瓦片缓存目录）
    QString cacheDir = getGCSMapEngine()->getCachePath();
    // 检查缓存目录是否存在
    if (!QFileInfo::exists(cacheDir)) {
        // 不存在则创建目录（QDir::root().mkpath：创建多级目录，如a/b/c）
        if (!QDir::root().mkpath(cacheDir)) {
            // 创建失败，输出警告日志，并切换到默认缓存路径（用户主目录下的.gcsmapscache）
            COMMON_WARN("Could not create mapping disk cache directory  {0}",
                        cacheDir.toStdString());
            cacheDir = QDir::homePath() + QStringLiteral("/.gcsmapscache/");
        }
    }

    // 再次检查默认缓存路径是否存在（防止第一次创建失败）
    if (!QFileInfo::exists(cacheDir)) {
        if (!QDir::root().mkpath(cacheDir)) {
            // 仍创建失败，输出警告，清空缓存路径（表示禁用磁盘缓存）
            COMMON_WARN("Could not create mapping disk cache directory  {0} ",
                        cacheDir.toStdString());
            cacheDir.clear();
        }
    }

    // ========== 步骤2：设置内存缓存大小限制 ==========
    // 获取GCS引擎配置的最大内存缓存（单位：MB），转换为字节（*1024*1024）
    uint32_t memLimit = getGCSMapEngine()->getMaxMemCache() * (1024 * 1024);

    // 内存缓存下限：最小1MB（防止配置过小导致缓存失效）
    if (memLimit < 1024 * 1024) {
        memLimit = 1024 * 1024;
    }

    // 内存缓存上限：最大1GB（防止配置过大占用过多内存）
    if (memLimit > 1024 * 1024 * 1024) {
        memLimit = 1024 * 1024 * 1024;
    }

    // ========== 步骤3：创建瓦片缓存对象并配置 ==========
    // 创建Qt文件瓦片缓存对象（管理磁盘+内存中的瓦片缓存）
    // 参数：缓存目录（为空则禁用磁盘缓存）
    QAbstractGeoTileCache *pTileCache = new QGeoFileTileCache(cacheDir);

    // 将缓存对象设置到引擎中，生效缓存配置
    setTileCache(pTileCache);

    if (pTileCache) {
        //-- We're basically telling it to use 100k of disk for cache. It doesn't like
        //   values smaller than that and I could not find a way to make it NOT cache.
        //   We handle our own disk caching elsewhere.

        // 设置磁盘缓存最大占用：100KB（注释说明：Qt不支持更小的值，实际自定义缓存逻辑在别处）
        // 注：这里仅为满足Qt框架要求，实际磁盘缓存由GCS引擎自己管理
        pTileCache->setMaxDiskUsage(1024 * 100);
        // 设置内存缓存最大占用：上面计算的memLimit（1MB~1GB）
        pTileCache->setMaxMemoryUsage(memLimit);
    }
}
