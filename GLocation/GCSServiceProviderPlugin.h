#pragma once    // 防止头文件重复包含（替代传统的#ifndef/#define/#endif）

#include <QtCore/QLoggingCategory>  // Qt日志分类，用于精细化控制日志输出
#include <QtCore/QObject>           // Qt核心对象类，提供信号槽、元对象系统等核心能力
#include <QtCore/QtPlugin>          // Qt插件系统核心头文件，支持插件创建/加载
#include <QtLocation/QGeoServiceProviderFactory>    // 地理服务接口

/*
 * 基于Qt框架（Qt Location 模块）编写的一个地理信息服务插件类的头文件
 * 核心目的是实现自定义的 Qt 地理服务插件
 * 用于创建各类地理相关的功能引擎（如地图渲染、地理编码、路径规划等）
 */

/*类定义
 * 1.该类是 Qt Location 模块的地理服务插件核心类，通过实现QGeoServiceProviderFactory接口，为应用提供自定义的地图、地理编码、路径规划等地理服务能力
 * 2.核心宏（Q_OBJECT/Q_INTERFACES/Q_PLUGIN_METADATA）是 Qt 插件生效的关键，缺一不可，且需要配合 moc 工具编译
 * 3.重写的四个createXXXEngine方法是功能实现的核心，需在.cpp 文件中具体实现，才能提供对应的地理服务功能
 */
class GCSServiceProviderPlugin : public QObject, public QGeoServiceProviderFactory {
    Q_OBJECT    // 启用Qt元对象系统（信号槽、动态类型转换、反射等）
    // 声明类实现了该接口，让Qt插件系统识别，加载插件时能通过接口找到对应的实现类
    Q_INTERFACES(QGeoServiceProviderFactory)
    /*插件的元数据配置：
     * IID：接口的唯一标识符（Qt 官方定义的QGeoServiceProviderFactory接口 ID），插件加载时通过这个 ID 匹配接口
     * FILE：指定插件的元数据文件（qgc_maps_plugin.json），该文件通常包含插件名称、版本、支持的服务类型等信息
     */
    Q_PLUGIN_METADATA(IID "org.qt-project.qt.geoservice.serviceproviderfactory/5.0" FILE "qgc_maps_plugin.json")   // 插件元数据

  public:
    // 构造/析构函数：声明默认构造和析构，需在.cpp文件中实现
    GCSServiceProviderPlugin();
    ~GCSServiceProviderPlugin();

    // 重写QGeoServiceProviderFactory的纯虚函数（final表示禁止派生类重写）
    //QGeoCodingManagerEngine *：函数的返回值类型，表示这个函数执行完会返回一个QGeoCodingManagerEngine类型的指针
    // 1. 创建地理编码引擎：地址↔经纬度转换（如“北京市朝阳区”转经纬度）
    QGeoCodingManagerEngine *createGeocodingManagerEngine(const QVariantMap &parameters,
                                                          QGeoServiceProvider::Error *error,
                                                          QString *errorString) const final;
    // 2. 创建地图渲染引擎：负责地图绘制、缩放、平移等核心显示功能
    QGeoMappingManagerEngine *createMappingManagerEngine(const QVariantMap &parameters,
                                                         QGeoServiceProvider::Error *error,
                                                         QString *errorString) const final;
    // 3. 创建路径规划引擎：计算两点间最优路径（驾车/步行/公交等）
    QGeoRoutingManagerEngine *createRoutingManagerEngine(const QVariantMap &parameters,
                                                         QGeoServiceProvider::Error *error,
                                                         QString *errorString) const final;
    // 4. 创建地点管理引擎：管理POI（兴趣点）、地点搜索、地点详情等
    QPlaceManagerEngine *createPlaceManagerEngine(const QVariantMap &parameters,
                                                  QGeoServiceProvider::Error *error,
                                                  QString *errorString) const final;
    // 自定义函数：设置QML引擎指针，用于插件中访问QML上下文/创建QML组件
    void setQmlEngine(QQmlEngine *engine);

  private:
    // 私有成员：存储QML引擎指针，初始化为nullptr
    QQmlEngine *m_engine = nullptr;
};
