/**
 * @file factdecodermgr.h
 * @brief 协议解析核心管理类头文件
 * @note 该类是协议解析模块的核心入口，负责协议文件加载、原始数据解析、解码、
 *       数据存储/通知、单元测试等全流程管理，适配多线程并发场景，支持QML集成
 */
#pragma once    // 替代传统的头文件保护宏，防止头文件重复包含

// 第三方并发队列头文件：用于多线程安全的协议消息队列管理
#include "concurrentqueue.h"
// 协议解析结果数据模型：存储解析后的协议帧数据（适配Qt Model-View架构）
#include "facttablemodel.h"
// 网络数据相关定义：包含通道配置、网络地址等结构体
#include "networkdata.h"
// 协议文件解析类：负责解析Excel格式的协议定义文件
#include "parseprotodef.h"
// 协议消息结构体：封装原始协议包的核心数据（数据缓冲区、长度、通道等）
#include "protomessage.h"
// 不可拷贝基类：禁止子类的拷贝构造和赋值运算符，避免多线程资源竞争
#include "uncopyable.h"

// Qt 日期时间类：用于记录协议文件时间戳
#include <QDateTime>
// Qt QML 集成头文件：用于将类注册到QML引擎，支持前端调用
#include <QtQml/qqml.h>

/**
 * @brief 遥测数据统计回调函数类型定义
 * @param veh 车辆ID（车辆唯一标识）
 * @param datas 本次解析的数据量（字节数/条目数）
 * @param frame 协议帧号（对应协议定义中的帧类型）
 * @note 用于协议解析完成后，回调外部逻辑统计遥测数据（如上报数据量、监控解析效率）
 */
//using TBD TelemetrySummary
using TelemetrySummary = std::function<void(int veh, int datas, int frame)>;

/**
 * @class FactDecoderMgr
 * @brief 车辆协议解析核心管理类
 * @details 负责协议文件加载、原始二进制数据解析、协议包构建、解码、数据存储、
 *          告警解析、数据更新通知、单元测试等核心功能，具备以下特性：
 *          1. 不可拷贝：继承Uncopyable，避免多线程下浅拷贝导致的资源冲突
 *          2. 多线程安全：使用原子变量、并发队列保证线程安全
 *          3. Qt集成：支持注册到QML引擎，数据模型适配Qt Model-View架构
 *          4. 动态库导出：BASEPROTO_EXPORT宏使其可被外部模块调用
 * @extends Uncopyable 禁止拷贝的基类
 * @author （可补充作者信息）
 * @date （可补充日期）
 */
//TBD BASEPROTO_EXPORT是什么？？class是什么？？
class BASEPROTO_EXPORT FactDecoderMgr : public Uncopyable {

  public:
    ///
    /// \brief FactDecoderMgr
    /// \param idx 车辆ID
    /// \param tm 协议输出文件时间
    ///TBD构造函数和析构函数的作用？
    FactDecoderMgr(uint8_t idx, QDateTime tm);

    ~FactDecoderMgr();

    /// 将协议包数据解析，解析后的数据存储到FactTableModel对象中
    /// 解析后的数据通过消息告知监听对象进行数据后处理
    ///
    bool protoDecoder(ProtoMessage *msg);

    /**
     * @brief 通知数据更新
     * @param first 告警检查回调函数（处理数据更新后的告警逻辑）
     * @param chl 数据通道号（标识数据来源的网络通道/串口通道）
     * @note 数据解析完成后调用，触发外部监听者的回调逻辑，如更新QML界面、上报数据
     */
    void notifyDataUpdate(CheckWarnCallback first, int chl);
    ///
    /// \brief initSummary 初始化遥测数据统计回调
    /// \param summary 统计遥测数据回调函数
    ///@note 设置外部传入的统计回调，协议解析完成后会调用该函数上报统计数据
    ///
    void initSummary(TelemetrySummary summary);

    /**
     * @brief 注册类到QML引擎
     * @note 静态方法，用于将FactDecoderMgr类注册到QML上下文，使QML前端可以调用
     *       该类的方法和属性（需配合qmlRegisterType使用）
     */
    static void registerType();

    /**
     * @brief 协议解析单元测试（加载文件）
     * @param file 测试文件路径（QString类型，支持Excel/二进制文件）
     * @param row 测试数据行号（uint32_t，指定读取文件的第几行数据）
     * @return 提取的测试数据（QByteArray类型，二进制原始数据）
     * @note 用于单元测试阶段，从指定文件中提取指定行的原始数据，供后续解码测试
     */
    static QByteArray unitTest(const QString &file, uint32_t row);

    /**
     * @brief 协议解码单元测试（带通道配置）
     * @param ctxrand 随机上下文值（模拟不同的解析上下文，如不同车辆状态）
     * @param jump 帧跳转步长（模拟协议帧的间隔/跳帧场景）
     * @param type 协议类型（对应Excel协议定义中的帧类型）
     * @param config 通道配置结构体（包含IP、端口、协议类型等通道参数）
     * @note 模拟指定通道配置下的协议解码，验证解码逻辑的正确性
     */
    void unitTestDecoder(int ctxrand, uint8_t jump, int type, ChannelConfig &config);

    /**
     * @brief 协议解码单元测试（重载：无通道配置）
     * @param ctxrand 随机上下文值
     * @param jump 帧跳转步长
     * @param type 协议类型
     * @note 使用默认通道配置进行解码测试，简化测试调用
     */
    void unitTestDecoder(int ctxrand, uint8_t jump, int type);

    /**
     * @brief 协议解码单元测试（重载：指定通道）
     * @param ctxrand 随机上下文值（uint8_t版本）
     * @param chl 通道号
     * @note 仅指定通道和上下文的简化测试，验证指定通道的解码逻辑
     */
    void unitTestDecoder(uint8_t ctxrand, int chl);

    /**
     * @brief Proto协议包单元测试
     * @param frame 协议帧号
     * @param ctxrand 随机上下文值
     * @note 模拟指定帧号的Proto协议包构建和解码，验证协议包封装逻辑
     */
    void unitTestProto(int frame, int ctxrand);

    /**
     * @brief 网络协议单元测试
     * @param chl 通道号
     * @param config 通道配置结构体（包含IP、端口、多播标识等）
     * @note 模拟指定通道的网络数据收发，验证网络层协议解析的正确性
     */
    void unitTestNetwork(int chl, ChannelConfig &config);

    /**
     * @brief 关闭协议数据保存功能
     * @note 停止将解析后的协议数据写入文件/数据库，释放相关文件句柄/数据库连接
     */
    void closeFactSave();

    /**
     * @brief 启用指定通道的协议数据保存功能
     * @param chl 通道号（仅启用该通道的数据保存）
     * @note 开启后，该通道解析后的协议数据会被写入文件/数据库，用于后续分析
     */
    void enableFactSave(int chl);

    /**
     * @brief 加载Excel格式的协议定义文件
     * @param file Excel文件路径（QString类型，支持绝对/相对路径）
     * @return 加载成功返回true，失败返回false（如文件不存在、格式错误、解析失败）
     * @details 1. 调用ParseProtoFile解析Excel文件中的协议帧定义
     *          2. 初始化frame_type_set（协议帧集合模型）
     *          3. 加载告警配置、设备状态参考值等附属信息
     */
    bool loadExcelProto(const QString &file);

    /**
     * @brief 单元测试专用：解析指定通道/类型的协议数据
     * @param chl 通道号
     * @param type 协议类型
     * @param buffer 原始二进制数据缓冲区指针（uint8_t类型）
     * @return 解析后的数据模型指针（FactTableModel，适配Qt Model-View）
     * @note 单元测试时调用，直接返回解析后的数据模型，方便验证解析结果
     */
    FactTableModel *unittestProto(int chl, int type, const uint8_t *buffer);
    ///
    /// \brief 解析原始协议数据，构建协议包并存入并发队列
    ///     直接构建协议包，数据存储到QUEUE对象中
    /// \param buffer   数据消息BUFFER 原始数据缓冲区指针（uint8_t类型，不可为空）
    /// \param size     数据消息大小 缓冲区数据大小（字节数，需大于0）
    /// \param channel  消息通道
    /// \param addr 数据来源地址（IP/串口地址等）
    /// \param port 数据来源端口（网络数据有效，串口可传0）
    /// \return 输入异常返回FALSE，异常分配空间返回False
    /// \note 静态方法，多线程环境下安全调用，负责原始数据到协议包的转换
    ///
    static bool parseProtoData(const uint8_t *buffer, int size, int channel, int addr, int port,
                               moodycamel::ConcurrentQueue<ProtoMessage *> &);

    ///
    /// \brief 封装原始数据为协议包，存入并发队列
    ///     直接构建协议包，数据存储到QUEUE对象中
    /// \param addr     消息来源地址（void*，兼容IP/串口等不同类型地址）
    /// \param buf      消息缓存区 指针（uint8_t类型）
    /// \param len      消息大小 （字节数）
    /// \param channel  消息通道
    /// \param queue 多线程安全的并发队列（存储构建后的ProtoMessage指针）
    /// \note 静态方法，仅做数据封装（不解析），将原始数据打包为ProtoMessage后入队
    ///
    static void dataPackage(const void *addr, const uint8_t *buf, int len, int channel,
                            moodycamel::ConcurrentQueue<ProtoMessage *> &);
    ///
    /// \brief dataDecoder
    ///     带地址的协议包解析，数据存储到QUEUE对象中
    /// \param decoder  消息缓存区域 （ProtoDataCache，存储待解码的原始数据）
    /// \param addr     消息来源地址 （void*）
    /// \param channel  消息通道
    /// \param queue 多线程安全的并发队列（存储解码后的ProtoMessage指针）
    /// \note 静态方法，带地址信息的解码，适用于需要区分数据来源的场景
    ///
    static void dataDecoder(ProtoDataCache &decoder, const void *addr, int channel,
                            moodycamel::ConcurrentQueue<ProtoMessage *> &);

    /**
     * @brief 解码协议包（重载：无地址信息），存入并发队列
     * @param decoder 协议数据缓存器
     * @param channel 数据通道号
     * @param queue 多线程安全的并发队列
     * @note 简化版解码方法，无需区分数据来源地址，适用于单来源场景
     */
    //TBD 什么是重载？
    static void dataDecoder(ProtoDataCache &decoder, int channel,
                            moodycamel::ConcurrentQueue<ProtoMessage *> &);

    /**
     * @brief 解析告警数据，处理不同级别的告警
     * @param warnConfig 告警配置结构体指针（包含告警阈值、级别、处理规则）
     * @param table 存储告警数据的模型（FactTableModel）
     * @param warn 告警级别（WarningLevel枚举：如无告警、普通告警、严重告警）
     * @param first 告警检查回调函数（处理告警触发后的逻辑）
     * @param chl 数据通道号
     * @note 核心告警处理逻辑，根据告警配置判断是否触发告警，并更新数据模型
     */
    void decodeWarnData(WarnConfig *, FactTableModel *table, WarningLevel warn,
                        CheckWarnCallback first, int chl);

    /// 协议文件包括多少个协议帧信息
    /**
     * @brief 获取协议帧集合模型
     * @return 协议帧集合模型引用（FactListModel）
     * @note 返回的模型包含所有加载的协议帧定义（来自Excel协议文件），可直接绑定到QML界面展示
     */
    FactListModel *frameSet() { return &frame_type_set; }

    /**
     * @brief 参考/校验设备运行条件
     * @param refer 设备条件参考结构体指针（包含运行条件、触发规则等）
     * @param chl 数据通道号
     * @return 条件满足返回true，不满足返回false
     * @note 验证设备是否满足指定运行条件（如温度、电压阈值）
     */
    bool referDeviceState(ReferenceValueCmp *refer, int chl);

    /**
     * @brief 参考/校验设备运行条件
     * @param refer 设备条件参考结构体指针（包含运行条件、触发规则等）
     * @param chl 数据通道号
     * @return 条件满足返回true，不满足返回false
     * @note 验证设备是否满足指定运行条件（如温度、电压阈值）
     */
    bool referDeviceCondition(ReferenceDevice *refer, int chl);

    /// 车辆索引与车辆地址信息绑定
    ///
    uint32_t getVehicleId() { return vehicleIndex; }

    /**
     * @brief 获取车辆地址（网络/硬件地址）
     * @return 车辆地址（uint32_t类型）
     * @note 车辆地址用于网络通信时标识目标车辆
     */
    uint32_t getVehicleAddr() { return vehicleAddr; }

    /**
     * @brief 根据协议帧名称获取对应的数据模型
     * @param name 协议帧名称（QString类型，与Excel中定义的帧名称一致）
     * @return 对应的数据模型指针（FactTableModel），未找到返回nullptr
     * @note 用于快速查找指定名称的协议帧解析结果
     */
    FactTableModel *protoframe(const QString &name) const;

    /**
     * @brief 根据协议帧类型获取对应的数据模型（重载）
     * @param type 协议帧类型（int类型，与Excel中定义的帧类型ID一致）
     * @return 对应的数据模型指针，未找到返回nullptr
     * @note 按类型查找比按名称更高效，适用于程序内部调用
     */
    FactTableModel *protoframe(int type) const;

    /**
     * @brief 获取设备告警配置组
     * @return 告警配置组指针（WarnConfigGroupVal）
     * @note 返回的配置组包含该车辆所有设备的告警规则，供解码告警时使用
     */
    WarnConfigGroupVal *deviceWarnConfigGroup();

  protected:
    /**
     * @brief 退出标志（原子布尔型）
     * @note 多线程安全的退出标记，用于通知解析线程停止运行，默认值false
     */
    std::atomic_bool exitflag = false;
    /**
     * @brief 协议文件的时间戳
     * @note 记录加载的Excel协议文件的生成时间，用于区分不同版本的协议
     */
    QDateTime file_datatime;
    /**
     * @brief 车辆地址（网络/硬件地址）
     * @note 用于网络通信时标识车辆，默认值0
     */
    uint32_t vehicleAddr = 0;
    /**
     * @brief 车辆空/地状态标识
     * @note 用于区分不同运行场景的协议解析规则
     */
    uint32_t vehAirGround = 0;
    /**
     * @brief 车辆索引（唯一标识）
     * @note uint8_t类型，范围0~255，用于区分多车辆场景下的不同车辆
     */
    uint8_t vehicleIndex = 0;
    /**
     * @brief 协议帧集合模型
     * @note 存储所有从Excel加载的协议帧定义，适配Qt Model-View架构
     */
    FactListModel frame_type_set;
    /**
     * @brief 协议文件解析器对象
     * @note 核心解析器，负责解析Excel格式的协议定义文件
     */
    ParseProtoFile proto_parser;

    /**
     * @brief 遥测数据统计回调函数
     * @note 默认值nullptr，通过initSummary()设置，用于上报解析统计数据
     */
    TelemetrySummary summary_telemetry = nullptr;
    /**
     * @brief 告警配置集合（智能指针管理）
     * @note std::shared_ptr自动管理内存，避免内存泄漏，存储该车辆所有告警规则
     */
    QVector<std::shared_ptr<WarnConfig>> warnings;
    /**
     * @brief 协议帧数据模型映射表
     * @param QMap Qt提供的有序键值对映射容器（替代C++标准库的 std::map）QMap<Key, T>
     * @param int 映射的键类型,代表 “协议数据通道号”
     * @param std::shared_ptr C++11 引入的共享智能指针
     * @param FactTableModel 协议解析结果的数据模型类（Qt Model-View 架构的核心）
     *        用于存储 / 展示某一通道的协议帧解析数据，可直接绑定到QML/Qt Widgets界面
     */
    QMap<int, std::shared_ptr<FactTableModel>> veh_proto_set;
};
